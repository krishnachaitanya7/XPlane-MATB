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
#include <QPalette>

// Start Global Static Variables
static QVBoxLayout *test;
static QWidget *my_button_window;
static QPushButton *last_clicked_button {nullptr};
// End Global Static Variables
// Start Constant Declaration. Only edit here
static int duration_ld {20};
static int rain_ld {0};
static int wind_ld {0};
static QString day_night_ld = "Day";
static int duration_md {20};
static int rain_md {50};
static int wind_md {50};
static QString day_night_md = "Day";
static int duration_hd {20};
static int rain_hd {99};
static int wind_hd {99};
static QString day_night_hd = "Day";
static int rest_seconds {15};
static QString low_difficulty {"Low Difficulty"};
static QString moderate_difficulty {"Moderate Difficulty"};
static QString high_difficulty {"High Difficulty"};
static QString insert_tlx {"Fill Out Survey"};
// End constanats declaration

//Important Links
// https://regex101.com/r/sCM1wj/1

QString difficulty_string_generator(QString difficulty, QString rain_percent, QString wind_percent, QString duration, QString day_or_night){
    QString return_string = difficulty + " Rain % is "+rain_percent+" Wind % is "+wind_percent+" for "+duration+" seconds Time of the day is "+day_or_night;
    return return_string;
}

void MATBWindow::set_all_defaults(){
    // Setting all default values
    // Generated from https://www.tablesgenerator.com/text_tables
    /* Please keep the table updated folks!
        +---------------------+----------+--------+--------+
        | Condition           | Duration | Rain % | Wind % |
        +---------------------+----------+--------+--------+
        | Low Difficulty      | 20       | 0      | 0      |
        +---------------------+----------+--------+--------+
        | Moderate Difficulty | 20       | 50     | 50     |
        +---------------------+----------+--------+--------+
        | High Difficulty     | 20       | 99     | 99     |
        +---------------------+----------+--------+--------+
        | Rest                | 15       | NA     | NA     |
        +---------------------+----------+--------+--------+
    */
    ui->duration_time_ld->setText(QString::number(duration_ld));
    ui->duration_time_md->setText(QString::number(duration_md));
    ui->duration_time_hd->setText(QString::number(duration_hd));
    ui->rain_percent_ld->setText(QString::number(rain_ld));
    ui->rain_percent_md->setText(QString::number(rain_md));
    ui->rain_percent_hd->setText(QString::number(rain_hd));
    ui->wind_percent_ld->setText(QString::number(wind_ld));
    ui->wind_percent_md->setText(QString::number(wind_md));
    ui->wind_percent_hd->setText(QString::number(wind_hd));
    ui->duration_time_rest->setText(QString::number(rest_seconds));
}

void send_message(QString send_msg){
    QMessageBox msgBox;
    msgBox.setText(send_msg);
    msgBox.exec();
}
QString get_config_string(QString rain_percent, QString wind_percent, QString duration_time, QString time_of_day){
    QString conf_string = "Rain % is "+rain_percent+" Wind % is "+wind_percent+" for "+duration_time+" seconds Time of the day is "+time_of_day;
    return conf_string;
}

void MATBWindow::dynamic_buttons_clicked(){
    QPushButton* buttonClicked = qobject_cast<QPushButton*>(sender()); // retrieve the button you have clicked
    last_clicked_button = buttonClicked;
    QPalette pal = buttonClicked->palette();
    pal.setColor(QPalette::Button, QColor(Qt::gray));
    buttonClicked->setAutoFillBackground(true);
    buttonClicked->setPalette(pal);
    buttonClicked->update();
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
    set_all_defaults();
    // Menu Actions
    connect(ui->menuXPlane_MATB, SIGNAL(triggered(QAction*)), this, SLOT(load_file_clicked(QAction*)));

}

MATBWindow::~MATBWindow()
{
    delete ui;
}

void MATBWindow::on_set_all_default_clicked(){
    set_all_defaults();
}

void MATBWindow::on_close_button_clicked(){
    this->close();
}

void MATBWindow::on_sel_ld_clicked(){
    QString rain_percent = ui->rain_percent_ld->toPlainText();
    QString wind_percent = ui->wind_percent_ld->toPlainText();
    QString duration_time = ui->duration_time_ld->toPlainText();
    QString time_of_day = ui->day_or_night_ld->currentText();
    QRegExp re("\\d*");
    if (re.exactMatch(rain_percent) && re.exactMatch(wind_percent) && re.exactMatch(duration_time)){
        QPushButton *button6 = new QPushButton(low_difficulty);
        connect(button6, SIGNAL(clicked()), this, SLOT(dynamic_buttons_clicked()));
        test->addWidget(button6);
    } else{
        send_message("You have not entered all numbers. Please make sure you enter only numbers.");
    }
}

void MATBWindow::on_sel_md_clicked(){
    QString rain_percent = ui->rain_percent_md->toPlainText();
    QString wind_percent = ui->wind_percent_md->toPlainText();
    QString duration_time = ui->duration_time_md->toPlainText();
    QString time_of_day = ui->day_or_night_md->currentText();
    QRegExp re("\\d*");
    if (re.exactMatch(rain_percent) && re.exactMatch(wind_percent) && re.exactMatch(duration_time)){
        QPushButton *button6 = new QPushButton(moderate_difficulty);
        connect(button6, SIGNAL(clicked()), this, SLOT(dynamic_buttons_clicked()));
        test->addWidget(button6);
    } else{
        send_message("You have not entered all numbers. Please make sure you enter only numbers.");
    }
}

