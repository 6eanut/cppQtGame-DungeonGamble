#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "shopdialog.h"
#include <QString>
#include <QMessageBox>
#include <cstdlib>
#include <ctime>
#include <QDebug>
#include <QScrollBar>
#include <QKeyEvent>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QCloseEvent>


MainWindow::MainWindow(QWidget *parent):QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->labelBossHP->hide();
    ui->btnAttack->hide();
    ui->btnDefend->hide();
    ui->btnNormal->hide();

    ui->stackedWidget->setCurrentIndex(0);

    srand(time(nullptr));
    correctMonster=getCorrectMonster();

    // 初始化玩家状态
    currentLevel =1;
    currentHP =100;
    gold =200;
    playerATK=30;
    bossMaxHP=60;
    bossHP=bossMaxHP;
    bossState=0;
    lastBossState=0;
    selectedIndex = 2;
    bossSelectedIndex = 2;
    maxLevel=1;
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path); // 确保文件夹存在

    QFile file(path + "/record.txt");

    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream in(&file);
        in >> maxLevel;
        file.close();
    }


    // 初始化 Label
    ui->labelLevel->setText(QString("第%1层").arg(currentLevel));
    updateHPUI();
    ui->labelGold->setText(QString("金币：%1").arg(gold));
    ui->labelATK->setText(QString("攻击力: %1").arg(playerATK));

    updateMonsterByLevel();
    updateMonsterDamageUI();
    updateMonsterGoldUI();
    updateSelectionUI();

    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();

    ui->btnMonster1->setFocusPolicy(Qt::NoFocus);
    ui->btnMonster2->setFocusPolicy(Qt::NoFocus);
    ui->btnMonster3->setFocusPolicy(Qt::NoFocus);

    ui->btnMonster1->setFocusPolicy(Qt::NoFocus);
    ui->btnMonster2->setFocusPolicy(Qt::NoFocus);
    ui->btnMonster3->setFocusPolicy(Qt::NoFocus);

    ui->btnShop->setFocusPolicy(Qt::NoFocus);
    ui->btnUseEliminate->setFocusPolicy(Qt::NoFocus);

    ui->btnAttack->setFocusPolicy(Qt::NoFocus);
    ui->btnDefend->setFocusPolicy(Qt::NoFocus);
    ui->btnNormal->setFocusPolicy(Qt::NoFocus);
}
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(ui->stackedWidget->currentIndex() != 1)
        return;

    //  按 S 进入商店
    if(event->key() == Qt::Key_S)
    {
        on_btnShop_clicked();
        return;
    }

    //  普通层
    if(!isBossFloor())
    {
        if(event->key() == Qt::Key_Left)
        {
            selectedIndex--;
            if(selectedIndex < 1) selectedIndex = 3;
            updateSelectionUI();
        }
        else if(event->key() == Qt::Key_Right)
        {
            selectedIndex++;
            if(selectedIndex > 3) selectedIndex = 1;
            updateSelectionUI();
        }
        else if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        {
            fightMonster(selectedIndex);
        }
    }
    //  Boss层
    else
    {
        if(event->key() == Qt::Key_Up)
        {
            bossSelectedIndex--;
            if(bossSelectedIndex < 1) bossSelectedIndex = 3;
            updateBossSelectionUI();
        }
        else if(event->key() == Qt::Key_Down)
        {
            bossSelectedIndex++;
            if(bossSelectedIndex > 3) bossSelectedIndex = 1;
            updateBossSelectionUI();
        }
        else if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        {
            fightBossRound(bossSelectedIndex);
        }
    }

    event->accept();
}
void MainWindow::on_startButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);

    this->setFocus();
}

