#include "medicineswindow.h"
#include "ui_medicineswindow.h"
#include "inputfields.h"
#include <QDebug>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>

/*!
 * \brief Конструктор окна.
 * \param parent - родительский объект (обычно главное окно).
 * \param MEntity - объект сущности лекарств.
 */
MedicinesWindow::MedicinesWindow(QWidget *parent, MedsEntity* MEntity) :
    DBWindow(parent),
    ui(new Ui::MedicinesWindow)
{
    qDebug() << "CRT medics";
    prnt = parent;
    InpFieldsAbstr = nullptr;
    entity = MEntity;
    ui->setupUi(this);
    this->setWindowTitle("Склад лекарств и препаратов");
    this->setGeometry(prnt->geometry());
    setAttribute(Qt::WA_DeleteOnClose);
    ui->verticalLayout->setSpacing(1);
    init_menubar();
    ui->verticalLayout->insertWidget(0, menubar);
    ui->verticalLayout->insertWidget(1, toolbar);

    QVector<QString> temp = entity->get_readable_names();
    keys = entity->get_column_names();
    // загрузка модели в виджет QTableView
    init_table();
    // Формирование списка ключей и соответствия
    for (int i = 0; i < entity->columnCount(); i++) {
        fields.insert(keys[i], temp[i]);
    }
    temp.clear();
    ui->table->setWordWrap(true);
}

MedicinesWindow::~MedicinesWindow() {
    qDebug() << "DEL medics {";
    delete ui;
}

/*!
 * \brief Реагирование на изменение размеров окна.
 * Производится задание нужной ширины каждого столбца таблицы.
 * Метод вызывается автоматически по сигналу.
 * \param event - событие изменения размеров окна.
 */
void MedicinesWindow::resizeEvent(QResizeEvent *event) {
    ui->table->setColumnWidth(0, 20);
    ui->table->setColumnWidth(1, (ui->table->width() - 570)/2);
    ui->table->setColumnWidth(2, (ui->table->width() - 570)/2);
    ui->table->setColumnWidth(3, 110);
    ui->table->setColumnWidth(4, 110);
    ui->table->setColumnWidth(5, 70);
    ui->table->setColumnWidth(6, 110);
    ui->table->setColumnWidth(7, 60);
    ui->table->resizeRowsToContents();
    event->accept();
}

/*!
 * \brief Инициализация табличного представления, а именно
 * передача ему модели таблицы, а также настройка нескольких параметров представления.
 */
void MedicinesWindow::init_table() {
    if (entity == nullptr) return;
    entity->select();
    ui->table->setModel(entity->get_model());
    ui->table->setColumnWidth(0, 20);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    QHeaderView* header = ui->table->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Interactive);
    header->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    //header->setDefaultSectionSize(25); // 20 px
    //header->close();
    header = nullptr;
    ui->table->setColumnWidth(0, 20);
}

/*!
 * \brief Метод привязки всех методов меню к соответствующим методам класса.
 */
void MedicinesWindow::connect_menu() {
    connect(backact, &QAction::triggered, this, &MedicinesWindow::goback);
    connect(addact, &QAction::triggered, this, &MedicinesWindow::clicked_on_add);
    connect(ui->table, &QTableView::pressed, this, &MedicinesWindow::enable_rows_operations);
    connect(delselact, &QAction::triggered, this, &MedicinesWindow::clicked_on_delete_selected);
    connect(editact, &QAction::triggered, this, &MedicinesWindow::clicked_on_edit);
    connect(delfoundact, &QAction::triggered, this, &MedicinesWindow::clicked_on_delete_found);
    connect(searchact, &QAction::triggered, this, &MedicinesWindow::clicked_on_find);
    connect(resetsrchact, &QAction::triggered, this, &MedicinesWindow::clicked_on_reset);
    connect(saveact, &QAction::triggered, this, &MedicinesWindow::clicked_on_submit);
    connect(revertact, &QAction::triggered, this, &MedicinesWindow::clicked_on_revert);
}

/*!
 * \brief Метод возврата назад.
 * При несохранённых изменениях просит подтверждения действия.
 * Переход назад осуществляется излучением сигнала goback_signal(...) для главного окна.
 */
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

/*!
 * \brief Метод, вызываемый при закрытии основного окна.
 * \param event - событие закрытия окна.
 */
void MedicinesWindow::closeEvent(QCloseEvent* event) {
    qDebug() << "close MedicinesWindow";
    event->accept();
}

/*!
 * \brief Переход к форме добавления записи в таблицу.
 * \details Инициализируется объект, проверяющий корректность ввода данных в форму.
 * Затем создаётся объект, генерирующий форму согласно полям таблицы.
 * Связывается сигнал отправки данных из формы с методом добавления записи в таблицу.
 */
void MedicinesWindow::clicked_on_add() {
    qDebug() << fields;
    impl = QSharedPointer<Implement>(new AddMedsImplement());
    QMap<QString, QString> titles = {{"title", "Добавить запись."}, {"exec", "Добавить"}};
    InpFieldsAbstr = new MedsAbstr(titles, fields, keys, impl, this);
    titles.clear();
    connect(impl.get(),
            QOverload<const QSqlRecord*>::of(&Implement::exec_clicked_signal),
            this,
            &MedicinesWindow::add_record_db);
    impl.reset();
    emit summoned_child(InpFieldsAbstr);
}

