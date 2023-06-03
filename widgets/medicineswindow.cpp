#include "medicineswindow.h"
#include "ui_medicineswindow.h"
#include "inputfields.h"
#include <QDebug>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>

MedicinesWindow::MedicinesWindow(QWidget *parent, MedsEntity* MEntity) :
    aChildWin(parent),
    ui(new Ui::MedicinesWindow)
{
    qDebug() << "CRT medics";
    prnt = parent;
    InFAbs = nullptr;
    entity = MEntity;
    ui->setupUi(this);
    this->setWindowTitle("Склад лекарств и препаратов");
    this->setGeometry(prnt->geometry());
    setAttribute(Qt::WA_DeleteOnClose);
    ui->verticalLayout->setSpacing(1);
    init_menubar();

    QVector<QString> temp = entity->get_fnames();
    // загрузка модели в виджет QTableView
    init_table();
    // Формирование списка ключей и соответствия
    for (int i = 0; i < entity->get_model()->record().count(); i++) {
        keys.append(entity->get_model()->record().fieldName(i));
        fields.insert(keys[i], temp[i]);
    }
    temp.clear();
    ui->table->setWordWrap(true);
}

MedicinesWindow::~MedicinesWindow() {
    qDebug() << "DEL medics {";
    impl.reset();
    delete ui;
    qDebug() << "}";
}

// Реагирование на изменение размеров окна
// задание нужной ширины каждого столбца таблицы
void MedicinesWindow::resizeEvent(QResizeEvent *event) {
    ui->table->setColumnWidth(0, 20);
    ui->table->setColumnWidth(1, (ui->table->width() - 520)/2);
    ui->table->setColumnWidth(2, (ui->table->width() - 520)/2);
    ui->table->setColumnWidth(3, 110);
    ui->table->setColumnWidth(4, 110);
    ui->table->setColumnWidth(5, 90);
    ui->table->setColumnWidth(6, 110);
    ui->table->resizeRowsToContents();
    event->accept();
}

void MedicinesWindow::init_table() {
    if (entity == nullptr) return;
    entity->select();
    ui->table->setModel(entity->get_model());
    ui->table->setColumnWidth(0, 20);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QHeaderView* header = ui->table->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Interactive);
    header->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    //header->setDefaultSectionSize(25); // 20 px
    //header->close();
    header = nullptr;
    ui->table->setColumnWidth(0, 20);
}

void MedicinesWindow::init_menubar() {
    menubar = new QMenuBar(this);
    QFont font = menubar->font();
    font.setPointSize(11);
    menubar->setFont(font);
    backact = new QAction(this->style()->standardIcon(QStyle::SP_ArrowLeft),
                                "",
                                menubar);
    backact->setShortcut(Qt::CTRL + Qt::Key_Left);
    connect(backact, &QAction::triggered, this, &MedicinesWindow::goback);
    menubar->addAction(backact);
    progmenu = new QMenu("Программа", menubar);
    progmenu->addAction("Настройки");
    //progmenu->addAction(this->style()->standardIcon(QStyle::SP_DialogCloseButton),
    //                    "Выход",
    //                    this,
    //                    &MedicinesWindow::closem,
    //                    Qt::CTRL + Qt::Key_Q);
    menubar->addMenu(progmenu);
    editing = new QMenu("Правка", menubar);
    editing->addAction("Добавить",
                       this,
                       &MedicinesWindow::clicked_on_add);
    delselact = new QAction("Удалить", editing);
    delselact->setShortcut(Qt::Key_Delete);
    connect(ui->table, &QTableView::pressed, this, &MedicinesWindow::enable_deleting);
    connect(delselact, &QAction::triggered, this, &MedicinesWindow::clicked_on_delete_selected);
    delselact->setEnabled(false);
    editing->addAction(delselact);
    delfoundact = new QAction("Удалить найденные", editing);
    connect(delfoundact, &QAction::triggered, this, &MedicinesWindow::clicked_on_delete_found);
    delfoundact->setEnabled(false);
    editing->addAction(delfoundact);
    menubar->addMenu(editing);

    searchmenu = new QMenu("Поиск", menubar);
    searchmenu->addAction("Поиск",
                          this,
                          &MedicinesWindow::clicked_on_find,
                          Qt::CTRL + Qt::Key_F);
    resetsrchact = new QAction("Сброс поиска", editing);
    connect(resetsrchact, &QAction::triggered, this, &MedicinesWindow::clicked_on_reset);
    resetsrchact->setEnabled(false);
    searchmenu->addAction(resetsrchact);
    menubar->addMenu(searchmenu);

    toolbar = new QToolBar(this);
    toolbar->setStyleSheet("QToolBar { padding: 0; spacing: 5px; }");
    saveact = new QAction(this->style()->standardIcon(QStyle::SP_DialogSaveButton),
                                "Сохранить", toolbar);
    saveact->setShortcut(Qt::CTRL + Qt::Key_S);
    saveact->setEnabled(false);
    connect(saveact, &QAction::triggered, this, &MedicinesWindow::clicked_on_submit);
    toolbar->addAction(saveact);
    revertact = new QAction(this->style()->standardIcon(QStyle::SP_DialogCancelButton),
                                "Отменить", toolbar);
    revertact->setShortcut(Qt::CTRL + Qt::Key_R);
    revertact->setEnabled(false);
    connect(revertact, &QAction::triggered, this, &MedicinesWindow::clicked_on_revert);
    toolbar->addAction(revertact);
    ui->verticalLayout->insertWidget(0, menubar);
    ui->verticalLayout->insertWidget(1, toolbar);
}

//void MedicinesWindow::closem() {
//
//}

