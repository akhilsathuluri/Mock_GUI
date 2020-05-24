#ifndef TORCHUTILS_H
#define TORCHUTILS_H

#include <QObject>
#include <QString>

class torchUtils: public QObject
{
    Q_OBJECT
public:
    torchUtils(QObject* parent = nullptr);
    Q_INVOKABLE QString load_model();
    Q_INVOKABLE QString predict_output(QString text);
};

#endif // TORCHUTILS_H
