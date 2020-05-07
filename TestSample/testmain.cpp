#include <QApplication>
#include <QLabel>
#include <QWidget>

int main(int argc, char *argv[ ]){

QApplication app(argc, argv);
QLabel hello("<center> Welcome to my Qt program </center>");
hello.setWindowTitle("Ttile one");
hello.resize(400,400);
hello.show();
return app.exec();

}
