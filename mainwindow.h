#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QStringList>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    struct Monster {
        QString name;
        int hp;
        int damage;
        int gold;
        bool isCorrect;
    };

    Monster monsters[3];


    // 小箭头数组
    QLabel* labelMonsterHint[3];

private slots:
    void on_btnMonster1_clicked();
    void on_btnMonster2_clicked();
    void on_btnMonster3_clicked();

    void on_btnShop_clicked();

    void on_startButton_clicked();

    void on_exitButton_clicked();

    void on_helpButton_clicked();

    void on_backButton_clicked();

    void on_backButton_2_clicked();

    void on_btnAttack_clicked();

    void on_btnDefend_clicked();

    void on_btnNormal_clicked();

    void on_btnUseEliminate_clicked();

    void on_btnRecord_clicked();

protected:
    void keyPressEvent(QKeyEvent *event);
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;

    int currentLevel;
    double currentHP;
    int gold;
    double playerATK;
    double bossHP;
    double bossMaxHP;
    int bossState;
    int lastBossState;
    int selectedIndex;
    int bossSelectedIndex;

    void fightMonster(int monsterType); // 战斗函数
    void updateMonsterDamageUI();
    void updateMonsterGoldUI();
    bool isBossFloor();
    void enterBossMode();
    void GameOver();
    int getCorrectMonster();
    int correctMonster;
    void fightBossRound(int playerChoice);
    void updateMonsterByLevel();
    bool hasEliminate=false;
    void resetMonsterButtons();
    bool bossAtkBuff=false;
    bool bossDefBuff = false;
    void addLog(QString text);
    QStringList battleLogs;
    void updateHPUI();
    void updateSelectionUI();
    void updateBossSelectionUI();
    void enterShopMode();
    void handleShopBuy(int);
    int maxLevel;

    // 3.0 美化
    bool bgmMuted = false;
    void playBGM(const QString &fp);
    void stopBGM();
    void toggleBGM();
    QString findFile(const QString &rel);
    void shake(QWidget *t);
    void flash(QWidget *t);
};


#endif // MAINWINDOW_H
