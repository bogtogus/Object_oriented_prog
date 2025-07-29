#include "dbwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>

DBWindow::DBWindow(QWidget *parent) : aChildWin(parent) {}

DBWindow::~DBWindow() {
    fields.clear();
    keys.clear();
    impl.reset();
    qDebug() << "}";
}

/*!
 * \brief Инициализация меню.
 * \details Создаются подменю и соответсвующие им действия.
 */
void DBWindow::init_menubar() {
    menubar = new QMenuBar(this);
    QFont font = menubar->font();
    font.setPointSize(11);
    menubar->setFont(font);
    backact = new QAction(this->style()->standardIcon(QStyle::SP_ArrowLeft),
                          "",
                          menubar);
    backact->setShortcut(Qt::CTRL | Qt::Key_Left);
    menubar->addAction(backact);

    editing = new QMenu("Правка", menubar);
    addact = new QAction("Добавить", editing);
    editing->addAction(addact);
    delselact = new QAction("Удалить", editing);
    delselact->setShortcut(Qt::Key_Delete);
    delselact->setEnabled(false);
    editing->addAction(delselact);
    editact = new QAction("Изменить", editing);
    //editact->setShortcut(Qt::Key_Delete);
    editact->setEnabled(false);
    editing->addAction(editact);
    delfoundact = new QAction("Удалить найденные", editing);
    delfoundact->setEnabled(false);
    editing->addAction(delfoundact);
    menubar->addMenu(editing);

    searchmenu = new QMenu("Поиск", menubar);
    searchact = new QAction("Поиск", searchmenu);
    searchact->setShortcut(Qt::CTRL | Qt::Key_F);
    searchmenu->addAction(searchact);
    resetsrchact = new QAction("Сброс поиска", editing);
    resetsrchact->setEnabled(false);
    searchmenu->addAction(resetsrchact);
    menubar->addMenu(searchmenu);

    toolbar = new QToolBar(this);
    toolbar->setStyleSheet("QToolBar { padding: 0; spacing: 5px; }");
    saveact = new QAction(this->style()->standardIcon(QStyle::SP_DialogSaveButton),
                                "Сохранить", toolbar);
    saveact->setShortcut(Qt::CTRL | Qt::Key_S);
    saveact->setEnabled(false);

    toolbar->addAction(saveact);
    revertact = new QAction(this->style()->standardIcon(QStyle::SP_DialogCancelButton),
                                "Отменить", toolbar);
    revertact->setShortcut(Qt::CTRL | Qt::Key_R);
    revertact->setEnabled(false);
    toolbar->addAction(revertact);
    connect_menu();
}

/*!
 * \brief Включение функций работы со строками.
 * \details Метод выполняется при нажатии на таблицу.
 */
void DBWindow::enable_rows_operations() {
    delselact->setEnabled(true);
    editact->setEnabled(true);
}
