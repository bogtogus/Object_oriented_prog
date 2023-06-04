#ifndef DBWINDOW_H
#define DBWINDOW_H
#include "achildwin.h"
#include "inputfields.h"

class DBWindow : public aChildWin {
    Q_OBJECT
protected:
    QMenuBar* menubar;
    QToolBar* toolbar;
    QMenu* progmenu;
    QMenu* editing;
    QMenu* searchmenu;
    QAction* backact;
    QAction* saveact;
    QAction* revertact;
    QAction* searchact;
    QAction* addact;
    QAction* delselact;
    QAction* delfoundact;
    QAction* editact;
    QAction* resetsrchact;

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
    void init_menubar();
    virtual void connect_menu() = 0;
    //void closem();
protected slots:
    virtual void clicked_on_add() = 0;
    virtual void clicked_on_delete_selected() = 0;
    virtual void clicked_on_edit() = 0;
    virtual void clicked_on_submit() = 0;
    virtual void clicked_on_revert() = 0;
    virtual void clicked_on_find() = 0;
    virtual void clicked_on_reset() = 0;
    virtual void clicked_on_delete_found() = 0;
    void enable_rows_operations();
public:
    DBWindow(QWidget *parent);
    virtual ~DBWindow();
};

#endif // DBWINDOW_H
