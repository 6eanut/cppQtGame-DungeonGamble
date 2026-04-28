#ifndef SHOPDIALOG_H
#define SHOPDIALOG_H

#include <QDialog>

namespace Ui {
class ShopDialog;
}

class ShopDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShopDialog(QWidget *parent = nullptr);
    ~ShopDialog();
protected:
    void keyPressEvent(QKeyEvent *event) override;
signals:
    void buyHeal();
    void buyBigHeal();
    void buyHint();
    void buyEliminate();
    void buyAtkPotion();
    void buyDefPotion();
private slots:
    void on_btnBuyHeal_clicked();

    void on_btnBuyHint_clicked();

    void on_btnBigHeal_clicked();

    void on_btnEliminate_clicked();

    void on_buyAtkPotion_clicked();

    void on_buyDefPotion_clicked();

private:
    Ui::ShopDialog *ui;
};

#endif // SHOPDIALOG_H
