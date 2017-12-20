#include "logic.h"
#include <QList>
#include <QByteArray>
#include <QHash>
#include <QFile>
#include <QDebug>

struct Figure
{
  int type;
  int x;
  int y;
};

struct Logic::Impl
{
  QList<Figure> figures;

  int idx_steps;
  QList<QString> saved_steps;

  int findByPosition(int x, int y);
  void saveStep(int fromX, int fromY, int toX, int toY);
};

int Logic::Impl::findByPosition(int x, int y) {
  for (int i(0); i<figures.size(); ++i) {
    if (figures[i].x != x || figures[i].y != y ) { 
      continue;
    }
    return i;
  }
  return -1;
}

void Logic::Impl::saveStep(int fromX, int fromY, int toX, int toY) {
    QString data_save = "";

    if ((idx_steps != saved_steps.size()))
        saved_steps.erase(saved_steps.begin() + idx_steps, saved_steps.end());
    int end_pos = findByPosition(toX, toY);

    data_save += QString::number(fromX) + " " + QString::number(fromY) + " ";
    data_save += QString::number(toX) + " " + QString::number(toY) + " ";
    data_save += QString::number(end_pos < 0 ? -1 : figures[end_pos].type);
    saved_steps << data_save;
    idx_steps++;
}

Logic::Logic(QObject *parent)
  : QAbstractListModel(parent)
  , impl(new Impl()) {
}

Logic::~Logic() {
}

int Logic::boardSize() const {
  return BOARD_SIZE;
}

int Logic::rowCount(const QModelIndex & ) const {
  return impl->figures.size() ;
}

QHash<int, QByteArray> Logic::roleNames() const {
  QHash<int, QByteArray> names;

  names.insert(Roles::Type      , "type");
  names.insert(Roles::PositionX , "positionX");
  names.insert(Roles::PositionY , "positionY");
  return names;
}

QVariant Logic::data(const QModelIndex & modelIndex, int role) const { 
  if (!modelIndex.isValid()) {
    return QVariant();
  }
    
  int index = static_cast<int>(modelIndex.row());
  
  if (index >= impl->figures.size() || index < 0) {
    return QVariant();
  }

  Figure & figure = impl->figures[index];
    
  switch (role) {
    case Roles::Type     : return figure.type;
    case Roles::PositionX: return figure.x;
    case Roles::PositionY: return figure.y;
  }
  return QVariant(); 
}

void Logic::clear() {
  beginResetModel();
  impl->idx_steps = 0;
  impl->saved_steps.clear();
  impl->figures.clear();
  endResetModel();
}

void Logic::new_game(){
    impl->idx_steps = 0;
    beginResetModel();

    if (!impl->figures.empty())
        impl->figures.clear();

    playerTurn = true;
    emit turnChanged();
    for (int i = 0; i < 8; i++){
        impl->figures << Figure { 0, i, 1};
        impl->figures << Figure { 6, i, 6};
    }

    impl->figures << Figure { 1,  1, 0 };
    impl->figures << Figure { 1,  6, 0 };
    impl->figures << Figure { 2,  2, 0 };
    impl->figures << Figure { 2,  5, 0 };
    impl->figures << Figure { 3,  0, 0 };
    impl->figures << Figure { 3,  7, 0 };
    impl->figures << Figure { 4,  3, 0 };
    impl->figures << Figure { 5,  4, 0 };

    impl->figures << Figure { 7,  1, 7 };
    impl->figures << Figure { 7,  6, 7 };
    impl->figures << Figure { 8,  2, 7 };
    impl->figures << Figure { 8,  5, 7 };
    impl->figures << Figure { 9,  0, 7 };
    impl->figures << Figure { 9,  7, 7 };
    impl->figures << Figure { 10, 3, 7 };
    impl->figures << Figure { 11, 4, 7 };

    endResetModel();
}

