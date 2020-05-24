import QtQuick 2.3
import QtQuick.Controls 1.2
import io.qt.torchutils 1.0

ApplicationWindow {
    id: root
    width: 640; height: 480

    TorUtils{
        id:torchutils
    }

    Button {
        id: predict_class
        x: 248
        y: 295
        width: 150
        height: 47
        text: qsTr("Predict")
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 138
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked:{
            textbox.text = torchutils.predict_output(textField.text)
            textField.enabled = false;
        }
    }

    Button {
        id: load_model
        x: 245
        y: 217
        width: 150
        height: 47
        text: qsTr("Load model")
        onClicked:{
            textbox.text = torchutils.load_model()
            textField.enabled = true;
        }
        Rectangle{
            x: -200
            y: -181
            width:550
            height:100
            color : "white"
            radius: 15
        }
        Text{
            id: textbox
            x: -183
            y: -170
            text : "Please load the model"
        }
    }

    TextField {
        id: textField
        x: 45
        y: 150
        width: 550
        height: 30
        placeholderText: qsTr("Enter image path")
        enabled: false
    }
}
