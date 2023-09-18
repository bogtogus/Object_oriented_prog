#include "bonusprogwindow.h"
#include "ui_bonusprogwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>

BonusProgWindow::BonusProgWindow(QWidget *parent,
                                  BonusEntity* BEntity) :
    DBWindow(parent),
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
    ui->verticalLayout->insertWidget(0, menubar);
    ui->verticalLayout->insertWidget(1, toolbar);
    for (int i = 0; i < entity->get_model()->record().count(); i++) {
        keys.append(entity->get_model()->record().fieldName(i));
        fields.insert(keys[i], temp[i]);
    }
    temp.clear();
}

BonusProgWindow::~BonusProgWindow() {
    qDebug() << "DEL bonus {";
    delete ui;
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

void BonusProgWindow::connect_menu() {
    connect(backact, &QAction::triggered, this, &BonusProgWindow::goback);
    connect(addact, &QAction::triggered, this, &BonusProgWindow::clicked_on_add);
    connect(ui->table, &QTableView::pressed, this, &BonusProgWindow::enable_rows_operations);
    connect(delselact, &QAction::triggered, this, &BonusProgWindow::clicked_on_delete_selected);
    connect(editact, &QAction::triggered, this, &BonusProgWindow::clicked_on_edit);
    connect(delfoundact, &QAction::triggered, this, &BonusProgWindow::clicked_on_delete_found);
    connect(searchact, &QAction::triggered, this, &BonusProgWindow::clicked_on_find);
    connect(resetsrchact, &QAction::triggered, this, &BonusProgWindow::clicked_on_reset);
    connect(saveact, &QAction::triggered, this, &BonusProgWindow::clicked_on_submit);
    connect(revertact, &QAction::triggered, this, &BonusProgWindow::clicked_on_revert);
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

