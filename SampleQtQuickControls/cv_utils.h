#ifndef CVUTILS_H
#define CVUTILS_H

#include <QObject>
#include <QString>

class cvUtils: public QObject
{
    Q_OBJECT
public:
    cvUtils(QObject* parent = nullptr);
    Q_INVOKABLE QString preprocess_image();
};

#endif // CVUTILS_H


