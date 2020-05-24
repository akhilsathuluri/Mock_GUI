import QtQuick 2.3
import QtQuick.Controls 1.2
import io.qt.camerautils 1.0

ApplicationWindow {
    id: root
    width: 640; height: 480

    CamUtils{
    id: camutils
    }

    Button {
        id: checkStatus
        x: 248
        y: 295
        width: 150
        height: 47
        text: qsTr("Check camera status")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 138
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked:{
            textbox.text = camutils.find_device()
            console.log(  )
            console.log(torchutils.load_model())
        }
        Rectangle{
          x: -200
          y: -250
        width:550
        height:100
        color : "white"
        radius: 15
        }
        Text{
        id: textbox
        x: -190
        y: -240
        text : "Camera not Found"
        }
    }
}