bool Logic::load_game(QString filename) {
    filename.remove(0, filename.lastIndexOf("/") + 1);
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream istream(&file);
    if (!impl->saved_steps.empty()) {
        clear();
    }
    QStringList pcs;
    if (!istream.atEnd()) {
        pcs = (istream.readLine()).split(" ");

        beginResetModel();
        playerTurn = pcs[0].toInt();
        emit turnChanged();
        for (int i = 1; (i + 1) < pcs.size(); ) {
            impl->figures << Figure { pcs[i++].toInt(), pcs[i++].toInt(), pcs[i++].toInt() };
        }
        endResetModel();
    }
    while (!istream.atEnd()) {
        impl->saved_steps << istream.readLine();
        impl->idx_steps++;
    }
    file.close();
    return true;
}

bool Logic::save_game(QString filename) {
    filename.remove(0, filename.lastIndexOf("/") + 1);
    QFile file(filename);

    if (file.exists()) {
        file.remove();
    }
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    QTextStream ostream(&file);
    ostream << playerTurn << " ";
    for (int i = 0; i < impl->figures.size(); i++) {
        ostream << impl->figures[i].type << " "
                << impl->figures[i].x << " "
                << impl->figures[i].y << " ";
    }
    ostream << endl;
    for (int i = 0; (i < impl->saved_steps.size()) && (i < impl->idx_steps); i++) {
        ostream << impl->saved_steps[i] << endl;
    }
    file.close();
    return true;
}

bool Logic::prev() {
    if (impl->idx_steps > 0) {
        impl->idx_steps--;
        QStringList pcs = impl->saved_steps[impl->idx_steps].split(" ");
        int figure_idx = impl->findByPosition(pcs[2].toInt(), pcs[3].toInt());

        beginResetModel();
        impl->figures[figure_idx].x = pcs[0].toInt();
        impl->figures[figure_idx].y = pcs[1].toInt();
        if (pcs[4].toInt() >= 0)
            impl->figures << Figure { pcs[4].toInt(), pcs[2].toInt(), pcs[3].toInt() };
        playerTurn = !playerTurn;
        emit turnChanged();
        endResetModel();
        return true;
    }
    return false;
}

bool Logic::next() {
    if (impl->idx_steps < impl->saved_steps.size()) {
        QStringList pcs = impl->saved_steps[impl->idx_steps].split(" ");
        int figure_idx = impl->findByPosition(pcs[0].toInt(), pcs[1].toInt());

        beginResetModel();
        if (pcs[4].toInt() >= 0)
            impl->figures.removeAt(impl->findByPosition(pcs[2].toInt(), pcs[3].toInt()));
        impl->figures[figure_idx].x = pcs[2].toInt();
        impl->figures[figure_idx].y = pcs[3].toInt();
        endResetModel();
        playerTurn = !playerTurn;
        emit turnChanged();
        impl->idx_steps++;
        return true;
    }
    return false;
}

bool Logic::getTurn() const {
    return playerTurn;
}

bool Logic::move(int fromX, int fromY, int toX, int toY) {
  int start_pos = impl->findByPosition(fromX, fromY);
  int end_pos = impl->findByPosition(toX, toY);
  
  if (toX < 0 || toX >= BOARD_SIZE || toY < 0 || toY >= BOARD_SIZE) {
    return false;
  }

  if (!checkMoveErrors(start_pos, end_pos)
          || !choose_action(impl->figures[start_pos].type, fromX, fromY, toX, toY)) {
      return false;
  }

  impl->saveStep(fromX, fromY, toX, toY);
  playerTurn = !playerTurn;
  emit turnChanged();
  impl->figures[start_pos].x = toX;
  impl->figures[start_pos].y = toY;

  QModelIndex topLeft = createIndex(start_pos, 0);
  QModelIndex bottomRight = createIndex(start_pos, 0);
  emit dataChanged(topLeft, bottomRight);
  beginResetModel();
  if (end_pos != -1) {
      impl->figures.removeAt(end_pos);
  }
  endResetModel();
  return true;
}

