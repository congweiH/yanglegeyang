#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setFixedSize(800, 600);
    this->setWindowTitle("羊了个羊");

    // 生成随机种子
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::on_showRankBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::distributionCards() {

}

// 判断card是否是最上层的card
bool MainWindow::isTop(Card *card)
{
    Card *cardTemp = (Card*) ui->stackedWidget->currentWidget()->childAt(card->geometry().topLeft());
    if (card != cardTemp) {
        return false;
    }
    cardTemp = (Card *) ui->stackedWidget->currentWidget()->childAt(card->geometry().bottomLeft());
    if (card != cardTemp) {
        return false;
    }
    cardTemp = (Card *) ui->stackedWidget->currentWidget()->childAt(card->geometry().topRight());
    if (card != cardTemp) {
        return false;
    }
    cardTemp = (Card *) ui->stackedWidget->currentWidget()->childAt(card->geometry().bottomRight());
    if (card != cardTemp) {
        return false;
    }
    return true;
}

// 当鼠标左键点击了card
void MainWindow::cardBeClicked()
{
    // 被点击的card
    Card *card = (Card*)sender();
    if (!this->isTop(card)) {
        qDebug() << "无法点击";
        return;
    }
    // 如果是最上层的card
    qDebug() << "yes";
    dissolveCardContainer->append(card);
    card->move(dissolveCardContainer->size() * CARD_SIZE, 450);

    this->dissolveCards();
}

void MainWindow::deleteCard(QList<int> idxs)
{
    std::sort(idxs.begin(), idxs.end());
    for(int i = idxs.size() - 1; i >= 0; i--) {
        Card* card = dissolveCardContainer->at(idxs.at(i));
        dissolveCardContainer->removeAt(idxs.at(i));
        delete card;
    }
    this->cardNum -= idxs.size();
}

// 消除卡片
void MainWindow::dissolveCards()
{
    qDebug() << "dissolveCards";
    int scoreDelta = this->canDissolve();
    if(!scoreDelta) {
        qDebug() << "不能消除";
        return;
    }
    qDebug() << "可以消除" << scoreDelta;
    this->score += scoreDelta;
    scoreLabel->setText(QString("分数：%1  ").arg(this->score));

    // 卡片向左对齐
    for(int i = 0; i < dissolveCardContainer->size(); i++) {
        dissolveCardContainer->at(i)->move((i+1) * CARD_SIZE, 450);
    }

    // 判断是否赢了
    if (this->cardNum == 0) {
        qDebug() << "win";
    }
}

int MainWindow::canDissolve()
{
    // 先按照名称放在map中
    QMap<QString, QList<int>> map;
    for(int i = 0; i < dissolveCardContainer->size(); i++) {
        map[dissolveCardContainer->at(i)->name].append(i);
    }
    qDebug() << map;
    QList<QString> keyList = map.keys();
    qDebug() << keyList;
    // 先判断是否有三个一样的
    for(int i = 0; i < keyList.size(); i++) {
        if(map[keyList[i]].size() >= 3) {
            QList<int> idxs;
            idxs.append(map[keyList[i]].at(0));
            idxs.append(map[keyList[i]].at(1));
            idxs.append(map[keyList[i]].at(2));
            this->deleteCard(idxs);
            return 5;       // 消除3个记作5分
        }
    }

    // 判断能不能消除两个
    // if & else
    QList<QString> name1 = {"if", "else"};
    int score = this->canDissolveTwo(map, name1);
    if (score) {
        return score;
    }

    // public private protected
    QList<QString> name2 = {"public", "private", "protected"};
    score = this->canDissolveTwo(map, name2);
    if (score) {
        return score;
    }

    // struct union enum
    QList<QString> name3 = {"struct", "union", "enum"};
    score = this->canDissolveTwo(map, name3);
    if (score) {
        return score;
    }

    // switch case default
    QList<QString> name4 = {"switch", "case", "default"};
    score = this->canDissolveTwo(map, name4);
    if (score) {
        return score;
    }

    // float char int long double
    QList<QString> name5 = {"float", "char", "int", "long", "double"};
    score = this->canDissolveTwo(map, name5);
    if (score) {
        return score;
    }

    // do for while
    QList<QString> name6 = {"do", "for", "while"};
    score = this->canDissolveTwo(map, name6);
    if (score) {
        return score;
    }

    return 0;   // 不能消除
}

int MainWindow::canDissolveTwo(QMap<QString, QList<int>> map, QList<QString> name)
{
    QList<QString> keyList = map.keys();
    for(int i = 0; i < name.size(); i++) {
        for(int j = i + 1; j < name.size(); j++) {
            if(keyList.contains(name[i]) && keyList.contains(name[j]) && map[name[i]].size() >= 1 && map[name[j]].size() >= 1) {
                QList<int> idxs;
                idxs.append(map[name[i]].at(0));
                idxs.append(map[name[j]].at(0));
                this->deleteCard(idxs);
                return 2;
            }
        }
    }
    return 0;
}

// 开始游戏按钮
void MainWindow::on_startGameBtn_clicked()
{
    // 校验输入框
    if (ui->usernamelineEdit->text().isEmpty()) {
        ui->usernamelineEdit->setStyleSheet("border: 1px solid red;");
        qDebug() << "用户名为空";
        return;
    }


    this->level = 0;    // 第一关
    this->dissolveCardContainer = new QList<Card*>();
    this->score = 0;

    // 展示分数和用户名
    levelLabel = new QLabel("第一关  ");
    usernameLabel = new QLabel(QString("用户名：%1  ").arg(ui->usernamelineEdit->text()));
    scoreLabel = new QLabel(QString("分数：%1  ").arg(this->score));

    ui->statusbar->addWidget(levelLabel);
    ui->statusbar->addWidget(usernameLabel);
    ui->statusbar->addWidget(scoreLabel);

    // 切换页面
    ui->stackedWidget->setCurrentIndex(1);

    this->cardNum = 10;

    for(int i = 0; i < this->cardNum; i++) {
        int imgIdx = qrand() % 20;
        Card* card = new Card(this->names[imgIdx]);
        // 绑定点击事件
        connect(card, &Card::clicked, this, &MainWindow::cardBeClicked);

        card->setParent(ui->stackedWidget->currentWidget());
        card->move(qrand() % 700, qrand() % (ui->stackedWidget->height() - 200));
        card->show();   // 显示出来
    }
}


