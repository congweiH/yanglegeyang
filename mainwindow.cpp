#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QUrl>
#include <QPainter>
#include <QPixmap>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/img/icon.png"));

    this->setFixedSize(800, 600);
    this->setWindowTitle("羊了个羊");

    // 播放音乐
    QAudioOutput *audioOutput = new QAudioOutput(this);
    QMediaPlayer *mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setSource(QUrl::fromLocalFile("bgm.mp3"));
    mediaPlayer->setLoops(-1);  // 循环播放
    mediaPlayer->play();

    // 生成随机种子
    srand(QTime(0,0,0).secsTo(QTime::currentTime()));

    // 初始化排名Label
    scoreRackList = new QList<QLabel*>();
    scoreRackList->append(ui->scorelabel1);
    scoreRackList->append(ui->scorelabel2);
    scoreRackList->append(ui->scorelabel3);
    scoreRackList->append(ui->scorelabel4);
    scoreRackList->append(ui->scorelabel5);
    scoreRackList->append(ui->scorelabel6);
    scoreRackList->append(ui->scorelabel7);
    scoreRackList->append(ui->scorelabel8);
    scoreRackList->append(ui->scorelabel9);
    scoreRackList->append(ui->scorelabel10);

    // 初始化成绩排名
    scoreList = new QList<QPair<QString, QString>>();
    // 读取文件
    this->readFile();

    // 初始化底部状态栏
    levelLabel = new QLabel();
    usernameLabel = new QLabel();
    scoreLabel = new QLabel();

    ui->statusbar->addWidget(levelLabel);
    ui->statusbar->addWidget(usernameLabel);
    ui->statusbar->addWidget(scoreLabel);

    this->dissolveCardContainer = new QList<Card*>();
    this->initCardContainer = new QList<Card*>();
    this->isSelectedList = new QList<Card*>();
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::on_showRankBtn_clicked()
{
    this->toRankPage();
}

void MainWindow::distributionCards(int level) {
    initCardContainer->clear();
    if (level == 1) {
        this->createCards(5);
    } else if (level == 2) {
        this->createCards(20);
    }
}

void MainWindow::createCards(int num)
{
    for(int i = 0; i < num; i++) {
        int cardIdx = rand() % 20;
        for (int i = 0; i < 3; i++) {
            Card* card = new Card(this->names[cardIdx]);
            initCardContainer->append(card);
            // 绑定点击事件
            connect(card, &Card::clicked, this, &MainWindow::cardBeClicked);

            card->setParent(ui->stackedWidget->currentWidget());
            card->move(rand() % 700, rand() % (ui->stackedWidget->height() - 200));
            card->show();   // 显示出来
        }
    }
    this->cardNum = initCardContainer->size();
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
    qDebug() << "可以点击";
    this->initCardContainer->removeOne(card);

    // 判断是否在卡槽内
    if(card->isInSlot) {
        this->toggleHighLight(card);
        return;
    }

    // 如果点击的还是class, 并且存在高亮的，并且高亮的不是class
    if (card->name == "class" && this->isSelectedList->size() > 0 && this->isSelectedList->at(0)->name != "class") {
        delete card;
        this->dissolveCardContainer->removeOne(this->isSelectedList->at(0));
        delete this->isSelectedList->at(0);
        this->isSelectedList->clear();

        this->cardNum -= 2;

        this->score += 2;
        scoreLabel->setText(QString("分数：%1  ").arg(this->score));
    } else {
        card->isInSlot = true;
        dissolveCardContainer->append(card);
        card->move(dissolveCardContainer->size() * CARD_SIZE, 450);

        this->dissolveCards();
    }

    // 卡片向左对齐
    for(int i = 0; i < dissolveCardContainer->size(); i++) {
        dissolveCardContainer->at(i)->move((i+1) * CARD_SIZE, 450);
    }

    qDebug() << this->cardNum << this->dissolveCardContainer->size();

    // 判断是否赢了
    if (this->cardNum == 0) {
        qDebug() << "win";
        if(this->level == 1) {
            // 如果是第一关，那么就跳转到第二关
            this->levelTow();
        } else if(this->level == 2) {
            // 如果是第二关，那么就结束游戏，保存成绩
            this->saveScore();
            // 跳转到排行榜
            this->toRankPage();
        }
    } else if(this->cardNum == this->dissolveCardContainer->size() || this->dissolveCardContainer->size() > 7) {
        qDebug() << "没有卡片可以选择了";
        this->saveScore();
        // 如果没有卡牌可以选了，但是卡槽还存在卡片, 获取卡槽的
        if(QMessageBox::Ok == QMessageBox::information(this, "提示", "游戏失败")) {
            this->toHomePage();
        }
    }
}

