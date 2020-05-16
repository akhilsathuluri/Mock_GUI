#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>


class cameraUtils : public QObject
{
    Q_OBJECT
public:
    cameraUtils(QObject* parent = nullptr);
//    Q_INVOKABLE void onButtonClicked();
    Q_INVOKABLE QString result();
signals:
    void echo();
};

#endif // BACKEND_H
