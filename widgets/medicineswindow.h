#ifndef MEDICINESWINDOW_H
#define MEDICINESWINDOW_H

#include "dbwindow.h"
#include <entities.h>
#include <QSqlTableModel>
#include <QCloseEvent>
#include <QAbstractItemView>

namespace Ui {
class MedicinesWindow;
}

/*!
 * \brief Окно работы с таблицей медикаментов на складе.
 */
class MedicinesWindow : public DBWindow
{
    Q_OBJECT

public:
    MedicinesWindow(QWidget *parent, MedsEntity*);
    virtual ~MedicinesWindow();
    void goback() override;
private:
    Ui::MedicinesWindow *ui;
    // объект, взаимодействующий с таблицей лекарств в БД
    MedsEntity* entity;

    void init_table();
    void connect_menu() override;
    void add_record_db(const QSqlRecord*);
    void find_record_db(const QString&);
    void edit_record_db(const QSqlRecord&, const int);
    //void closem();
private slots:
    void closeEvent(QCloseEvent*) override;
    void resizeEvent(QResizeEvent *event) override;
    void clicked_on_add() override;
    void clicked_on_delete_selected() override;
    void clicked_on_edit() override;
    void clicked_on_submit() override;
    void clicked_on_revert() override;
    void clicked_on_find() override;
    void clicked_on_reset() override;
    void clicked_on_delete_found() override;
    void contextMenuRequested(const QPoint& mousepos) override;
};

#endif // MEDICINESWINDOW_H
