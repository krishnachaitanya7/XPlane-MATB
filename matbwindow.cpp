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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


// Namespaces
namespace pt = boost::property_tree;
// Start Global Static Variables
static QVBoxLayout *test;
static QWidget *my_button_window;
static QPushButton *last_clicked_button {nullptr};
static std::map<std::string, std::string> airports_codes_map;
static std::map<std::string, int> airports_altitude_map;
static std::string arrival_port_default {"Select Arrival Airport"};
static std::string departure_port_default {"Select Departure Airport"};
static std::string wind_percent_string {"Wind_Percent"};
static std::string rain_percent_string {"Rain_Percent"};
static std::string duration_string {"Duration"};
static std::string day_or_night_string {"Day_or_Night"};
static std::string external_manipulation_string {"External_Manipulation"};
static std::string departure_airport_string {"Departure_Airport"};
static std::string arrival_airport_string {"Arrival_Airport"};
static std::string airport_code_string {"Airport_Code"};
static std::string airport_height_string {"Airport_Base_Height"};
static std::string action_list_string {"Actions_List"};
static std::string rest_string {"Rest"};
// End Global Static Variables
// Start Constant Declaration. Only edit here
static int duration_ld {20};
static int rain_ld {0};
static int wind_ld {0};
static std::string day_night_ld = "Day";
static int duration_md {20};
static int rain_md {50};
static int wind_md {50};
static std::string day_night_md = "Day";
static int duration_hd {20};
static int rain_hd {99};
static int wind_hd {99};
static std::string day_night_hd = "Day";
static int rest_seconds {15};
static std::string low_difficulty {"Low Difficulty"};
static std::string moderate_difficulty {"Moderate Difficulty"};
static std::string high_difficulty {"High Difficulty"};
static std::string insert_tlx {"Fill Out Survey"};
static std::string homedir;
static std::string airport_file;
// End constanats declaration
void send_message(QString send_msg);

std::string MATBWindow::get_full_airport_name(std::string airport_code){
    for(auto const &x: airports_codes_map){
        if(x.second == airport_code){
            return x.first;
        }
    }
    send_message(QString::fromStdString("Airport name not found for this code. Checks airports.json and add this code: "+airport_code+" Exiting now"));
    this->close();
}

std::string MATBWindow::get_airport_code(std::string airport_name){
    // Regex Link
    // https://regex101.com/r/pYWRLT/1
    std::map<std::string, std::string>::iterator airport_iterator;
    airport_iterator = airports_codes_map.find(airport_name);
    if(airport_iterator == airports_codes_map.end()){
        send_message(QString::fromStdString("Airport code not found. Checks airports.json for this airport name: "+airport_name+" Exiting now"));
        this->close();
      }
    return airport_iterator->second;
}

int MATBWindow::get_airport_height(std::string airport_name){
    // Regex Link
    // https://regex101.com/r/pYWRLT/1
    std::map<std::string, int>::iterator airport_iterator;
    airport_iterator = airports_altitude_map.find(airport_name);
    if(airport_iterator == airports_altitude_map.end()){
        send_message(QString::fromStdString("Airport Height not found. Checks airports.json for this airport name: "+airport_name+" Exiting now"));
        this->close();
      }
    return airport_iterator->second;
}

void MATBWindow::add_airports(){
    pt::ptree root;
    pt::read_json(airport_file, root);
    ui->departure_airport->addItem(QString::fromStdString(departure_port_default));
    ui->arrival_airport->addItem(QString::fromStdString(arrival_port_default));
    for (pt::ptree::value_type &airport : root){
        std::string airport_name = airport.first;
        ui->departure_airport->addItem(QString::fromStdString(airport_name));
        ui->arrival_airport->addItem(QString::fromStdString(airport_name));
        for(pt::ptree::value_type &airport_contents : root.get_child(airport_name)){
            if(airport_contents.first == airport_code_string){
                std::string airport_code = airport_contents.second.data();
                airports_codes_map.emplace(airport_name, airport_code);
            } else if(airport_contents.first == airport_height_string){
                int airport_height = std::stoi(airport_contents.second.data());
                airports_altitude_map.emplace(airport_name, airport_height);

            }
        }
    }

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
    // Get environment Variables
    homedir = getenv("HOME");
    airport_file = homedir + "/xplane_plugin_files/airports.json";
    // End
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
    add_airports();
    // Menu Actions
    connect(ui->menuXPlane_MATB, SIGNAL(triggered(QAction*)), this, SLOT(load_file_clicked(QAction*)));



}

MATBWindow::~MATBWindow(){
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
        QPushButton *button6 = new QPushButton(QString::fromStdString(low_difficulty));
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
        QPushButton *button6 = new QPushButton(QString::fromStdString(moderate_difficulty));
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
        QPushButton *button6 = new QPushButton(QString::fromStdString(high_difficulty));
        connect(button6, SIGNAL(clicked()), this, SLOT(dynamic_buttons_clicked()));
        test->addWidget(button6);
    } else{
        send_message("You have not entered all numbers. Please make sure you enter only numbers.");
    }
}

