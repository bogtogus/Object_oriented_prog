#ifndef SELLHISTWINDOW_H
#define SELLHISTWINDOW_H

#include "mainwindow.h"
#include "achildwin.h"
#include "inputfields.h"
#include <entities.h>
#include <QSqlTableModel>
#include <QCloseEvent>
#include <QAbstractItemView>

namespace Ui {
class SellHistWindow;
}

class SellHistWindow : public aChildWin
{
    Q_OBJECT

public:
    explicit SellHistWindow(QWidget *parent,
                             MedsEntity*,
                             BonusEntity*,
                             SellEntity*);
    ~SellHistWindow();
    void goback() override;
private:
    Ui::SellHistWindow *ui;
    // объект, работающий с таблицей лекарств лекарств в БД
    MedsEntity* MEntity;
    // объект, работающий с таблицей карт лекарств в БД
    BonusEntity* BEntity;
    // объект, работающий с таблицей истории лекарств в БД
    SellEntity* SEntity;
    // Соответствие "Имя столбца в БД"-"Наименование столбца"
    QMap<QString, QString> fields;
    // список неотсортированных(т.е. в том порядке, в каком они в таблице)
    // имён столбцов в БД
    QVector<QString> keys;
    // Окно ввода данных
    inputFields* InFAbs;
    // Обработчик данных в окне ввода данных
    QSharedPointer<Implement> impl;
    // первый добавленный индекс в таблице
    int first_add;
    // временное хранилище новых значений количества
    // лекарств на складе (id:new_amount)
    // которое потом будет вноситься в реальную таблицу
    QMap<int, int> temp_sales;

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

#endif // SELLHISTWINDOW_H
