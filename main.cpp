#include "matbwindow.h"
#include "rest_dialog.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MATBWindow w;
    w.show();

    rest_dialog r(nullptr, 20);
    r.show();

    return a.exec();
}
