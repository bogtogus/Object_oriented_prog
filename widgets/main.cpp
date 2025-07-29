#include "mainwindow.h"
#include <QApplication>

/*!
 * \file Главный запускаемый файл.
 * \brief В файле создаётся и выводится на экран объект главного окна.
*/

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Фиалка-Системс");
    w.show();
    return a.exec();
}
