#ifndef REST_DIALOG_H
#define REST_DIALOG_H

#include <QDialog>

namespace Ui {
class rest_dialog;
}

class rest_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit rest_dialog(QWidget *parent = nullptr, int elapsed_time = 15);
    ~rest_dialog();
public slots:
    void MyTimerSlot();

private:
    Ui::rest_dialog *ui;
};

#endif // REST_DIALOG_H
