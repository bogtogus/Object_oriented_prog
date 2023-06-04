#ifndef BONUSPROGWINDOW_H
#define BONUSPROGWINDOW_H

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
    void clicked_on_add();
    void clicked_on_delete_selected();
    void clicked_on_edit();
    void clicked_on_submit();
    void clicked_on_revert();
    void clicked_on_find();
    void clicked_on_reset();
    void clicked_on_delete_found();
    void enable_deleting();

private:
    Ui::BonusProgWindow *ui;
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
    void init_menubar();
    void add_record_db(const QSqlRecord*);
    void edit_record_db(const QSqlRecord&, const int);
    void find_record_db(const QString&);
    void enable_rows_operations();
};

#endif // BONUSPROGWINDOW_H