void MainWindow::on_exitButton_clicked()
{
    close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::isBossFloor()
{
    return currentLevel %10==0;
}

// 点击怪物按钮1
void MainWindow::on_btnMonster1_clicked() {
    fightMonster(1);
}

// 点击怪物按钮2
void MainWindow::on_btnMonster2_clicked() {
    fightMonster(2);
}

// 点击怪物按钮3
void MainWindow::on_btnMonster3_clicked() {
    fightMonster(3);
}

void MainWindow::updateSelectionUI()
{
    // 先清空样式
    ui->btnMonster1->setStyleSheet("");
    ui->btnMonster2->setStyleSheet("");
    ui->btnMonster3->setStyleSheet("");

    // 根据 selectedIndex 高亮
    if(selectedIndex == 1)
        ui->btnMonster1->setStyleSheet("background-color: rgba(100,180,255,30); border:1px solid rgba(100,180,255,80);");
    else if(selectedIndex == 2)
        ui->btnMonster2->setStyleSheet("background-color: rgba(100,180,255,30); border:1px solid rgba(100,180,255,80);");
    else if(selectedIndex == 3)
        ui->btnMonster3->setStyleSheet("background-color: rgba(100,180,255,30); border:1px solid rgba(100,180,255,80);");
}
void MainWindow::updateBossSelectionUI()
{
    // 清空样式
    ui->btnAttack->setStyleSheet("");
    ui->btnDefend->setStyleSheet("");
    ui->btnNormal->setStyleSheet("");

    // 根据选中项高亮
    if(bossSelectedIndex == 1)
        ui->btnAttack->setStyleSheet("background-color: rgba(100,180,255,30); border:1px solid rgba(100,180,255,80);");
    else if(bossSelectedIndex == 2)
        ui->btnDefend->setStyleSheet("background-color: rgba(100,180,255,30); border:1px solid rgba(100,180,255,80);");
    else if(bossSelectedIndex == 3)
        ui->btnNormal->setStyleSheet("background-color: rgba(100,180,255,30); border:1px solid rgba(100,180,255,80);");
}
void MainWindow::updateMonsterByLevel()
{
    int phase = (currentLevel - 1) / 20 + 1;

    switch(phase) {
    case 1:
        monsters[0].damage = 10;  monsters[0].gold = 15;
        monsters[1].damage = 25;  monsters[1].gold = 30;
        monsters[2].damage = 50;  monsters[2].gold = 65;
        break;
    case 2:
        monsters[0].damage = 15;  monsters[0].gold = 20;
        monsters[1].damage = 35;  monsters[1].gold = 40;
        monsters[2].damage = 70;  monsters[2].gold = 80;
        break;
    case 3:
        monsters[0].damage = 20;  monsters[0].gold = 25;
        monsters[1].damage = 45;  monsters[1].gold = 50;
        monsters[2].damage = 90;  monsters[2].gold = 100;
        break;
    case 4:
        monsters[0].damage = 25;  monsters[0].gold = 30;
        monsters[1].damage = 55;  monsters[1].gold = 60;
        monsters[2].damage = 110; monsters[2].gold = 130;
        break;
    default:
        monsters[0].damage = 25;  monsters[0].gold = 30;
        monsters[1].damage = 55;  monsters[1].gold = 60;
        monsters[2].damage = 110; monsters[2].gold = 130;
        break;
    }
}
void MainWindow::resetMonsterButtons()
{
    ui->btnMonster1->setEnabled(true);
    ui->btnMonster2->setEnabled(true);
    ui->btnMonster3->setEnabled(true);

    updateSelectionUI();
}
// 战斗逻辑
void MainWindow::fightMonster(int monsterType) {
    if((monsterType == 1 && !ui->btnMonster1->isEnabled()) ||
        (monsterType == 2 && !ui->btnMonster2->isEnabled()) ||
        (monsterType == 3 && !ui->btnMonster3->isEnabled()))
    {
        return;
    }
    if(monsterType==1)ui->btnMonster1->setEnabled(false);
    else if(monsterType==2)ui->btnMonster2->setEnabled(false);
    else ui->btnMonster3->setEnabled(false);
    // 更新 UI 上怪物伤害
    updateMonsterDamageUI();
    updateMonsterGoldUI();
    int damage = 0;

    damage = monsters[monsterType-1].damage;

    currentHP -= damage;
    if(currentHP < 0) currentHP = 0;

    updateHPUI();

    // 选对怪物升层
    QString monsterName;

    if(monsterType==1)monsterName="小怪";
    else if(monsterType==2)monsterName="大怪";
    else monsterName="精英怪";
    int baseGold = monsters[monsterType-1].gold;
    int reward = baseGold;

    // 暴击机制
    int r = rand()%100;

    if(monsterType==1 && r < 10) reward = baseGold * 2;
    else if(monsterType==2 && r < 20) reward = baseGold * 2;
    else if(monsterType==3 && r < 30) reward = baseGold * 2;

    gold+=reward;

    ui->labelGold->setText(QString("金币：%1").arg(gold));

    if(monsterType == correctMonster) {
        ui->labelATK->setText(QString("攻击力: %1").arg(playerATK));
        QString rewardText;

        if(reward > baseGold)
            rewardText = QString("💥 %1（暴击！）").arg(reward);
        else
            rewardText = QString::number(reward);

        QMessageBox::information(this,"结果",QString("你选择了【%1】，选对了！\n获得金币：%2").arg(monsterName).arg(rewardText));
        addLog(QString("第%1层：选对【%2】 +%3金币")
                   .arg(currentLevel)
                   .arg(monsterName)
                   .arg(reward));
        currentLevel++;
        ui->labelLevel->setText(QString("第%1层").arg(currentLevel));

        playerATK+=1;
        addLog(QString("—— 第%1层 ——").arg(currentLevel));
        if(isBossFloor())
        {
            enterBossMode();
        }
        else
        {
            ui->btnMonster1->show();
            ui->btnMonster2->show();
            ui->btnMonster3->show();

            ui->labelBossHP->hide();
            ui->btnAttack->hide();
            ui->btnDefend->hide();
            ui->btnNormal->hide();

            ui->labelMonster1Damage->show();
            ui->labelMonster2Damage->show();
            ui->labelMonster3Damage->show();

            ui->labelMonster1Gold->show();
            ui->labelMonster2Gold->show();
            ui->labelMonster3Gold->show();

            ui->btnUseEliminate->show();
        }

        correctMonster=getCorrectMonster();
        updateMonsterByLevel();
        resetMonsterButtons();
        selectedIndex = 2;
        updateSelectionUI();
        this->setFocus();

        // 更新 UI 上怪物伤害
        updateMonsterDamageUI();
        updateMonsterGoldUI();
    }
    else
    {
        QString rewardText;

        if(reward > baseGold)
            rewardText = QString("💥 %1（暴击！）").arg(reward);
        else
            rewardText = QString::number(reward);

        QMessageBox::information(this,"结果",QString("你选择了【%1】，选错了！\n获得金币：%2").arg(monsterName).arg(rewardText));
        addLog(QString("第%1层：选错【%2】 -%3HP +%4金币")
                   .arg(currentLevel)
                   .arg(monsterName)
                   .arg(damage)
                   .arg(reward));
    }

    // 死亡判断
    if(currentHP <= 0) {
        GameOver();
        return;
    }
}

int MainWindow::getCorrectMonster()
{
    int r=rand()%100;

    if(r<25)return 1;
    else if(r<60)return 2;
    else return 3;
}
void MainWindow::on_btnShop_clicked()
{
    ShopDialog dialog(this);
    //  回血
    connect(&dialog, &ShopDialog::buyHeal, this, [=]() {
        if(gold >= 50)
        {
            gold -= 50;
            currentHP += 30;

            ui->labelGold->setText(QString("金币：%1").arg(gold));
            updateHPUI();

            QMessageBox::information(this, "商店", "回血成功！");
        }
        else
        {
            QMessageBox::information(this, "商店", "金币不足！");
        }
    });
    // 大生命药水
    connect(&dialog, &ShopDialog::buyBigHeal, this, [=]() {
        if(gold >= 150)
        {
            gold -= 150;
            currentHP += 120;

            ui->labelGold->setText(QString("金币：%1").arg(gold));
            updateHPUI();

            QMessageBox::information(this, "商店", "回血成功！");
        }
        else
        {
            QMessageBox::information(this, "商店", "金币不足！");
        }
    });

    //  提示卷轴
    connect(&dialog, &ShopDialog::buyHint, this, [=]() {
        if(gold >= 30)
        {
            gold -= 30;
            ui->labelGold->setText(QString("金币：%1").arg(gold));

            QString hint;
            if(correctMonster == 1) hint = "小怪";
            else if(correctMonster == 2) hint = "大怪";
            else hint = "精英怪";

            QMessageBox::information(this, "提示",QString("正确的是：%1").arg(hint));
        }
        else
        {
            QMessageBox::information(this, "商店", "金币不足！");
        }
    });

    //  排除卷轴
    connect(&dialog, &ShopDialog::buyEliminate, this, [=]() {
        if(gold >= 15)
        {
            gold -= 15;
            hasEliminate = true;

            ui->labelGold->setText(QString("金币：%1").arg(gold));

            QMessageBox::information(this, "商店", "你获得了一张【排除卷轴】！");
        }
        else
        {
            QMessageBox::information(this, "商店", "金币不足！");
        }
    });

    //  攻击药水
    connect(&dialog, &ShopDialog::buyAtkPotion, this, [=]() {
        if(!isBossFloor())
        {
            QMessageBox::information(this, "商店", "只能在Boss战中使用！");
            return;
        }

        if(bossAtkBuff)
        {
            QMessageBox::information(this, "商店", "攻击药水已经生效了！");
            return;
        }

        if(gold >= 80)
        {
            gold -= 80;
            bossAtkBuff = true;

            ui->labelGold->setText(QString("金币：%1").arg(gold));

            QMessageBox::information(this, "商店", "攻击药水已生效（本次Boss战）！");
        }
        else
        {
            QMessageBox::information(this, "商店", "金币不足！");
        }
    });
    // 防御药水
    connect(&dialog, &ShopDialog::buyDefPotion, this, [=]() {
        if(!isBossFloor())
        {
            QMessageBox::information(this, "商店", "只能在Boss战中使用！");
            return;
        }

        if(bossDefBuff)
        {
            QMessageBox::information(this, "商店", "防御药水已经生效了！");
            return;
        }

        if(gold >= 100)
        {
            gold -= 100;
            bossDefBuff = true;

            ui->labelGold->setText(QString("金币：%1").arg(gold));

            QMessageBox::information(this, "商店", "防御药水已生效（本次Boss战）！");
        }
        else
        {
            QMessageBox::information(this, "商店", "金币不足！");
        }
    });

    dialog.exec();
    this->setFocus();
}

void MainWindow::on_helpButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_backButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::updateMonsterDamageUI()
{
    // 只显示已经存在的怪物属性
    ui->labelMonster1Damage->setText(QString("伤害: %1").arg(monsters[0].damage));
    ui->labelMonster2Damage->setText(QString("伤害: %1").arg(monsters[1].damage));
    ui->labelMonster3Damage->setText(QString("伤害: %1").arg(monsters[2].damage));
}

void MainWindow::updateMonsterGoldUI()
{
    ui->labelMonster1Gold->setText(QString("基础金币: %1").arg(monsters[0].gold));
    ui->labelMonster2Gold->setText(QString("基础金币: %1").arg(monsters[1].gold));
    ui->labelMonster3Gold->setText(QString("基础金币: %1").arg(monsters[2].gold));
}
void MainWindow::enterBossMode()
{
    bossAtkBuff=false;
    bossDefBuff=false;
    bossMaxHP=10+currentLevel*5;
    bossHP=bossMaxHP;
    ui->labelLevel->setText(QString("第%1层（boss）").arg(currentLevel));
    ui->labelBossHP->setText(QString("Boss血量:%1").arg(bossHP));

    ui->btnMonster1->hide();
    ui->btnMonster2->hide();
    ui->btnMonster3->hide();

    ui->labelBossHP->show();
    ui->btnAttack->show();
    ui->btnDefend->show();
    ui->btnNormal->show();

    ui->labelMonster1Damage->hide();
    ui->labelMonster2Damage->hide();
    ui->labelMonster3Damage->hide();

    ui->labelMonster1Gold->hide();
    ui->labelMonster2Gold->hide();
    ui->labelMonster3Gold->hide();

    ui->btnUseEliminate->hide();


    QMessageBox::information(this,"提示","boss出现了！");

    bossSelectedIndex = 2;
    updateBossSelectionUI();
}

void MainWindow::GameOver()
{
    if(currentLevel > maxLevel)
    {
        maxLevel = currentLevel;
    }
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);

    QFile file(path + "/record.txt");

    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);
        out << maxLevel;
        file.close();
    }
    else
    {
        qDebug() << "写入失败！";
    }

    QMessageBox::information(this,"Game Over","你死了！");

    ui->stackedWidget->setCurrentIndex(0);

    currentLevel=1;
    currentHP=100;
    gold=200;
    playerATK=30;
    bossAtkBuff=false;
    ui->labelATK->setText(QString("攻击力: %1").arg(playerATK));

    ui->labelLevel->setText(QString("第%1层").arg(currentLevel));
    updateHPUI();
    ui->labelGold->setText(QString("金币：%1").arg(gold));

    battleLogs.clear();
    ui->textLog->clear();

    resetMonsterButtons();

    ui->btnMonster1->show();
    ui->btnMonster2->show();
    ui->btnMonster3->show();

    ui->labelBossHP->hide();
    ui->btnAttack->hide();
    ui->btnDefend->hide();
    ui->btnNormal->hide();

    ui->btnUseEliminate->show();
    ui->labelHP->setStyleSheet("");

    updateMonsterByLevel();
    updateMonsterDamageUI();
    updateMonsterGoldUI();
}

