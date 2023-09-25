#include "sellhistwindow.h"
#include "ui_sellhistwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>


/*!
 * \brief Конструктор окна.
 * \param parent - родительский объект (обычно главное окно).
 * \param me - объект сущности лекарств.
 * \param be - объект сущности бонусных карт.
 * \param se - объект сущности истории продаж.
 */
SellHistWindow::SellHistWindow(QWidget *parent,
                               MedsEntity* me,
                               BonusEntity* be,
                               SellEntity* se) :
    DBWindow(parent),
    ui(new Ui::SellHistWindow)
{
    qDebug() << "CRT hist";
    prnt = parent;
    MEntity = me;
    BEntity = be;
    SEntity = se;
    InpFieldsAbstr = nullptr;
    record_being_edited = nullptr;
    ui->setupUi(this);
    ui->verticalLayout->setSpacing(1);
    init_menubar();
    ui->verticalLayout->insertWidget(0, menubar);
    ui->verticalLayout->insertWidget(1, toolbar);

    this->setWindowTitle("История продаж");
    this->setGeometry(prnt->geometry());
    setAttribute(Qt::WA_DeleteOnClose);
    QVector<QString> temp = SEntity->get_readable_names();
    keys = SEntity->get_column_names();
    init_table();
    for (int i = 0; i < SEntity->columnCount(); i++) {
        fields.insert(keys[i], temp[i]);
    }
    temp.clear();
    ui->table->resizeRowsToContents();
}

SellHistWindow::~SellHistWindow() {
    qDebug() << "DEL hist {";
    MEntity->clear_cache();
    delete ui;
}

/*!
 * \brief Реагирование на изменение размеров окна.
 * Производится задание нужной ширины каждого столбца таблицы.
 * Метод вызывается автоматически по сигналу.
 * \param event - событие изменения размеров окна.
 */
void SellHistWindow::resizeEvent(QResizeEvent *event) {
    ui->table->setColumnWidth(0, 20);
    if (SEntity->columnCount() == 1) return;
    int wi = (ui->table->width() - 65) / (SEntity->columnCount() - 1);
    for (int i = 1; i < SEntity->columnCount(); i++) {
        ui->table->setColumnWidth(i, wi);
    }
    event->accept();
    //ui->table->resizeRowsToContents();
}

/*!
 * \brief Инициализация табличного представления, а именно
 * передача ему модели таблицы, а также настройка нескольких параметров представления.
 */
void SellHistWindow::init_table() {
    SEntity->select();
    QHeaderView* header = ui->table->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Interactive);
    header->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
    //header->setDefaultSectionSize(25); // 20 px
    //header->close();
    header = nullptr;
    ui->table->setColumnWidth(0, 20);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table->setModel(SEntity->get_model());
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table->setWordWrap(true);
    ui->table->setContextMenuPolicy(Qt::CustomContextMenu);
}

/*!
 * \brief Метод привязки всех методов меню к соответствующим методам класса.
 */
void SellHistWindow::connect_menu() {
    connect(backact, &QAction::triggered, this, &SellHistWindow::goback);
    connect(addact, &QAction::triggered, this, &SellHistWindow::clicked_on_add);
    connect(ui->table, &QTableView::pressed, this, &SellHistWindow::enable_rows_operations);
    connect(delselact, &QAction::triggered, this, &SellHistWindow::clicked_on_delete_selected);
    connect(editact, &QAction::triggered, this, &SellHistWindow::clicked_on_edit);
    connect(delfoundact, &QAction::triggered, this, &SellHistWindow::clicked_on_delete_found);
    connect(searchact, &QAction::triggered, this, &SellHistWindow::clicked_on_find);
    connect(resetsrchact, &QAction::triggered, this, &SellHistWindow::clicked_on_reset);
    connect(saveact, &QAction::triggered, this, &SellHistWindow::clicked_on_submit);
    connect(revertact, &QAction::triggered, this, &SellHistWindow::clicked_on_revert);

    connect(ui->table, &QWidget::customContextMenuRequested, this, &SellHistWindow::contextMenuRequested);
}