// нажание на кнопку возврата назад
void MedicinesWindow::goback() {
    if (entity->isDirty()) {
        QMessageBox* msgBox = new QMessageBox("Возврат назад",
                                              "В базу данных не внемены изменения."
                                              "Отменить их и вернуться назад?\n",
                                              QMessageBox::Question,
                                              QMessageBox::Button::Yes,
                                              QMessageBox::Button::No,
                                              QMessageBox::Button::Cancel,
                                              this);
        int resBtn = msgBox->exec();
        if (resBtn == QMessageBox::Yes) {
            entity->revertAll();
            emit goback_signal(this);
        }
    }
    else {
        emit goback_signal(this);
    }
}

void MedicinesWindow::closeEvent(QCloseEvent* event) {
    qDebug() << "close MedicinesWindow";
    event->accept();
}

// переход к форме добавления записи
void MedicinesWindow::clicked_on_add() {
    qDebug() << fields;
    impl = QSharedPointer<Implement>(new AddMedsImplement());
    QMap<QString, QString> titles = {{"title", "Добавить запись."}, {"exec", "Добавить"}};
    InFAbs = new MedsAbstr(titles, fields, keys, impl, this);
    titles.clear();
    connect(impl.get(), QOverload<QSqlRecord*>::of(&Implement::exec_clicked_signal), this, &MedicinesWindow::add_record_db);
    impl.reset();
    emit summoned_child(InFAbs);
}

// добавление записи
void MedicinesWindow::add_record_db(QSqlRecord* record) {
    if (entity->addRecord(record)) {
        QMessageBox::information(this, "Успех!",
                             "Введённая запись добавлена в временное представление таблицы базы данных. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
    }
    else {
        QMessageBox::warning(this, "Ошибка!",
                             "Ошибка добвления записи в таблицу!");
    }
    //fields_names.clear();
    saveact->setEnabled(true);
    revertact->setEnabled(true);

}

// удаление выбранных строк
void MedicinesWindow::clicked_on_delete_selected() {
    QItemSelectionModel* selection = ui->table->selectionModel();
    QModelIndexList selection_list(selection->selectedIndexes());
    if (selection_list.isEmpty()) return;

    QMessageBox* msgBox = new QMessageBox("Удаление",
                                          "",
                                          QMessageBox::Question,
                                          QMessageBox::Button::Yes,
                                          QMessageBox::Button::No,
                                          QMessageBox::Button::Cancel,
                                          this);
    int resBtn = 0;

    // Быстрая проверка на то, что размер массива равен 1
    if (selection_list.cbegin() == --selection_list.cend()) {
        msgBox->setText("Удалить запись №" +
                        QString::number(
                            entity->get_record(selection_list.at(0).row()).field(0).value().toLongLong())
                        + "?\n");
    }
    else {
        msgBox->setText("Удалить выделенные записи?\n");
    }
    resBtn = msgBox->exec();
    if (resBtn != QMessageBox::Yes) {
        return;
    }
    else {
        for (QModelIndex& index : selection_list) {
            entity->removeRecord(index.row());
        }
        saveact->setEnabled(true);
        revertact->setEnabled(true);
    }
}

// подтверждение изменений в таблице
void MedicinesWindow::clicked_on_submit() {
    if (!entity->submitAll()) {
        qDebug() << entity->lastError();
    }
    else {
        saveact->setEnabled(false);
        revertact->setEnabled(false);
    }
}

// отмена изменений в таблице
void MedicinesWindow::clicked_on_revert() {
    saveact->setEnabled(false);
    revertact->setEnabled(false);
    entity->revertAll();
}

// переход к форме поиска записи
void MedicinesWindow::clicked_on_find() {
    impl = QSharedPointer<Implement>(new FindMedsImplement());
    QMap<QString, QString> titles = {{"title", "Найти запись."}, {"exec", "Найти"}};
    InFAbs = new MedsAbstr(titles, fields, keys, impl, this);
    connect(impl.get(), QOverload<QString&>::of(&Implement::exec_clicked_signal), this, &MedicinesWindow::find_record_db);
    impl.reset();
    emit summoned_child(InFAbs);
}

// установка фильтра по введённым данным в поля
void MedicinesWindow::find_record_db(QString& where) {
    qDebug() << where;
    entity->setFilter(where);
    resetsrchact->setEnabled(true);
    if (entity->rowCount() == 0) {
        QMessageBox::warning(this, "Ошибка!",
                             "По заданному запросу ничего не найдено!");
    }
    else {
        InFAbs->goback();
        delfoundact->setEnabled(true);
    }
}

// отмена изменений в таблице
void MedicinesWindow::clicked_on_reset(){
    entity->setFilter("");
    if (entity->rowCount() == 0) {
        entity->select();
    }
    resetsrchact->setEnabled(false);
    delfoundact->setEnabled(false);
}

// удаление всех найденных записей
void MedicinesWindow::clicked_on_delete_found() {
    if (entity->filter() == "") return;
    QMessageBox* msgBox = new QMessageBox("Удаление найденных",
                                          "Удалить все найденные записи?\n",
                                          QMessageBox::Question,
                                          QMessageBox::Button::Yes,
                                          QMessageBox::Button::No,
                                          QMessageBox::Button::Cancel,
                                          this);
    int resBtn = msgBox->exec();
    qDebug() << entity->rowCount();
    if (resBtn == QMessageBox::Yes) {
        entity->removeRecords(0, entity->rowCount());
        QMessageBox::information(this, "Успех!",
                             "Все найденные записи помечены на удаление. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
        saveact->setEnabled(true);
        revertact->setEnabled(true);
    }
}

void MedicinesWindow::enable_deleting() {
    delselact->setEnabled(true);
}