void MATBWindow::on_insert_tlx_clicked(){
    QPushButton *button6 = new QPushButton(QString::fromStdString(insert_tlx));
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
            tr("Conf file in JSON (*.json);;All Files (*)"));
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    QTextStream out(&file);
    pt::ptree write_file_root;
    // Write Rest String
    write_file_root.put(rest_string, ui->duration_time_rest->toPlainText().toUtf8().constData());
    // Write Low difficulty JSON
    write_file_root.put(low_difficulty+"."+wind_percent_string, std::stoi(ui->wind_percent_ld->toPlainText().toUtf8().constData()));
    write_file_root.put(low_difficulty+"."+rain_percent_string, std::stoi(ui->rain_percent_ld->toPlainText().toUtf8().constData()));
    write_file_root.put(low_difficulty+"."+duration_string, std::stoi(ui->duration_time_ld->toPlainText().toUtf8().constData()));
    write_file_root.put(low_difficulty+"."+day_or_night_string, ui->day_or_night_ld->currentText().toUtf8().constData());
    write_file_root.put(low_difficulty+"."+external_manipulation_string, ui->external_mani_ld->toPlainText().toUtf8().constData());
    // Write Moderate Difficulty JSON
    write_file_root.put(moderate_difficulty+"."+wind_percent_string, std::stoi(ui->wind_percent_md->toPlainText().toUtf8().constData()));
    write_file_root.put(moderate_difficulty+"."+rain_percent_string, std::stoi(ui->rain_percent_md->toPlainText().toUtf8().constData()));
    write_file_root.put(moderate_difficulty+"."+duration_string, std::stoi(ui->duration_time_md->toPlainText().toUtf8().constData()));
    write_file_root.put(moderate_difficulty+"."+day_or_night_string, ui->day_or_night_md->currentText().toUtf8().constData());
    write_file_root.put(moderate_difficulty+"."+external_manipulation_string, ui->external_mani_md->toPlainText().toUtf8().constData());
    // Write High Difficulty JSON
    write_file_root.put(high_difficulty+"."+wind_percent_string, std::stoi(ui->wind_percent_hd->toPlainText().toUtf8().constData()));
    write_file_root.put(high_difficulty+"."+rain_percent_string, std::stoi(ui->rain_percent_hd->toPlainText().toUtf8().constData()));
    write_file_root.put(high_difficulty+"."+duration_string, std::stoi(ui->duration_time_hd->toPlainText().toUtf8().constData()));
    write_file_root.put(high_difficulty+"."+day_or_night_string, ui->day_or_night_hd->currentText().toUtf8().constData());
    write_file_root.put(high_difficulty+"."+external_manipulation_string, ui->external_mani_hd->toPlainText().toUtf8().constData());
    // Write Airports JSON
    write_file_root.put(departure_airport_string+"."+airport_code_string, get_airport_code(ui->departure_airport->currentText().toUtf8().constData()));
    write_file_root.put(departure_airport_string+"."+airport_height_string, get_airport_height(ui->departure_airport->currentText().toUtf8().constData()));
    write_file_root.put(arrival_airport_string+"."+airport_code_string, get_airport_code(ui->arrival_airport->currentText().toUtf8().constData()));
    write_file_root.put(arrival_airport_string+"."+airport_height_string, get_airport_height(ui->arrival_airport->currentText().toUtf8().constData()));
    // Write Action Strings
    pt::ptree actions_list;
    QList<QPushButton *> butts = my_button_window->findChildren<QPushButton *>();
    for (const auto *but: butts){
        pt::ptree each_action;
        each_action.put("", but->text().toUtf8().constData());
        actions_list.push_back(std::make_pair("", each_action));
    }
    write_file_root.add_child(action_list_string, actions_list);
    // Write JSON to file
    static std::fstream final_config_file;
    final_config_file.open(fileName.toUtf8().constData(), std::fstream::in | std::fstream::out | std::fstream::app);
    if (!final_config_file )
        {
            std::cout << "Cannot open file, file does not exist. Creating new file..";
            final_config_file.open(fileName.toUtf8().constData(),  std::fstream::in | std::fstream::out | std::fstream::trunc);
            final_config_file.close();

        }
    pt::write_json(final_config_file, write_file_root);

}

void MATBWindow::add_buttons_to_list(QString button_text){
    QPushButton *button6 = new QPushButton(button_text);
    test->addWidget(button6);
}


