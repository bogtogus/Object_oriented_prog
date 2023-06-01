#ifndef BONUSPROGWINDOW_H
#define BONUSPROGWINDOW_H

#include "mainwindow.h"
#include "achildwin.h"
#include "inputfields.h"
#include <entities.h>
#include <QSqlTableModel>
#include <QCloseEvent>
#include <QAbstractItemView>

namespace Ui {
class BonusProgWindow;
}

class BonusProgWindow : public aChildWin
{
    Q_OBJECT

public:
    explicit BonusProgWindow(QWidget *parent,
                              BonusEntity*);
    ~BonusProgWindow();
    void goback() override;
private slots:
    void closeEvent(QCloseEvent*) override;
    void resizeEvent(QResizeEvent *event) override;
    void on_add_clicked();
    void on_delete_selected_clicked();
    void on_find_clicked();
    void on_reset_clicked();
    void on_delete_found_clicked();
    void on_submit_clicked();
    void on_revert_clicked();

private:
    Ui::BonusProgWindow *ui;
    // объект, взаимодействующий с таблицей карт в БД
    BonusEntity* entity;
    // Соответствие "Имя столбца в БД"-"Наименование столбца"
    QMap<QString, QString> fields;
    // список неотсортированных(т.е. в том порядке, в каком они в таблице)
    // имён столбцов в БД
    QVector<QString> keys;
    // Окно ввода данных
    inputFields* InFAbs;
    // Обработчик данных в окне ввода данных
    QSharedPointer<Implement> impl;

    void init_table();
    void add_record_db(QSqlRecord*);
    void find_record_db(QString&);
};

#endif // BONUSPROGWINDOW_H
