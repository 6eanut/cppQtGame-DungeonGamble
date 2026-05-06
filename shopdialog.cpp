#include "shopdialog.h"
#include "ui_shopdialog.h"
#include <QKeyEvent>

ShopDialog::ShopDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ShopDialog)
{
    ui->setupUi(this);
}

ShopDialog::~ShopDialog()
{
    delete ui;
}

void ShopDialog::on_btnBuyHeal_clicked()
{
    emit buyHeal();
}


void ShopDialog::on_btnBuyHint_clicked()
{
    emit buyHint();
}


void ShopDialog::on_btnBigHeal_clicked()
{
    emit buyBigHeal();
}


void ShopDialog::on_btnEliminate_clicked()
{
    emit buyEliminate();
}


void ShopDialog::on_buyAtkPotion_clicked()
{
    emit buyAtkPotion();
}

void ShopDialog::on_buyDefPotion_clicked()
{
    emit buyDefPotion();
}

void ShopDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_S)
    {
        this->close();   // 关闭商店
        return;
    }

    QDialog::keyPressEvent(event);
}
