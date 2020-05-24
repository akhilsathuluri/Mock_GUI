import QtQuick 2.3
import QtQuick.Controls 1.2
import io.qt.cvutils 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 640
    height: 480

    CvUtils{
        id:cvutils
    }

    Button {
        id: predict
        x: 245
        y: 57
        width: 150
        height: 47
        text: qsTr("Predict")
        onClicked: {
            var component = Qt.createComponent("predictp.qml")
            console.log("Component Status:", component.status, component.errorString())
            var window = component.createObject(root)
            window.show()

        }
    }

    Grid {
        id: grid
        x: 28
        y: 129
        width: 600
        height: 169
        anchors.verticalCenterOffset: 17
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        transformOrigin: Item.Center
        z: 0
        rotation: 0
        scale: 1
        spacing: 76
        flow: Grid.LeftToRight
        columns: 3

        Button {
            id: logb
            width: 150
            height: 47
            text: qsTr("Log")
        }

        Button {
            id: reportb
            width: 150
            height: 47
            text: qsTr("Report")
        }

        Button {
            id: statusb
            width: 150
            height: 47
            text: qsTr("Status")
        }

        Button {
            id: settingsb
            width: 150
            height: 47
            text: qsTr("Settings")
        }

        Button {
            id: camerab
            width: 150
            height: 47
            text: qsTr("Camera")
            onClicked: {
                var component = Qt.createComponent("camerap.qml")
                console.log("Component Status:", component.status, component.errorString())
                var window = component.createObject(root)
                window.show()
            }
        }

        Button {
            id: extrab
            width: 150
            height: 47
            text: qsTr("Extra")
            onClicked: {
                cvutils.preprocess_image()
            }
        }

    }

}

