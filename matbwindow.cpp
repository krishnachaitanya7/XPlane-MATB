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
#include <QInputDialog>
#include <regex>
#include <QMenu>
#include <QString>
#include <QFileDialog>
#include <fstream>

// Start Global Static Variables
static QVBoxLayout *test;
static QWidget *my_button_window;
// End Global Static Variables

//Important Links
// https://regex101.com/r/y4qWeI/1

void send_message(QString send_msg){
    QMessageBox msgBox;
    msgBox.setText(send_msg);
    msgBox.exec();
}
QString get_config_string(QString rain_percent, QString wind_percent, QString duration_time, QString time_of_day){
    QString conf_string = "Rain % is "+rain_percent+" Wind % is "+wind_percent+" for "+duration_time+" seconds Time of the day is "+time_of_day;
    return conf_string;
}

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

    // Menu Actions
    connect(ui->menuXPlane_MATB, SIGNAL(triggered(QAction*)), this, SLOT(load_file_clicked(QAction*)));

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
    int new_rain_percent;
    int new_wind_percent;
    int new_duration;
//    std::cout << buttonText.toUtf8().constData() << std::endl;
    std::string button_text = buttonText.toUtf8().constData();
    std::regex rgx("Rain % is (\\d*) Wind % is (\\d*) for (\\d*) seconds Time of the day is (Day|Night)");
    std::smatch matches;
    std::regex_search(button_text, matches, rgx);
    bool ok {false};
    while(!ok){
    new_rain_percent = QInputDialog::getInt(this, "Enter new rain % input", "New Rain %: ", std::stoi(matches[1]), 0, 100, 1, &ok);
    if (!ok){send_message("Incorrect value inputted. Please try again.");}
    }
    ok = false;
    while(!ok){
    new_wind_percent = QInputDialog::getInt(this, "Enter new rain % input", "New Rain %: ", std::stoi(matches[2]), 0, 100, 1, &ok);
    if (!ok){send_message("Incorrect value inputted. Please try again.");}
    }
    ok = false;
    while(!ok){
    new_duration = QInputDialog::getInt(this, "Enter new rain % input", "New Rain %: ", std::stoi(matches[3]), 0, 100, 1, &ok);
    if (!ok){send_message("Incorrect value inputted. Please try again.");}
    }
    buttonSender->setText(get_config_string(QString::number(new_rain_percent), QString::number(new_wind_percent), QString::number(new_duration), "Day"));

}

void MATBWindow::on_Add_to_list_clicked()
{
    QString rain_percent = ui->rain_percent->toPlainText();
    QString wind_percent = ui->wind_percent->toPlainText();
    QString duration_time = ui->duration_time->toPlainText();
    QString time_of_day = ui->day_or_night->currentText();
    QRegExp re("\\d*");
    if (re.exactMatch(rain_percent) && re.exactMatch(wind_percent) && re.exactMatch(duration_time)){
        QString final_string = get_config_string(rain_percent, wind_percent, duration_time, time_of_day);
        QPushButton *button6 = new QPushButton(final_string);
        connect(button6, SIGNAL(clicked()), this, SLOT(dynamic_buttons_clicked()));
        test->addWidget(button6);
        ui->rain_percent->clear();
        ui->wind_percent->clear();
        ui->duration_time->clear();

    } else{
        send_message("You have not entered all numbers. Please make sure you enter only numbers.");
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

void MATBWindow::add_buttons_to_list(QString rain_percent, QString wind_percent, QString duration_time, QString time_of_day){
    QString final_string = get_config_string(rain_percent, wind_percent, duration_time, time_of_day);
    QPushButton *button6 = new QPushButton(final_string);
    connect(button6, SIGNAL(clicked()), this, SLOT(dynamic_buttons_clicked()));
    test->addWidget(button6);
}

void MATBWindow::load_file_clicked(QAction* test){
    QString fileName = QFileDialog::getOpenFileName(this, ("Open File"),
                                                      "/home",
                                                      ("Conf Files (*.conf)"));
    std::cout << fileName.toUtf8().constData() << std::endl;
    std::regex rgx("Rain % is (\\d*) Wind % is (\\d*) for (\\d*) seconds Time of the day is (Day|Night)");
    std::smatch matches;
    std::string str;
    std::ifstream input_file(fileName.toUtf8().constData());
    if (!input_file.good()){
        std::cout << "Unable to read file" << std::endl;
    }
    while (std::getline(input_file, str)) {
        if(std::regex_search(str, matches, rgx)){
            add_buttons_to_list(QString::fromStdString(matches[1]), QString::fromStdString(matches[2]), QString::fromStdString(matches[3]), QString::fromStdString(matches[4]));
        } else{
            QString custom_msg = QString::fromStdString("This line aint recognized by regex: "+str);
            send_message(custom_msg);

        }
    }
}