void SellHistWindow::form_exit() {
    if (record_being_edited) {
        delete record_being_edited;
        record_being_edited = nullptr;
    }
}

/*!
 * \brief Метод возврата назад.
 * При несохранённых изменениях просит подтверждения действия.
 * Переход назад осуществляется излучением сигнала goback_signal(...) для главного окна.
 */
void SellHistWindow::goback() {
    if (SEntity->isDirty()) {
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
            SEntity->revertAll();
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
void SellHistWindow::closeEvent(QCloseEvent *event) {
    qDebug() << "close SellHistWindow";
    event->accept();
}

/*!
 * \brief Переход к форме добавления записи в таблицу.
 * \details Инициализируется объект, проверяющий корректность ввода данных в форму.
 * Затем создаётся объект, генерирующий форму согласно полям таблицы.
 * Связывается сигнал отправки данных из формы с методом добавления записи в таблицу.
 */
void SellHistWindow::clicked_on_add() {
    qDebug() << fields;
    impl = QSharedPointer<Implement>(new AddSellImplement());
    QMap<QString, QString> titles = {{"title", "Добавить запись."}, {"exec", "Добавить"}};
    InpFieldsAbstr = new SellAbstr(titles, fields, keys, SEntity, impl, this);
    titles.clear();
    connect(impl.get(),
            QOverload<const QSqlRecord*>::of(&Implement::exec_clicked_signal),
            this,
            &SellHistWindow::add_record_db);
    connect(InpFieldsAbstr, &SellAbstr::goback_signal, this, &SellHistWindow::form_exit);
    impl.reset();
    emit summoned_child(InpFieldsAbstr);
}

/*!
 * \brief Добавление записи по данным, полученным из окна заполнения данных.
 * \param record - сформированная запись.
 */
void SellHistWindow::add_record_db(const QSqlRecord* record) {
    int med_id = record->value(keys[3]).toInt();
    int pieces_sold = record->value(keys[4]).toInt();
    int card_num = record->value(keys[2]).toInt();
    int withdrown = record->value(keys[6]).toInt();
    int customer_spent = record->value(keys[5]).toInt();

    if (MEntity->cache_contains(med_id) && !MEntity->is_enough_meds(med_id, pieces_sold)) {
        QMessageBox::warning(this, "Ошибка!",
                             "Ошибка добавления записи в таблицу! "
                             "На складе нет достаточного количества лекарства (id " +
                             QString::number(med_id) + ")! Запрошено " +
                             QString::number(pieces_sold) + "шт. , в наличии " +
                             QString::number(MEntity->get_med_amount(med_id)) + "шт.");
        return;
    }
    if (BEntity->cache_contains(card_num) && !BEntity->is_enough_bonuses(card_num, withdrown)) {
        QMessageBox::warning(this, "Ошибка!",
                             "Ошибка добавления записи в таблицу! "
                             "На счету бонусной карты нет достаточного количества лекарства (id " +
                             QString::number(card_num) + ")! Попытка снять " +
                             QString::number(withdrown) + "баллов , на счёте " +
                             QString::number(BEntity->get_card_balance(card_num)) + "баллов.");
        return;
    }
    if (SEntity->addRecord(record)) {
        QMessageBox::information(this, "Успех!",
                             "Введённая запись добавлена в временное представление таблицы базы данных. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
        // Если в временном хранилище транзакций ещё не было операций
        // с данным id лекартсва, то она сначала добавляется в это хранилище
        MEntity->add_temp_sale(med_id, pieces_sold);
        BEntity->add_temp_withdraw(card_num, withdrown, customer_spent);
    }
    else {
        QMessageBox::warning(this, "Ошибка!",
                             "Ошибка добвления записи в таблицу!");
        qDebug() << SEntity->lastError();
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
void SellHistWindow::clicked_on_edit() {
    impl = QSharedPointer<Implement>(new EditSellImplement());
    QItemSelectionModel* selection = ui->table->selectionModel();
    int row = selection->selectedIndexes().at(0).row();
    QSqlRecord record = SEntity->get_record(row);
    QMap<QString, QString> titles = {{"title", "Редактировать запись."}, {"exec", "Применить"}};
    InpFieldsAbstr = new SellAbstr(titles, fields, keys, SEntity, impl, this);
    InpFieldsAbstr->fill_fields(record, row);
    titles.clear();
    connect(impl.get(),
            QOverload<const QSqlRecord&, const int>::of(&Implement::exec_clicked_signal),
            this,
            &SellHistWindow::edit_record_db);
    impl.reset();
    record_being_edited = new QSqlRecord(record);
    emit summoned_child(InpFieldsAbstr);
}

/*!
 * \brief Изменение записи согласно данным, полученным из окна заполнения данных.
 * \param record - сформированная новая запись.
 * \param row - строка в модели таблицы, где находится старая запись.
 */
void SellHistWindow::edit_record_db(const QSqlRecord & record, const int row) {
    if (SEntity->setRecord(row, record)) {
        QMessageBox::information(this, "Успех!",
                             "Введённая запись отредактирована. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
    }
    else {
        QMessageBox::warning(this, "Ошибка!",
                             "Ошибка добвления записи в таблицу!");
        qDebug() << SEntity->lastError();
    }
    //int old_med_id = record_being_edited->value(keys[3]).toInt();
    //int old_sold = record_being_edited->value(keys[4]).toInt();
    //int old_card_number = record_being_edited->value(keys[2]).toInt();
    //int old_withdrown = record_being_edited->value(keys[6]).toInt();
    //int old_customer_spent = record_being_edited->value(keys[5]).toInt();

    //int med_id = record.value(keys[3]).toInt();
    //int sold = record.value(keys[4]).toInt();
    //int card_num = record.value(keys[2]).toInt();
    //int withdrown = record.value(keys[6]).toInt();
    //int customer_spent = record.value(keys[5]).toInt();

    //if (MEntity->cache_contains(med_id) && !MEntity->is_enough_meds(med_id, sold)) {
    //    QMessageBox::warning(this, "Ошибка!",
    //                         "Ошибка добавления записи в таблицу! "
    //                         "На складе нет достаточного количества лекарства (id " +
    //                         QString::number(med_id) + ")! Запрошено " +
    //                         QString::number(sold) + "шт. , в наличии " +
    //                         QString::number(MEntity->get_med_amount(med_id)) + "шт.");
    //    return;
    //}
    //if (!BEntity->is_enough_bonuses(card_num, withdrown)) {
    //    QMessageBox::warning(this, "Ошибка!",
    //                         "Ошибка добавления записи в таблицу! "
    //                         "На счету бонусной карты нет достаточного количества лекарства (id " +
    //                         QString::number(card_num) + ")! Попытка снять " +
    //                         QString::number(withdrown) + " баллов , на счёте " +
    //                         QString::number(BEntity->get_card_balance(card_num)) + " баллов.");
    //    return;
    //}
    //if (SEntity->setRecord(row, record)) {
    //    QMessageBox::information(this, "Успех!",
    //                         "Введённая запись отредактирована. "
    //                         "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
    //
    //    // Если изменился айди проданного лекарства, причём оно было в кэше, то из кэша значение удаляется
    //    if (old_med_id != 0 && old_med_id != med_id && MEntity->cache_contains(old_med_id)) {
    //        MEntity->remove_temp_sale(old_med_id);
    //        BEntity->remove_temp_withdraw(old_card_number);
    //    }
    //    if (old_card_number != 0 && old_card_number != card_num && MEntity->cache_contains(old_card_number)) {
    //        BEntity->remove_temp_withdraw(old_card_number);
    //    }
    //    MEntity->add_temp_sale(med_id, sold - old_sold);
    //    BEntity->add_temp_withdraw(card_num, withdrown - old_withdrown, customer_spent - old_customer_spent);
    //}
    //else {
    //    QMessageBox::warning(this, "Ошибка!",
    //                         "Ошибка добвления записи в таблицу!");
    //}
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
void SellHistWindow::clicked_on_find() {
    impl = QSharedPointer<Implement>(new FindSellImplement());
    QMap<QString, QString> titles = {{"title", "Найти запись."}, {"exec", "Найти"}};
    InpFieldsAbstr = new SellAbstr(titles, fields, keys, SEntity, impl, this, true);
    connect(impl.get(),
            QOverload<const QString&>::of(&Implement::exec_clicked_signal),
            this,
            &SellHistWindow::find_record_db);
    impl.reset();
    emit summoned_child(InpFieldsAbstr);
}

/*!
 * \brief Установка фильтра для модели согласно запросу по полям записи.
 * \param where - SQL-запрос типа "WHERE ..."
 */
void SellHistWindow::find_record_db(const QString& where) {
    qDebug() << where;
    SEntity->setFilter(where);
    if (SEntity->rowCount() == 0) {
        QMessageBox::warning(this, "Ошибка!",
                             "По заданному запросу ничего не найдено!");
        SEntity->setFilter("");
        if (SEntity->rowCount() == 0) {
            SEntity->select();
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
void SellHistWindow::clicked_on_submit() {
    if (!SEntity->submitAll()) {
        qDebug() << SEntity->lastError();
    }
    else {
        if (!MEntity->apply_cache_info()) {
            qDebug() << MEntity->lastError();
        }
        if (!BEntity->apply_cache_info()) {
            qDebug() << BEntity->lastError();
        }
        revertact->setEnabled(false);
        saveact->setEnabled(false);
    }
}

/*!
 * \brief Отмена изменений в таблице.
 */
void SellHistWindow::clicked_on_revert() {
    MEntity->clear_cache();
    BEntity->clear_cache();
    revertact->setEnabled(false);
    saveact->setEnabled(false);
    SEntity->revertAll();
}

/*!
 * \brief Сброс поиска по таблице.
 */
void SellHistWindow::clicked_on_reset(){
    SEntity->setFilter("");
    if (SEntity->rowCount() == 0) {
        SEntity->select();
    }
    resetsrchact->setEnabled(false);
    delfoundact->setEnabled(false);
}

/*!
 * \brief Удаление записи или нескольих записей, выделенных в таблице.
 * \brief Согласно размеру списка выделенных записей запрашивается подтверждение на удаление,
 * а затем и удаление выделенных записей.
 */
void SellHistWindow::clicked_on_delete_selected() {
    QItemSelectionModel* selection = ui->table->selectionModel();
    QModelIndexList selection_list(selection->selectedRows());
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
                            SEntity->get_record(selection_list.at(0).row()).field(0).value().toLongLong())
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
            QSqlRecord current = SEntity->get_record(index.row());
            if (current.field(0).value().isNull()) {
                MEntity->remove_temp_sale(current.field(3).value().toInt());
                BEntity->remove_temp_withdraw(current.field(2).value().toInt());
            }
            SEntity->removeRecord(index.row());
        }
        saveact->setEnabled(true);
        revertact->setEnabled(true);
    }
}

/*!
 * \brief Удаление всех найденных записей.
 */
void SellHistWindow::clicked_on_delete_found() {
    if (SEntity->filter() == "") return;
    QMessageBox* msgBox = new QMessageBox("Удаление найденных",
                                          "Удалить все найденные записи?\n",
                                          QMessageBox::Question,
                                          QMessageBox::Button::Yes,
                                          QMessageBox::Button::No,
                                          QMessageBox::Button::Cancel,
                                          this);
    int resBtn = msgBox->exec();
    qDebug() << SEntity->rowCount();
    //model->
    if (resBtn == QMessageBox::Yes) {
        qDebug() << "DER ROWS " << SEntity->removeRecords(0, SEntity->rowCount());
        QMessageBox::information(this, "Успех!",
                             "Все найденные записи помечены на удаление. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
        saveact->setEnabled(true);
        revertact->setEnabled(true);
    }
}

void SellHistWindow::contextMenuRequested(const QPoint &mousepos) {
    QMenu * menu = new QMenu(this);
    menu->addAction(editact);
    menu->addAction(addact);
    menu->addAction(delselact);
    menu->addAction(delfoundact);
    menu->popup(ui->table->viewport()->mapToGlobal(mousepos));
}


