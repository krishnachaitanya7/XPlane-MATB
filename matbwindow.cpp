#include "matbwindow.h"
#include "ui_matbwindow.h"
#include <QVBoxLayout>
#include <QScrollArea>

// Start Global Static Variables
static QVBoxLayout *test;
// End Global Static Variables

MATBWindow::MATBWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MATBWindow)
{
    ui->setupUi(this);    
    QWidget *window = new QWidget;
    QVBoxLayout *events_list = new QVBoxLayout;
    test = events_list;
    QScrollArea *scroll = new QScrollArea;
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setWidget(window);
    ui->events_list->setWidget(window);
    window->setLayout(events_list);
    window->show();
}

MATBWindow::~MATBWindow()
{
    delete ui;
}

void MATBWindow::on_close_button_clicked()
{
    this->close();
}

void MATBWindow::on_Add_to_list_clicked()
{
    QPushButton *button6 = new QPushButton("Test");
    test->addWidget(button6);

}

void MATBWindow::on_Gen_script_clicked()
{

}
