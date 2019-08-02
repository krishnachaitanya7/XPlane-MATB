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

private:
    Ui::MATBWindow *ui;
};

#endif // MATBWINDOW_H
