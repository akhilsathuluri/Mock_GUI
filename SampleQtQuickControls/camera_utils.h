#ifndef CAMERA_UTILS_H
#define CAMERA_UTILS_H

#include <QObject>
#include <QString>

class camera_utils : public QObject
{
    Q_OBJECT

public:
    explicit camera_utils(QObject *parent = nullptr);
    Q_INVOKABLE int check_status();

public slots:

private:

};

#endif // CAMERA_UTILS_H
