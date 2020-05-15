import QtQuick 2.3
import QtQuick.Controls 1.2
import com.camera_utils 1.0

ApplicationWindow {
    id: root
    width: 640; height: 480

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
        onClicked: {
            cameraUtils.check_status();
        }
    }
}