void MainWindow::fightBossRound(int playerChoice)
{
    //随机boss状态
    int r = rand() % 100;

    if(lastBossState == 1) // 上一回合是蓄力
    {
        // 下一回合大概率变普通
        if(r < 70) bossState = 0;
        else bossState = 2;
    }
    else if(lastBossState == 2) // 上一回合是防御
    {
        if(r < 70) bossState = 0;
        else bossState = 1;
    }
    else // 上一回合是普通
    {
        if(r < 40) bossState = 1;
        else if(r < 80) bossState = 2;
        else bossState = 0;
    }
    //  Boss针对玩家（核心升级）
    int adjust = rand() % 100;

    if(playerChoice == 1) // 玩家强攻
    {
        if(adjust < 40) bossState = 2; // 更容易防御
    }
    else if(playerChoice == 2) // 玩家防御
    {
        if(adjust < 40) bossState = 1; // 更容易蓄力
    }
    // 稳定输出不针对

    double playerDamageFactor = 1.0;
    if(bossAtkBuff)
    {
        playerDamageFactor*=1.5;
    }
    double playerTakenFactor = 1.0;
    if(bossDefBuff)
    {
        playerTakenFactor *= 0.6;
    }

    // 玩家选择影响
    if(playerChoice == 1) // 强攻
    {
        playerDamageFactor*= 1.5;
        playerTakenFactor*= 1.3;
    }
    else if(playerChoice == 2) // 防御
    {
        playerDamageFactor *= 0.5;
        playerTakenFactor *= 0.5;
    }

    // Boss状态影响
    double bossDamageFactor = 1.0;
    double bossDefenseFactor = 1.0;

    if(bossState == 1) // 蓄力
    {
        bossDamageFactor *= 1.6;
    }
    else if(bossState == 2) // 防御
    {
        bossDamageFactor *= 0.7;
        bossDefenseFactor *= 0.5;
    }

    // 计算伤害
    double damageToBoss = playerATK * playerDamageFactor * bossDefenseFactor;
    double bossATK= 20+currentLevel*1.5;
    double damageToPlayer = bossATK * bossDamageFactor * playerTakenFactor;

    bossHP -= damageToBoss;
    currentHP -= damageToPlayer;

    if(currentHP < 0) currentHP = 0;
    if(bossHP < 0) bossHP = 0;

    updateHPUI();
    if(bossAtkBuff && bossDefBuff)
    {
        ui->labelBossHP->setText(
            QString("Boss血量: %1（攻↑ 防↑）").arg(bossHP)
            );
    }
    else if(bossAtkBuff)
    {
        ui->labelBossHP->setText(
            QString("Boss血量: %1（攻击↑）").arg(bossHP)
            );
    }
    else if(bossDefBuff)
    {
        ui->labelBossHP->setText(
            QString("Boss血量: %1（防御↑）").arg(bossHP)
            );
    }
    else
    {
        ui->labelBossHP->setText(QString("Boss血量: %1").arg(bossHP));
    }
    // 提示
    QString msg;

    // 第一层：战斗感觉
    if(damageToPlayer > 40)
    {
        msg = "Boss的攻势突然变得极其凶猛，你有些招架不住！\n";
    }
    else if(damageToBoss < 15)
    {
        msg = "Boss似乎早有准备，你的攻击很难奏效…\n";
    }
    else
    {
        msg = "双方你来我往，战斗陷入僵持。\n";
    }

    //  第二层：状态暗示（核心）
    if(bossState == 1)
    {
        msg += "你隐约感觉到，Boss的动作变得有些激进。\n";
    }
    else if(bossState == 2)
    {
        msg += "Boss的动作显得十分谨慎。\n";
    }
    else
    {
        msg += "Boss保持着稳定的节奏。\n";
    }

    // 第三层：数值反馈
    msg += QString("\n你对Boss造成 %1 伤害\nBoss对你造成 %2 伤害")
               .arg(damageToBoss)
               .arg(damageToPlayer);

    QMessageBox::information(this, "战斗结果", msg);
    addLog(QString("Boss战：你造成%1伤害，受到%2伤害")
               .arg((int)damageToBoss)
               .arg((int)damageToPlayer));

    //  胜负判断
    if(currentHP <= 0)
    {
        GameOver();
        return;
    }

    if(bossHP <= 0)
    {
        QMessageBox::information(this,"胜利","你击败了Boss！");
        addLog(QString("第%1层Boss被击败！+150金币").arg(currentLevel));

        int reward = 100 + currentLevel * 10;
        gold += reward;
        bossAtkBuff=false;

        ui->labelGold->setText(QString("金币：%1").arg(gold));

        currentLevel++;
        playerATK += 5 + currentLevel / 10;
        ui->labelATK->setText(QString("攻击力: %1").arg(playerATK));
        ui->labelLevel->setText(QString("第%1层").arg(currentLevel));

        // 恢复普通模式
        ui->btnMonster1->show();
        ui->btnMonster2->show();
        ui->btnMonster3->show();
        resetMonsterButtons();

        ui->labelBossHP->hide();
        ui->btnAttack->hide();
        ui->btnDefend->hide();
        ui->btnNormal->hide();

        ui->labelMonster1Damage->show();
        ui->labelMonster2Damage->show();
        ui->labelMonster3Damage->show();

        ui->labelMonster1Gold->show();
        ui->labelMonster2Gold->show();
        ui->labelMonster3Gold->show();

        ui->btnUseEliminate->show();

        correctMonster = getCorrectMonster();
        updateMonsterByLevel();
        updateMonsterDamageUI();
        updateMonsterGoldUI();

        return;
    }
    lastBossState=bossState;
}

