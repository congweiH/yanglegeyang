#include "card.h"
#include <QDebug>

Card::Card(QString name)
{
    // 加载图片
    QPixmap pix;
    bool res = pix.load(QString(":/img/%1.png").arg(name));
    if(!res) {
        // 图片加载失败
        return;
    }
    this->name = name;
    this->setFixedSize(pix.width(), pix.height());
    qDebug() << pix.width() << " " << pix.height();
    this->setIcon(pix);
    this->setIconSize(QSize(pix.width(), pix.height()));
}
