#ifndef SELLHISTWINDOW_H
#define SELLHISTWINDOW_H

#include "dbwindow.h"
#include <entities.h>
#include <QSqlTableModel>
#include <QCloseEvent>
#include <QAbstractItemView>

namespace Ui {
class SellHistWindow;
}

/*!
 * \brief Окно работы с таблицей проданных лекарств.
 */
class SellHistWindow : public DBWindow
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
    // айди лекарства, которое было подвергнуто изменению
    QSqlRecord* record_being_edited;

    void add_record_db(const QSqlRecord*);
    void find_record_db(const QString&);
    void edit_record_db(const QSqlRecord&, const int);
    void init_table();
    void connect_menu() override;
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

#endif // SELLHISTWINDOW_H
