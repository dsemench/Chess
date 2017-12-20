import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2

Item {
    Button {
      id: startGame
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.leftMargin: 10
      anchors.rightMargin: 10

      text: "New game"

      onClicked: {
        logic.new_game();
        stopGame.visible = true
        saveGame.visible = true
        prev.visible = true
        next.visible = true
        turn.visible = true
        startGame.visible = false
        loadGame.visible = false
      }
    }

    Button {
      id: loadGame
      anchors.top: startGame.bottom
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.topMargin: 40
      anchors.leftMargin: 10
      anchors.rightMargin: 10

      text: "Load game"

      onClicked: fileLoad.open()
    }

    Button {
      id: prev
      visible: false
      anchors.top: loadGame.bottom
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.topMargin: 40
      anchors.leftMargin: 10
      anchors.rightMargin: 10

      text: "Prev"

      onClicked: {
        if(!logic.prev())
            infoPrev.visible = true
      }
    }

    Button {
      id: next
      visible: false
      anchors.top: prev.bottom
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.topMargin: 40
      anchors.leftMargin: 10
      anchors.rightMargin: 10

      text: "Next"

      onClicked: {
          if (!logic.next()) {
                infoNext.visible = true
          }
      }
    }

    Button {
      id: stopGame
      visible: false
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.topMargin: 40
      anchors.leftMargin: 10
      anchors.rightMargin: 10

      text: "Stop game"

      onClicked: {
        logic.clear();
        stopGame.visible = false
        saveGame.visible = false
        prev.visible = false
        next.visible = false
        turn.visible = false
        startGame.visible = true
        loadGame.visible = true

      }
    }

    Button {
      id: saveGame
      visible: false
      anchors.top: stopGame.bottom
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.topMargin: 40
      anchors.leftMargin: 10
      anchors.rightMargin: 10

      text: "Save game"

      onClicked: fileSave.open()
    }

    Column {
        id : turn
        visible: false
        anchors.top: next.bottom
        anchors.topMargin: 40
        anchors.left: parent.left
        anchors.right: parent.right
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 16
            color: "black"
            text: "<b>Turn</b>"
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            font.pointSize: 16
            color: "black"
            text: logic.playerTurn ? "White" : "Black"
        }
    }

    FileDialog {
        id: fileLoad
        title: "Load file"
        folder: source
        nameFilters: ["Text files (*.ch)"]
        onAccepted: {
            if (!logic.load_game(fileLoad.fileUrl))
              errorLoad.visible = true
            else {
              stopGame.visible = true
              saveGame.visible = true
              prev.visible = true
              next.visible = true
              turn.visible = true
              startGame.visible = false
              loadGame.visible = false
            }
        }
    }

    FileDialog {
        id: fileSave
        title: "Load file"
        folder: source
        nameFilters: ["Text files (*.ch)"]
        selectExisting: false
        onAccepted: {
            if (logic.save_game(fileSave.fileUrl))
                gameSaved.visible = true
            else
                errorSave.visible = true
        }
    }

    MessageDialog {
        id: gameSaved
        icon: StandardIcon.Information
        text: "Your game has been saved"
        standardButtons: StandardButton.Ok
        onAccepted: visible = false
    }

    MessageDialog {
        id: errorSave
        icon: StandardIcon.Warning
        text: "Your game wasn't been saved"
        standardButtons: StandardButton.Ok
        onAccepted: visible = false
    }

    MessageDialog {
        id: errorLoad
        icon: StandardIcon.Warning
        text: "Can't load game"
        standardButtons: StandardButton.Ok
        onAccepted: visible = false
    }

    MessageDialog {
        id: infoPrev
        icon: StandardIcon.Information
        text: "You're at the beginning of the game"
        standardButtons: StandardButton.Ok
        onAccepted: visible = false
    }

    MessageDialog {
        id: infoNext
        icon: StandardIcon.Information
        text: "This is the end point of the moves in the game"
        standardButtons: StandardButton.Ok
        onAccepted: visible = false
    }
}
