#pragma once
#include <memory>
#include <QAbstractListModel>

class Logic: public QAbstractListModel
{
    Q_OBJECT
public:

    enum GlobalConstants {
        BOARD_SIZE = 8
    };

    enum Roles {
        Type = Qt::UserRole,
        PositionX,
        PositionY,
    };
    
public:
    explicit Logic(QObject *parent = 0);
    ~Logic();

    Q_PROPERTY(int boardSize READ boardSize CONSTANT)
    int boardSize() const;
    Q_PROPERTY(bool playerTurn READ getTurn NOTIFY turnChanged)
    bool    getTurn() const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void new_game();
    Q_INVOKABLE bool load_game(QString filename);
    Q_INVOKABLE bool save_game(QString filename);
    Q_INVOKABLE bool next();
    Q_INVOKABLE bool prev();
    Q_INVOKABLE bool move(int fromX, int fromY, int toX, int toY);

signals:
    void turnChanged();
protected:

    int rowCount(const QModelIndex & parent) const override;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
    bool playerTurn;

    bool checkMoveErrors(int start_pos, int end_pos);
    bool choose_action(int index, int fromX, int fromY, int toX, int toY);

    bool action_pawn(int fromX, int fromY, int toX, int toY);
    bool action_knight(int fromX, int fromY, int toX, int toY);
    bool action_bishop(int fromX, int fromY, int toX, int toY);
    bool action_rook(int fromX, int fromY, int toX, int toY);
    bool action_queen(int fromX, int fromY, int toX, int toY);
    bool action_king(int fromX, int fromY, int toX, int toY);
};