void MATBWindow::load_file_clicked(QAction* test){
    QString fileName = QFileDialog::getOpenFileName(this, ("Open File"),
                                                      "/home",
                                                      ("Conf Files (*.json)"));
    std::cout << fileName.toUtf8().constData() << std::endl;
    std::string filename_in_stdstring {fileName.toUtf8().constData()};
    pt::ptree root;
    pt::read_json(filename_in_stdstring, root);
    for (pt::ptree::value_type &each_element : root) {
        std::string element_name = each_element.first;
        if(element_name == low_difficulty){
            for(pt::ptree::value_type &second_layer_elements : root.get_child(element_name)){
                if(second_layer_elements.first == wind_percent_string){
                    int wind_percent {std::stoi(second_layer_elements.second.data())};
                    ui->wind_percent_ld->setText(QString::number(wind_percent));
                } else if(second_layer_elements.first == rain_percent_string){
                    int rain_percent {std::stoi(second_layer_elements.second.data())};
                    ui->rain_percent_ld->setText(QString::number(rain_percent));
                } else if(second_layer_elements.first == duration_string){
                    int duration {std::stoi(second_layer_elements.second.data())};
                    ui->duration_time_ld->setText(QString::number(duration));
                } else if(second_layer_elements.first == day_or_night_string){
                    int index {ui->day_or_night_ld->findText(QString::fromStdString(second_layer_elements.second.data()))};
                    ui->day_or_night_ld->setCurrentIndex(index);
                } else if(second_layer_elements.first == external_manipulation_string){
                    ui->external_mani_ld->setText(QString::fromStdString(second_layer_elements.second.data()));
                }
            }
        } else if(element_name == moderate_difficulty){
            for(pt::ptree::value_type &second_layer_elements : root.get_child(element_name)){
                if(second_layer_elements.first == wind_percent_string){
                    int wind_percent {std::stoi(second_layer_elements.second.data())};
                    ui->wind_percent_md->setText(QString::number(wind_percent));
                } else if(second_layer_elements.first == rain_percent_string){
                    int rain_percent {std::stoi(second_layer_elements.second.data())};
                    ui->rain_percent_md->setText(QString::number(rain_percent));
                } else if(second_layer_elements.first == duration_string){
                    int duration {std::stoi(second_layer_elements.second.data())};
                    ui->duration_time_md->setText(QString::number(duration));
                } else if(second_layer_elements.first == day_or_night_string){
                    int index {ui->day_or_night_ld->findText(QString::fromStdString(second_layer_elements.second.data()))};
                    ui->day_or_night_md->setCurrentIndex(index);
                } else if(second_layer_elements.first == external_manipulation_string){
                    ui->external_mani_md->setText(QString::fromStdString(second_layer_elements.second.data()));
                }
            }
        } else if(element_name == high_difficulty){
            for(pt::ptree::value_type &second_layer_elements : root.get_child(element_name)){
                if(second_layer_elements.first == wind_percent_string){
                    int wind_percent {std::stoi(second_layer_elements.second.data())};
                    ui->wind_percent_hd->setText(QString::number(wind_percent));
                } else if(second_layer_elements.first == rain_percent_string){
                    int rain_percent {std::stoi(second_layer_elements.second.data())};
                    ui->rain_percent_hd->setText(QString::number(rain_percent));
                } else if(second_layer_elements.first == duration_string){
                    int duration {std::stoi(second_layer_elements.second.data())};
                    ui->duration_time_hd->setText(QString::number(duration));
                } else if(second_layer_elements.first == day_or_night_string){
                    int index {ui->day_or_night_ld->findText(QString::fromStdString(second_layer_elements.second.data()))};
                    ui->day_or_night_hd->setCurrentIndex(index);
                } else if(second_layer_elements.first == external_manipulation_string){
                    ui->external_mani_hd->setText(QString::fromStdString(second_layer_elements.second.data()));
                }
            }
        } else if(element_name == departure_airport_string){
            for(pt::ptree::value_type &second_layer_elements : root.get_child(element_name)){
                if(second_layer_elements.first == airport_code_string){
                    std::string departure_airport {get_full_airport_name(second_layer_elements.second.data())};
                    if(!departure_airport.empty()){
                        int depart_index {ui->departure_airport->findText(QString::fromStdString(departure_airport))};
                        ui->departure_airport->setCurrentIndex(depart_index);
                    } else {
                                std::cout << "Airport Code not found: " << second_layer_elements.second.data() << std::endl;
                            }

                } /*else if(second_layer_elements.first == airport_height_string){
                    If you wanna do anything with height in the future

                }*/
            }
        } else if(element_name == arrival_airport_string){
            for(pt::ptree::value_type &second_layer_elements : root.get_child(element_name)){
                if(second_layer_elements.first == airport_code_string){
                    std::string arrival_airport {get_full_airport_name(second_layer_elements.second.data())};
                    if(!arrival_airport.empty()){
                        int arrival_index {ui->departure_airport->findText(QString::fromStdString(arrival_airport))};
                        ui->arrival_airport->setCurrentIndex(arrival_index);
                    } else {
                                std::cout << "Airport Code not found: " << second_layer_elements.second.data() << std::endl;
                            }

                } /*else if(second_layer_elements.first == airport_height_string){
                    If you wanna do anything with height in the future

                }*/
            }
        }  else if(element_name == action_list_string){
            for(pt::ptree::value_type &second_layer_elements : root.get_child(element_name)){
                add_buttons_to_list(QString::fromStdString(second_layer_elements.second.data()));
            }

        }
    }
    // Set Rest
    ui->duration_time_rest->setText(QString::number(root.get<int>(rest_string, -1)));


}
