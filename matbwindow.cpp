#include "matbwindow.h"
#include "ui_matbwindow.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QRegExp>
#include <QMessageBox>
#include <iostream>
#include <QFile>
#include <QIODevice>
#include <QTextStream>

// Start Global Static Variables
static QVBoxLayout *test;
static QWidget *my_button_window;
// End Global Static Variables

//Important Links
// https://regex101.com/r/y4qWeI/1

MATBWindow::MATBWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MATBWindow)
{
    // Setting Up the Scroll Area
    ui->setupUi(this);    
    QWidget *window = new QWidget;
    QVBoxLayout *events_list = new QVBoxLayout;
    test = events_list;
    my_button_window = window;
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

void MATBWindow::dynamic_buttons_clicked(){
    QPushButton* buttonSender = qobject_cast<QPushButton*>(sender()); // retrieve the button you have clicked
    QString buttonText = buttonSender->text();
    std::cout << buttonText.toUtf8().constData() << std::endl;

}

void MATBWindow::on_Add_to_list_clicked()
{
    QString rain_percent = ui->rain_percent->toPlainText();
    QString wind_percent = ui->wind_percent->toPlainText();
    QString duration_time = ui->duration_time->toPlainText();
    QString time_of_day = ui->day_or_night->currentText();
    QRegExp re("\\d*");
    if (re.exactMatch(rain_percent) && re.exactMatch(wind_percent) && re.exactMatch(duration_time)){
        QString final_string = "Rain % is "+rain_percent+" Wind % is "+wind_percent+" for "+duration_time+" seconds Time of the day is "+time_of_day;
        QPushButton *button6 = new QPushButton(final_string);
        connect(button6, SIGNAL(clicked()), this, SLOT(dynamic_buttons_clicked()));
        test->addWidget(button6);
        ui->rain_percent->clear();
        ui->wind_percent->clear();
        ui->duration_time->clear();

    } else{
        QMessageBox msgBox;
        msgBox.setText("You have not entered all numbers. Please make sure you enter only numbers.");
        msgBox.exec();
    }


}

void MATBWindow::on_Gen_script_clicked()
{
    QString file_name = "XPlane.conf";
    QFile file(file_name);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    QList<QPushButton *> butts = my_button_window->findChildren<QPushButton *>();
    for (const auto *but: butts) {
        out << but->text();
        out << "\n";
    }
}
