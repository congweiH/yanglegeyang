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
    bool isInSlot;  // 是否在消除卡槽内
    int clickNum;   // 在卡槽被点击的次数
};

#endif // CARD_H
