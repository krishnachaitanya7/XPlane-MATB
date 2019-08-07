#ifndef MATBWINDOW_H
#define MATBWINDOW_H

#include <QMainWindow>

namespace Ui {
class MATBWindow;
}

class MATBWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MATBWindow(QWidget *parent = nullptr);
    ~MATBWindow();

private slots:
    void on_close_button_clicked();

    void on_Add_to_list_clicked();

    void on_Gen_script_clicked();

    void dynamic_buttons_clicked();

    void load_file_clicked(QAction* test);

    void add_buttons_to_list(QString rain_percent, QString wind_percent, QString duration_time, QString time_of_day);
private:
    Ui::MATBWindow *ui;


};

#endif // MATBWINDOW_H
