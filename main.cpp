#include "matbwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MATBWindow w;
    w.show();

    return a.exec();
}
