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

    void set_all_defaults();

    void on_set_all_default_clicked();

    void on_close_button_clicked();

    void on_sel_ld_clicked();

    void add_airports();

    void on_sel_md_clicked();

    void on_sel_hd_clicked();

    void on_insert_tlx_clicked();

    void on_del_condition_clicked();

    void dynamic_buttons_clicked();

    void on_Gen_script_clicked();

    void load_file_clicked(QAction* test);

    void add_buttons_to_list(QString button_text);

private:
    Ui::MATBWindow *ui;


};

#endif // MATBWINDOW_H
