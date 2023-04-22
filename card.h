#ifndef CARD_H
#define CARD_H

#include <QObject>
#include <QPushButton>
#include <QList>

class Card: public QPushButton
{
    Q_OBJECT
public:
    Card(QString name = nullptr);

    QString name;
};

#endif // CARD_H
