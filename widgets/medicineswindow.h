#ifndef MEDICINESWINDOW_H
#define MEDICINESWINDOW_H

#include "mainwindow.h"
#include "achildwin.h"
#include "inputfields.h"
#include <entities.h>
#include <QSqlTableModel>
#include <QCloseEvent>
#include <QAbstractItemView>

namespace Ui {
class MedicinesWindow;
}

class MedicinesWindow : public aChildWin
{
    Q_OBJECT

public:
    MedicinesWindow(QWidget *parent, MedsEntity*);
    ~MedicinesWindow();
    void goback() override;
private:
    Ui::MedicinesWindow *ui;
    // объект, взаимодействующий с таблицей лекарств в БД
    MedsEntity* entity;
    // Соответствие "Имя столбца в БД"-"Наименование столбца"
    QMap<QString, QString> fields;
    // список неотсортированных(т.е. в том порядке, в каком они в таблице)
    // имён столбцов в БД
    QVector<QString> keys;
    // Окно ввода данных
    inputFields* InFAbs;
    // Обработчик данных в окне ввода данных
    QSharedPointer<Implement> impl;

    void add_record_db(QSqlRecord*);
    void find_record_db(QString&);
    void init_table();
private slots:
    void closeEvent(QCloseEvent*) override;
    void resizeEvent(QResizeEvent *event) override;
    void on_add_clicked();
    void on_delete_selected_clicked();
    void on_submit_clicked();
    void on_revert_clicked();
    void on_find_clicked();
    void on_reset_clicked();
    void on_delete_found_clicked();
};

#endif // MEDICINESWINDOW_H