void MATBWindow::on_sel_hd_clicked(){
    QString rain_percent = ui->rain_percent_hd->toPlainText();
    QString wind_percent = ui->wind_percent_hd->toPlainText();
    QString duration_time = ui->duration_time_hd->toPlainText();
    QString time_of_day = ui->day_or_night_hd->currentText();
    QRegExp re("\\d*");
    if (re.exactMatch(rain_percent) && re.exactMatch(wind_percent) && re.exactMatch(duration_time)){
        QPushButton *button6 = new QPushButton(high_difficulty);
        connect(button6, SIGNAL(clicked()), this, SLOT(dynamic_buttons_clicked()));
        test->addWidget(button6);
    } else{
        send_message("You have not entered all numbers. Please make sure you enter only numbers.");
    }
}

void MATBWindow::on_insert_tlx_clicked(){
    QPushButton *button6 = new QPushButton(insert_tlx);
    connect(button6, SIGNAL(clicked()), this, SLOT(dynamic_buttons_clicked()));
    test->addWidget(button6);
}

void MATBWindow::on_del_condition_clicked(){
    if (last_clicked_button){
        last_clicked_button->deleteLater();
        last_clicked_button = nullptr;
    }

}

void MATBWindow::on_Gen_script_clicked(){
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save MATB Conf File"), "",
            tr("Conf file (*.conf);;All Files (*)"));
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    out << difficulty_string_generator("ld", ui->rain_percent_ld->toPlainText(), ui->wind_percent_ld->toPlainText(), ui->duration_time_ld->toPlainText(), ui->day_or_night_ld->currentText());
    out << "\n";
    out << difficulty_string_generator("md", ui->rain_percent_md->toPlainText(), ui->wind_percent_md->toPlainText(), ui->duration_time_md->toPlainText(), ui->day_or_night_md->currentText());
    out << "\n";
    out << difficulty_string_generator("hd", ui->rain_percent_hd->toPlainText(), ui->wind_percent_hd->toPlainText(), ui->duration_time_hd->toPlainText(), ui->day_or_night_hd->currentText());
    out << "\n";
    out << "Rest duration is: " << ui->duration_time_rest->toPlainText();
    out << "\n";
    QList<QPushButton *> butts = my_button_window->findChildren<QPushButton *>();
    for (const auto *but: butts) {
        out << but->text();
        out << "\n";
    }
}

void MATBWindow::add_buttons_to_list(QString button_text){
    QPushButton *button6 = new QPushButton(button_text);
    test->addWidget(button6);
}


void MATBWindow::load_file_clicked(QAction* test){
    QString fileName = QFileDialog::getOpenFileName(this, ("Open File"),
                                                      "/home",
                                                      ("Conf Files (*.conf)"));
    std::cout << fileName.toUtf8().constData() << std::endl;
    std::regex rgx("((l|m|h)d) Rain % is (\\d*) Wind % is (\\d*) for (\\d*) seconds Time of the day is (Day|Night)");
    std::smatch matches;
    std::string str;
    std::ifstream input_file(fileName.toUtf8().constData());
    if (!input_file.good()){
        std::cout << "Unable to read file" << std::endl;
    }
    int counter {1};
    bool okay {true};
    while (std::getline(input_file, str)) {
        if (counter < 4){
            if(std::regex_search(str, matches, rgx)){
                if (matches[1] == "ld"){
                    if(ui->rain_percent_ld->toPlainText().toUtf8().constData() == matches[3]
                            && ui->wind_percent_ld->toPlainText().toUtf8().constData() == matches[4]
                            && ui->duration_time_ld->toPlainText().toUtf8().constData() == matches[5]
                            && ui->day_or_night_ld->currentText().toUtf8().constData() == matches[6] ){

                    }else {
                        QString custom_msg = QString::fromStdString("This line does not match original requirements at the time of saving file "+str);
                        send_message(custom_msg);
                        okay = false;
                        break;
                            }
                } else if(matches[1] == "md"){
                    if(ui->rain_percent_md->toPlainText().toUtf8().constData() == matches[3]
                            && ui->wind_percent_md->toPlainText().toUtf8().constData() == matches[4]
                            && ui->duration_time_md->toPlainText().toUtf8().constData() == matches[5]
                            && ui->day_or_night_md->currentText().toUtf8().constData() == matches[6] ){

                    }else {
                                QString custom_msg = QString::fromStdString("This line does not match original requirements at the time of saving file "+str);
                                send_message(custom_msg);
                                okay = false;
                                break;
                            }
                } else if(matches[1] == "hd"){
                    if(ui->rain_percent_hd->toPlainText().toUtf8().constData() == matches[3]
                            && ui->wind_percent_hd->toPlainText().toUtf8().constData() == matches[4]
                            && ui->duration_time_hd->toPlainText().toUtf8().constData() == matches[5]
                            && ui->day_or_night_hd->currentText().toUtf8().constData() == matches[6] ){

                    }else {
                        QString custom_msg = QString::fromStdString("This line does not match original requirements at the time of saving file "+str);
                        send_message(custom_msg);
                        okay = false;
                        break;
                            }
                }
            } else{
                QString custom_msg = QString::fromStdString("This line aint recognized by regex: "+str);
                send_message(custom_msg);
                break;
            }
            ++counter;
        } else {
            if (str == low_difficulty.toStdString() || str == moderate_difficulty.toStdString() || str == high_difficulty.toStdString() || str == insert_tlx.toStdString()){
                add_buttons_to_list(QString::fromStdString(str));
            }else {
                QString custom_msg = QString::fromStdString("This line is not recognized "+str);
                send_message(custom_msg);
                okay = false;
                break;

                    }
        }
    }
    if(okay){
        QString custom_msg = QString::fromStdString("Conf File loaded successfully");
        send_message(custom_msg);} else{
        QString custom_msg = QString::fromStdString("Errors Occured. Couldn't load file. Check if all default arguments are same");
        send_message(custom_msg);
    }
}
