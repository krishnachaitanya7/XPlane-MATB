#include "matbwindow.h"
#include "ui_matbwindow.h"

MATBWindow::MATBWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MATBWindow)
{
    ui->setupUi(this);
}

MATBWindow::~MATBWindow()
{
    delete ui;
}
