#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QtGlobal>
#include <QLabel>
#include "card.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void distributionCards();   // 分布卡片
    bool isTop(Card *card);            // 卡片是否可以被点击
    void dissolveCards();       // 消除卡片
    int canDissolve();         // 可以消除
    int canDissolveTwo(QMap<QString, QList<int>> map, QList<QString> name);  // 是否可以组合消除两个
    void cardBeClicked();       // card被点击
    void deleteCard(QList<int> idxs);     // 消除dissolveCardContainer下标为i的卡片


private slots:
    void on_showRankBtn_clicked();

    void on_startGameBtn_clicked();

public:
    int level;          // 0 是第一关，1 是第二关
    int score;          // 分数
    int cardNum;    // 初始卡片数量

    QList<Card*> *dissolveCardContainer;    // 消除卡牌的容器

    QLabel *usernameLabel;
    QLabel *scoreLabel;
    QLabel *levelLabel;

    const int CARD_SIZE = 64;    // 图片大小 64px
    QString names[20] = {
        "case",
        "char",
        "class",
        "default",
        "do",
        "double",
        "else",
        "enum",
        "float",
        "for",
        "if",
        "int",
        "long",
        "private",
        "protected",
        "public",
        "struct",
        "switch",
        "union",
        "while"
    };

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
