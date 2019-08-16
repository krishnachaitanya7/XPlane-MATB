#include "rest_dialog.h"
#include "ui_rest_dialog.h"
#include <QTimer>


static int elapsed_seconds;

rest_dialog::rest_dialog(QWidget *parent, int elapsed_time) :
    QDialog(parent),
    ui(new Ui::rest_dialog)
{
    ui->setupUi(this);
    elapsed_seconds = elapsed_time;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this, SLOT(MyTimerSlot()));
    timer->start(1000);
    ui->progressBar->setMaximum(elapsed_time);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(elapsed_seconds);
    ui->countdown_timer->setText(QString::number(elapsed_seconds)+" Seconds");
}

rest_dialog::~rest_dialog()
{
    delete ui;
}

void rest_dialog::MyTimerSlot(){
    ui->countdown_timer->setText(QString::number(elapsed_seconds)+" Seconds");
    ui->progressBar->setValue(elapsed_seconds);
    --elapsed_seconds;
    if (elapsed_seconds == 0){
        this->close();
    }
}
