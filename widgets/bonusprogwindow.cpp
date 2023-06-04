#include "bonusprogwindow.h"
#include "ui_bonusprogwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>

BonusProgWindow::BonusProgWindow(QWidget *parent,
                                  BonusEntity* BEntity) :
    aChildWin(parent),
    ui(new Ui::BonusProgWindow)
{
    qDebug() << "CRT bonus";
    entity = BEntity;
    InFAbs = nullptr;
    ui->setupUi(this);
    this->setWindowTitle("База бонусных карт");
    this->setGeometry(prnt->geometry());
    setAttribute(Qt::WA_DeleteOnClose);
    ui->verticalLayout->setSpacing(1);
    init_menubar();

    this->setWindowTitle("Склад лекарств и препаратов");
    this->setGeometry(prnt->geometry());
    setAttribute(Qt::WA_DeleteOnClose);
    revertact->setEnabled(false);
    saveact->setEnabled(false);

    QVector<QString> temp = entity->get_fnames();
    init_table();
    for (int i = 0; i < entity->get_model()->record().count(); i++) {
        keys.append(entity->get_model()->record().fieldName(i));
        fields.insert(keys[i], temp[i]);
    }
    temp.clear();
}

BonusProgWindow::~BonusProgWindow() {
    qDebug() << "DEL bonus {";
    impl.reset();
    delete ui;
    qDebug() << "}";
}

// Реагирование на изменение размеров окна
// задание нужной ширины каждого столбца таблицы
void BonusProgWindow::resizeEvent(QResizeEvent *event) {
    ui->table->setColumnWidth(0, 100);
    ui->table->setColumnWidth(1, std::max((ui->table->width() - 300), 100));
    ui->table->setColumnWidth(2, 150);
    event->accept();
}

