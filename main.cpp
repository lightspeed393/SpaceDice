#include "mainwindow.h"
#include <QApplication>
#include "stuff.h"
#include "functions.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    gcurl_init();
    init_config();
    w.show();

    return a.exec();
}