/*!
 * \brief Добавление записи по данным, полученным из окна заполнения данных.
 * \param record - сформированная запись.
 */
void MedicinesWindow::add_record_db(const QSqlRecord* record) {
    if (entity->addRecord(record)) {
        QMessageBox::information(this, "Успех!",
                             "Введённая запись добавлена в временное представление таблицы базы данных. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
    }
    else {
        QMessageBox::warning(this, "Ошибка!",
                             "Ошибка добвления записи в таблицу!");
        qDebug() << entity->lastError();
        qDebug() << *record;
    }
    //fields_names.clear();
    saveact->setEnabled(true);
    revertact->setEnabled(true);
}

/*!
 * \brief Переход к форме редактирования записи в таблице.
 * \details Инициализируется объект, проверяющий корректность ввода данных в форму.
 * Затем создаётся объект, генерирующий форму согласно полям таблицы.
 * Связывается сигнал отправки данных из формы с методом изменения записи в таблице.
 */
void MedicinesWindow::clicked_on_edit() {
    impl = QSharedPointer<Implement>(new EditMedsImplement());
    QItemSelectionModel* selection = ui->table->selectionModel();
    int row = selection->selectedIndexes().at(0).row();
    QSqlRecord record = entity->get_record(row);
    QMap<QString, QString> titles = {{"title", "Редактировать запись."}, {"exec", "Применить"}};
    InpFieldsAbstr = new MedsAbstr(titles, fields, keys, impl, this);
    InpFieldsAbstr->fill_fields(record, row);
    titles.clear();
    connect(impl.get(),
            QOverload<const QSqlRecord&, const int>::of(&Implement::exec_clicked_signal),
            this,
            &MedicinesWindow::edit_record_db);
    impl.reset();
    emit summoned_child(InpFieldsAbstr);
}

/*!
 * \brief Изменение записи согласно данным, полученным из окна заполнения данных.
 * \param record - сформированная новая запись.
 * \param row - строка в модели таблицы, где находится старая запись.
 */
void MedicinesWindow::edit_record_db(const QSqlRecord& record, const int row) {
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

/*!
 * \brief Переход к форме поиска записи в таблице.
 * Поиск осуществляется по любому полю.
 * \details Инициализируется объект, проверяющий корректность ввода данных в форму.
 * Затем создаётся объект, генерирующий форму согласно полям таблицы.
 * Связывается сигнал отправки данных из формы с методом добавления записи в таблицу.
 */
void MedicinesWindow::clicked_on_find() {
    impl = QSharedPointer<Implement>(new FindMedsImplement());
    QMap<QString, QString> titles = {{"title", "Найти запись."}, {"exec", "Найти"}};
    InpFieldsAbstr = new MedsAbstr(titles, fields, keys, impl, this, true);
    connect(impl.get(),
            QOverload<const QString&>::of(&Implement::exec_clicked_signal),
            this,
            &MedicinesWindow::find_record_db);
    impl.reset();
    emit summoned_child(InpFieldsAbstr);
}

/*!
 * \brief Установка фильтра для модели согласно запросу по полям записи.
 * \param where - SQL-запрос типа "WHERE ..."
 */
void MedicinesWindow::find_record_db(const QString& where) {
    qDebug() << where;
    entity->setFilter(where);
    if (entity->rowCount() == 0) {
        QMessageBox::warning(this, "Ошибка!",
                             "По заданному запросу ничего не найдено!");
        entity->setFilter("");
        if (entity->rowCount() == 0) {
            entity->select();
        }
    }
    else {
        InpFieldsAbstr->goback();
        resetsrchact->setEnabled(true);
        delfoundact->setEnabled(true);
    }
}

/*!
 * \brief Подтверждение изменений в таблице.
 */
void MedicinesWindow::clicked_on_submit() {
    if (!entity->submitAll()) {
        qDebug() << entity->lastError();
    }
    else {
        saveact->setEnabled(false);
        revertact->setEnabled(false);
    }
}

/*!
 * \brief Отмена изменений в таблице.
 */
void MedicinesWindow::clicked_on_revert() {
    saveact->setEnabled(false);
    revertact->setEnabled(false);
    entity->revertAll();
}


/*!
 * \brief Сброс поиска по таблице.
 */
void MedicinesWindow::clicked_on_reset(){
    entity->setFilter("");
    if (entity->rowCount() == 0) {
        entity->select();
    }
    resetsrchact->setEnabled(false);
    delfoundact->setEnabled(false);
}

/*!
 * \brief Удаление всех найденных записей.
 */
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

/*!
 * \brief Удаление записи или нескольих записей, выделенных в таблице.
 * \brief Согласно размеру списка выделенных записей запрашивается подтверждение на удаление,
 * а затем и удаление выделенных записей.
 */
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
                        entity->get_record(selection_list.at(0).row()).field(0).value().toString()
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


