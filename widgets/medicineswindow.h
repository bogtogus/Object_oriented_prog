#ifndef MEDICINESWINDOW_H
#define MEDICINESWINDOW_H

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
    QMenuBar* menubar;
    QToolBar* toolbar;
    QMenu* progmenu;
    QMenu* editing;
    QMenu* searchmenu;
    QAction* backact;
    QAction* saveact;
    QAction* revertact;
    QAction* delselact;
    QAction* delfoundact;
    QAction* editact;
    QAction* resetsrchact;

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

    void add_record_db(const QSqlRecord*);
    void find_record_db(const QString&);
    void edit_record_db(const QSqlRecord&, const int);
    void init_table();
    void init_menubar();
    //void closem();
private slots:
    void closeEvent(QCloseEvent*) override;
    void resizeEvent(QResizeEvent *event) override;
    void clicked_on_add();
    void clicked_on_delete_selected();
    void clicked_on_edit();
    void clicked_on_submit();
    void clicked_on_revert();
    void clicked_on_find();
    void clicked_on_reset();
    void clicked_on_delete_found();
    void enable_rows_operations();
};

#endif // MEDICINESWINDOW_H
