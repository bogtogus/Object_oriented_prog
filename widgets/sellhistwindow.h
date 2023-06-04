#ifndef SELLHISTWINDOW_H
#define SELLHISTWINDOW_H

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

    void add_record_db(const QSqlRecord*);
    void find_record_db(const QString&);
    void edit_record_db(const QSqlRecord&, const int);
    void init_table();
    void init_menubar();
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

#endif // SELLHISTWINDOW_H