void MainWindow::on_btnAttack_clicked()
{
    fightBossRound(1);
}


void MainWindow::on_btnDefend_clicked()
{
    fightBossRound(2);
}


void MainWindow::on_btnNormal_clicked()
{
    fightBossRound(3);
}


void MainWindow::on_btnUseEliminate_clicked()
{
    if(!hasEliminate)
    {
        QMessageBox::information(this, "提示", "你没有排除卷轴！");
        return;
    }

    hasEliminate = false;

    int fakeMonster;
    do
    {
        fakeMonster = rand() % 3 + 1;
    } while(fakeMonster == correctMonster);

    QString fakeName;
    if(fakeMonster == 1) fakeName = "小怪";
    else if(fakeMonster == 2) fakeName = "大怪";
    else fakeName = "精英怪";

    QMessageBox::information(this, "排除触发", QString("你排除了【%1】（该选项不是正确答案）").arg(fakeName));
}
void MainWindow::addLog(QString text)
{
    ui->textLog->append(text);

    while(ui->textLog->document()->blockCount() > 50)
    {
        QTextCursor cursor(ui->textLog->document());
        cursor.movePosition(QTextCursor::Start);
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.removeSelectedText();
        cursor.deleteChar();
    }
}
void MainWindow::updateHPUI()
{
    ui->labelHP->setText(QString("当前血量: %1").arg(currentHP));

    if(currentHP <= 50)
    {
        ui->labelHP->setStyleSheet("color:red; font-weight:bold;");
    }
    else
    {
        ui->labelHP->setStyleSheet("");
    }
}
void MainWindow::on_btnRecord_clicked()
{
    QMessageBox::information(this, "历史记录",QString("你的历史最高层数是：%1层").arg(maxLevel));
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(currentLevel > maxLevel)
    {
        maxLevel = currentLevel;
    }

    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(path);

    QFile file(path + "/record.txt");

    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);
        out << maxLevel;
        file.close();
    }

    event->accept();
}