bool Logic::checkMoveErrors(int start_pos, int end_pos) {
    if (start_pos < 0) return false;

    if ((impl->figures[start_pos].type >= 6 && !playerTurn)
            || (impl->figures[start_pos].type <= 5 && playerTurn)) {
        return false;
    }

    if (end_pos != -1 && ((impl->figures[end_pos].type >= 6 && playerTurn)
                          || (impl->figures[end_pos].type <= 5 && !playerTurn))) {
        return false;
    }
    return true;
}

bool Logic::choose_action(int figure,int fromX, int fromY, int toX, int toY) {
    switch (figure >= 6 ? figure - 6 : figure) {
        case 0: return action_pawn(fromX, fromY, toX, toY);
        case 1: return action_knight(fromX, fromY, toX, toY);
        case 2: return action_bishop(fromX, fromY, toX, toY);
        case 3: return action_rook(fromX, fromY, toX, toY);
        case 4: return action_queen(fromX, fromY, toX, toY);
        case 5: return action_king(fromX, fromY, toX, toY);
        default: return false;
    }
}

bool Logic::action_pawn(int fromX, int fromY, int toX, int toY) {
    int end_pos = impl->findByPosition(toX, toY);
    int diff_Y = abs(fromY - toY);
    int diff_X = abs(fromX - toX);

    if (end_pos == -1 && diff_X == 0 && diff_Y <= 2) {
        int midle_step = impl->findByPosition(toX, toY + (1 * (playerTurn ? 1 : -1)));
        if (diff_Y == 1) {
            return true;
        }
        else if (fromY == (playerTurn ? 6 : 1) && diff_Y == 2 && midle_step == -1) {
            return true;
        }
    }
    else {
        if (end_pos != -1) {
            int is_enemy = impl->figures[end_pos].type;
            if (diff_X == 1 && diff_Y == 1 && is_enemy <= (playerTurn ? 5 : 11)) {
                return true;
            }
        }
    }
    return false;
}

bool Logic::action_knight(int fromX, int fromY, int toX, int toY) {
    bool res = (abs(fromY - toY) == 2 && abs(fromX - toX) == 1)
            || (abs(fromY - toY) == 1 && abs(fromX - toX) == 2);
    return res;
}

bool Logic::action_bishop(int fromX, int fromY, int toX, int toY) {
    int operation_X = toX > fromX ? 1 : -1;
    int operation_Y = toY > fromY ? 1 : -1;

    fromX += operation_X;
    fromY += operation_Y;
    while ((toX != fromX) && (toY != fromY)) {
        if (impl->findByPosition(fromX, fromY) != -1) {
            return false;
        }
        fromX += operation_X;
        fromY += operation_Y;
    }
    return (toX == fromX && toY == fromY);
}

bool Logic::action_rook(int fromX, int fromY, int toX, int toY) {
    if (fromX == toX) {
        int operation_Y = toY > fromY ? 1 : -1;
        fromY += operation_Y;
        while (toY != fromY) {
            if (impl->findByPosition(fromX, fromY) != -1) {
                return false;
            }
            fromY += operation_Y;
        }
    }
    else if (fromY == toY) {
        int operation_X = toX > fromX ? 1 : -1;
        fromX += operation_X;
        while (toX != fromX) {
            if (impl->findByPosition(fromX, fromY) != -1) {
                return false;
            }
            fromX += operation_X;
        }
    }
    return (toX == fromX && toY == fromY);
}

bool Logic::action_queen(int fromX, int fromY, int toX, int toY) {
    return (action_bishop(fromX, fromY, toX, toY)
            || action_rook(fromX, fromY, toX, toY));
}

bool Logic::action_king(int fromX, int fromY, int toX, int toY) {
    return (abs(fromY - toY) <= 1 && abs(fromX - toX) <= 1);
}