// нажание на кнопку возврата назад
void BonusProgWindow::goback() {
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

void BonusProgWindow::closeEvent(QCloseEvent *event) {
    qDebug() << "close BonusProgWindow";
    event->accept();
}

void BonusProgWindow::init_table() {
    ui->table->setModel(entity->get_model());
    QHeaderView* header = ui->table->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Interactive);
    header->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    //header->setDefaultSectionSize(25); // 20 px
    //header->close();
    header = nullptr;
    ui->table->setColumnWidth(0, 20);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void BonusProgWindow::init_menubar() {
    menubar = new QMenuBar(this);
    QFont font = menubar->font();
    font.setPointSize(11);
    menubar->setFont(font);
    backact = new QAction(this->style()->standardIcon(QStyle::SP_ArrowLeft),
                                "",
                                menubar);
    backact->setShortcut(Qt::CTRL + Qt::Key_Left);
    connect(backact, &QAction::triggered, this, &BonusProgWindow::goback);
    menubar->addAction(backact);
    progmenu = new QMenu("Программа", menubar);
    progmenu->addAction("Настройки");
    //progmenu->addAction(this->style()->standardIcon(QStyle::SP_DialogCloseButton),
    //                    "Выход",
    //                    this,
    //                    &BonusProgWindow::closem,
    //                    Qt::CTRL + Qt::Key_Q);
    menubar->addMenu(progmenu);
    editing = new QMenu("Правка", menubar);
    editing->addAction("Добавить",
                       this,
                       &BonusProgWindow::clicked_on_add);
    delselact = new QAction("Удалить", editing);
    delselact->setShortcut(Qt::Key_Delete);
    connect(ui->table, &QTableView::pressed, this, &BonusProgWindow::enable_deleting);
    connect(delselact, &QAction::triggered, this, &BonusProgWindow::clicked_on_delete_selected);
    delselact->setEnabled(false);
    editing->addAction(delselact);
    editact = new QAction("Изменить", editing);
    //editact->setShortcut(Qt::Key_Delete);
    connect(ui->table, &QTableView::pressed, this, &BonusProgWindow::enable_rows_operations);
    connect(editact, &QAction::triggered, this, &BonusProgWindow::clicked_on_edit);
    editact->setEnabled(false);
    editing->addAction(editact);
    delfoundact = new QAction("Удалить найденные", editing);
    connect(delfoundact, &QAction::triggered, this, &BonusProgWindow::clicked_on_delete_found);
    delfoundact->setEnabled(false);
    editing->addAction(delfoundact);
    menubar->addMenu(editing);

    searchmenu = new QMenu("Поиск", menubar);
    searchmenu->addAction("Поиск",
                          this,
                          &BonusProgWindow::clicked_on_find,
                          Qt::CTRL + Qt::Key_F);
    resetsrchact = new QAction("Сброс поиска", editing);
    connect(resetsrchact, &QAction::triggered, this, &BonusProgWindow::clicked_on_reset);
    resetsrchact->setEnabled(false);
    searchmenu->addAction(resetsrchact);
    menubar->addMenu(searchmenu);

    toolbar = new QToolBar(this);
    toolbar->setStyleSheet("QToolBar { padding: 0; spacing: 5px; }");
    saveact = new QAction(this->style()->standardIcon(QStyle::SP_DialogSaveButton),
                                "Сохранить", toolbar);
    saveact->setShortcut(Qt::CTRL + Qt::Key_S);
    saveact->setEnabled(false);
    connect(saveact, &QAction::triggered, this, &BonusProgWindow::clicked_on_submit);
    toolbar->addAction(saveact);
    revertact = new QAction(this->style()->standardIcon(QStyle::SP_DialogCancelButton),
                                "Отменить", toolbar);
    revertact->setShortcut(Qt::CTRL + Qt::Key_R);
    revertact->setEnabled(false);
    connect(revertact, &QAction::triggered, this, &BonusProgWindow::clicked_on_revert);
    toolbar->addAction(revertact);
    ui->verticalLayout->insertWidget(0, menubar);
    ui->verticalLayout->insertWidget(1, toolbar);
}

// переход к форме добавления записи
void BonusProgWindow::add_record_db(const QSqlRecord* record) {
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

void BonusProgWindow::edit_record_db(const QSqlRecord & record, const int row) {
    if (entity->setRecord(row, record)) {
        QMessageBox::information(this, "Успех!",
                             "Введённая запись отредактирована. "
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

// добавление записи
void BonusProgWindow::clicked_on_add() {
    impl = QSharedPointer<Implement>(new AddBonusImplement());
    QMap<QString, QString> titles = {{"title", "Добавить бонусную карту."}, {"exec", "Добавить"}};
    InFAbs = new BonusAbstr(titles, fields, keys, impl, this);
    titles.clear();
    connect(impl.get(),
            QOverload<const QSqlRecord*>::of(&Implement::exec_clicked_signal),
            this,
            &BonusProgWindow::add_record_db);
    impl.reset();
    emit summoned_child(InFAbs);
}

void BonusProgWindow::clicked_on_edit() {
    impl = QSharedPointer<Implement>(new EditBonusImplement());
    QItemSelectionModel* selection = ui->table->selectionModel();
    int row = selection->selectedIndexes().at(0).row();
    QSqlRecord record = entity->get_record(row);
    QMap<QString, QString> titles = {{"title", "Редактировать запись."}, {"exec", "Применить"}};
    InFAbs = new BonusAbstr(titles, fields, keys, impl, this);
    InFAbs->fill_fields(record, row);
    titles.clear();
    connect(impl.get(),
            QOverload<const QSqlRecord&, const int>::of(&Implement::exec_clicked_signal),
            this,
            &BonusProgWindow::edit_record_db);
    impl.reset();
    emit summoned_child(InFAbs);
}

// переход к форме поиска записи
void BonusProgWindow::clicked_on_find() {
    impl = QSharedPointer<Implement>(new FindBonusImplement());
    QMap<QString, QString> titles = {{"title", "Найти бонусную карту."}, {"exec", "Найти"}};
    InFAbs = new BonusAbstr(titles, fields, keys, impl, this);
    connect(impl.get(),
            QOverload<const QString&>::of(&Implement::exec_clicked_signal),
            this,
            &BonusProgWindow::find_record_db);
    impl.reset();
    emit summoned_child(InFAbs);
}

// установка фильтра по введённым данным в поля
void BonusProgWindow::find_record_db(const QString& where) {
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

void BonusProgWindow::enable_rows_operations() {
    delselact->setEnabled(true);
    editact->setEnabled(true);
}

// удаление выбранной строки
void BonusProgWindow::clicked_on_delete_selected() {
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

// отмена изменений в таблице
void BonusProgWindow::clicked_on_reset() {
    entity->setFilter("");
    if (entity->rowCount() == 0) {
        entity->select();
    }
    resetsrchact->setEnabled(false);
    delfoundact->setEnabled(false);
}

// удаление всех найденных записей
void BonusProgWindow::clicked_on_delete_found() {
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
    //model->
    if (resBtn == QMessageBox::Yes) {
        entity->removeRecords(0, entity->rowCount());
        QMessageBox::information(this, "Успех!",
                             "Все найденные бонусные карты помечены на удаление. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
        saveact->setEnabled(true);
        revertact->setEnabled(true);
    }
}

// подтверждение изменений в таблице
void BonusProgWindow::clicked_on_submit() {
    if (!entity->submitAll()) {
        qDebug() << entity->lastError();
    }
    else {
        revertact->setEnabled(false);
        saveact->setEnabled(false);
    }
}

// отмена изменений в таблице
void BonusProgWindow::clicked_on_revert() {
    revertact->setEnabled(false);
    saveact->setEnabled(false);
    entity->revertAll();
}

void BonusProgWindow::enable_deleting() {
    delselact->setEnabled(true);
}