void MainWindow::toggleHighLight(Card* card)
{
    if(this->isSelectedList->size() == 0) {
        // 没有被选中的，则加入
        this->isSelectedList->append(card);
        // 高亮
        card->setStyleSheet("background-color: yellow;");
    } else {
        // 如果已经有高亮的
        if(this->isSelectedList->at(0) == card) {
            qDebug() << "是自己";
            // 如果就是自己, 则取消高亮
            card->setStyleSheet("background-color: none;");
            this->isSelectedList->clear();
        } else {
            // 如果是其他的
            qDebug() << "是其他的";
            // 先将之前的清除掉
            this->isSelectedList->at(0)->setStyleSheet("background-color: none;");
            this->isSelectedList->clear();
            // 添加新的
            card->setStyleSheet("background-color: yellow;");
            this->isSelectedList->append(card);
        }
    }
}

// 第二关
void MainWindow::levelTow()
{
    this->level = 2;
    // 切换页面
    ui->stackedWidget->setCurrentIndex(2);
    // 更改第几关
    levelLabel->setText("第二关 ");

    this->distributionCards(2);
}

void MainWindow::saveScore()
{
    qDebug() << "saveScore";
    // 将当前的玩家姓名和成绩添加到容器中
    this->scoreList->append(QPair<QString, QString>(this->username, QString("%1").arg(this->score)));
    qDebug() << this->scoreList;
    // 排序
    std::sort(this->scoreList->begin(), this->scoreList->end(), [](QPair<QString, QString> a, QPair<QString, QString> b) {
        return a.second.toInt() > b.second.toInt();
    });
    // 如果超出了10个，则讲后面的全部去掉
    while(this->scoreList->size() > 10) {
        this->scoreList->removeLast();
    }

    // 打开文件，如果不存在则会自动创建新的同名的文件
    QFile file(this->fileName);
    file.open(QIODevice::WriteOnly);

    QDataStream out(&file);

    for (int i = 0; i < this->scoreList->size(); i++) {
        out << this->scoreList->at(i).first << this->scoreList->at(i).second;
    }

    file.close();
}

void MainWindow::showScore()
{
    qDebug() << "showScore";
    qDebug() << this->scoreList->size();
    for (int i = 0; i < this->scoreList->size(); i++) {
        qDebug() << this->scoreList->at(i).first << this->scoreList->at(i).second;
        this->scoreRackList->at(i)->setText(QString("%1. %2 %3").arg(i + 1).arg(this->scoreList->at(i).first).arg(this->scoreList->at(i).second));
    }
}

void MainWindow::readFile()
{
    qDebug() << "readFile";
    QFile file(this->fileName);
    file.open(QIODevice::ReadOnly);

    QList<QString> list;

    QDataStream in(&file);
    while (!in.atEnd()) {
        QByteArray line;
        in >> line;
        list.append(line.data());
    }
    file.close();

    for (int i = 0; i < list.size(); i += 2) {
        this->scoreList->append(QPair<QString, QString>(list.at(i), list.at(i+1)));
    }
}

void MainWindow::toRankPage()
{
    ui->stackedWidget->setCurrentIndex(3);

    levelLabel->setText("排行榜");

    // 展示分数
    this->showScore();
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

void MainWindow::clearAll()
{
    this->cardNum -= dissolveCardContainer->size();
    for(int i = dissolveCardContainer->size() - 1; i >= 0; i--) {
        Card* card = dissolveCardContainer->at(i);
        dissolveCardContainer->removeAt(i);
        delete card;
    }
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

    // 两张class消除所有
    if (map["class"].size() >= 2) {
        this->clearAll();
        return 15;
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
    this->username = ui->usernamelineEdit->text();
    if (this->username.isEmpty()) {
        ui->usernamelineEdit->setStyleSheet("border: 1px solid red;");
        qDebug() << "用户名为空";
        return;
    }

    this->level = 1;    // 第一关
    this->score = 0;

    // 设置底部状态栏文本
    levelLabel->setText("第一关  ");
    usernameLabel->setText(QString("用户名：%1  ").arg(this->username));
    scoreLabel->setText(QString("分数：%1  ").arg(this->score));

    // 切换页面
    ui->stackedWidget->setCurrentIndex(1);

    // 分布卡片
    this->distributionCards(1);
}

// 回到主页面
void MainWindow::on_toHomeAction_triggered()
{
    this->toHomePage();
}

void MainWindow::toHomePage()
{
    qDebug() << "回到首页";
    // 改变界面
    ui->stackedWidget->setCurrentIndex(0);

    ui->usernamelineEdit->clear();

    for(int i = 0; i < this->initCardContainer->size(); i++) {
        delete this->initCardContainer->at(i);
    }

    for(int i = 0; i < this->dissolveCardContainer->size(); i++) {
        delete this->dissolveCardContainer->at(i);
    }

    this->initCardContainer->clear();
    this->dissolveCardContainer->clear();
    this->isSelectedList->clear();

    levelLabel->setText("");
    usernameLabel->setText("");
    scoreLabel->setText("");

    this->score = 0;
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    QPainter *painter = new QPainter(this);
    QPixmap pic = QPixmap(":/bg.jpg");
    painter->drawPixmap(0, 0, this->width(), this->height(), pic);
}
