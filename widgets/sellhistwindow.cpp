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
    InFAbs = nullptr;
    record_being_edited = nullptr;
    first_add = -1;
    ui->setupUi(this);
    ui->verticalLayout->setSpacing(1);
    init_menubar();
    ui->verticalLayout->insertWidget(0, menubar);
    ui->verticalLayout->insertWidget(1, toolbar);

    this->setWindowTitle("История продаж");
    this->setGeometry(prnt->geometry());
    setAttribute(Qt::WA_DeleteOnClose);
    QVector<QString> temp = SEntity->get_fnames();
    init_table();
    for (int i = 0; i < SEntity->get_model()->record().count(); i++) {
        keys.append(SEntity->get_model()->record().fieldName(i));
        fields.insert(keys[i], temp[i]);
    }
    temp.clear();
    ui->table->resizeRowsToContents();
    ui->table->setWordWrap(true);
}

SellHistWindow::~SellHistWindow() {
    qDebug() << "DEL hist {";
    temp_sales.clear();
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
    InFAbs = new SellAbstr(titles, fields, keys, SEntity, impl, this);
    titles.clear();
    connect(impl.get(),
            QOverload<const QSqlRecord*>::of(&Implement::exec_clicked_signal),
            this,
            &SellHistWindow::add_record_db);
    impl.reset();
    emit summoned_child(InFAbs);
}

/*!
 * \brief Добавление записи по данным, полученным из окна заполнения данных.
 * \param record - сформированная запись.
 */
void SellHistWindow::add_record_db(const QSqlRecord* record) {
    int med_id = record->value(keys[3]).toInt();
    int sell_count = record->value(keys[4]).toInt();

    if (temp_sales.contains(med_id) &&
            temp_sales[med_id] < sell_count) {
        QMessageBox::warning(this, "Ошибка!",
                             "Ошибка добавления записи в таблицу! "
                             "На складе нет достаточного количества лекарства (id " +
                             QString::number(med_id) + ")! Запрошено " +
                             QString::number(sell_count) + "шт. , в наличии " +
                             QString::number(temp_sales[med_id]) + "шт.");
        return;
    }
    if (SEntity->addRecord(record)) {
        QMessageBox::information(this, "Успех!",
                             "Введённая запись добавлена в временное представление таблицы базы данных. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
        if (first_add < 0) first_add = SEntity->rowCount() - 1;
        // Если в временном хранилище транзакций ещё не было операций
        // с данным id лекартсва, то она сначала добавляется в это хранилище
        if (!temp_sales.contains(med_id)) {
            int db_med_count = 0;
            if (!MEntity->get_med_amount(med_id, db_med_count)) {
                qDebug() << MEntity->lastError();
                return;
            }
            temp_sales.insert(med_id, db_med_count);
        }
        temp_sales[med_id] -= sell_count;
        qDebug() << temp_sales;
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
    InFAbs = new SellAbstr(titles, fields, keys, SEntity, impl, this);
    InFAbs->fill_fields(record, row);
    titles.clear();
    connect(impl.get(),
            QOverload<const QSqlRecord&, const int>::of(&Implement::exec_clicked_signal),
            this,
            &SellHistWindow::edit_record_db);
    impl.reset();
    record_being_edited = &record;
    emit summoned_child(InFAbs);
}

/*!
 * \brief Изменение записи согласно данным, полученным из окна заполнения данных.
 * \param record - сформированная новая запись.
 * \param row - строка в модели таблицы, где находится старая запись.
 */
void SellHistWindow::edit_record_db(const QSqlRecord & record, const int row) {
    int old_med_id = record_being_edited->value(keys[3]).toInt();
    int med_id = record.value(keys[3]).toInt();
    int sell_count = record.value(keys[4]).toInt();
    if (temp_sales.contains(med_id) &&
            temp_sales[med_id] < sell_count) {
        QMessageBox::warning(this, "Ошибка!",
                             "Ошибка добавления записи в таблицу! "
                             "На складе нет достаточного количества лекарства (id " +
                             QString::number(med_id) + ")! Запрошено " +
                             QString::number(sell_count) + "шт. , в наличии " +
                             QString::number(temp_sales[med_id]) + "шт.");
        return;
    }
    if (SEntity->setRecord(row, record)) {
        QMessageBox::information(this, "Успех!",
                             "Введённая запись отредактирована. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
        if (old_med_id != med_id && temp_sales.contains(old_med_id)) {
            temp_sales.remove(old_med_id);
        }
        if (!temp_sales.contains(med_id)) {
            int db_med_count = 0;
            if (!MEntity->get_med_amount(med_id, db_med_count)) {
                qDebug() << MEntity->lastError();
                return;
            }
            temp_sales.insert(med_id, db_med_count);
        }
        temp_sales[med_id] -= sell_count;
        qDebug() << temp_sales;
        delete record_being_edited;
        record_being_edited = nullptr;
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
void SellHistWindow::clicked_on_find() {
    impl = QSharedPointer<Implement>(new FindSellImplement());
    QMap<QString, QString> titles = {{"title", "Найти запись."}, {"exec", "Найти"}};
    InFAbs = new SellAbstr(titles, fields, keys, SEntity, impl, this);
    connect(impl.get(),
            QOverload<const QString&>::of(&Implement::exec_clicked_signal),
            this,
            &SellHistWindow::find_record_db);
    impl.reset();
    emit summoned_child(InFAbs);
}

/*!
 * \brief Установка фильтра для модели согласно запросу по полям записи.
 * \param where - SQL-запрос типа "WHERE ..."
 */
void SellHistWindow::find_record_db(const QString& where) {
    qDebug() << where;
    SEntity->setFilter(where);
    resetsrchact->setEnabled(true);
    if (SEntity->rowCount() == 0) {
        QMessageBox::warning(this, "Ошибка!",
                             "По заданному запросу ничего не найдено!");
    }
    else {
        InFAbs->goback();
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
        if (first_add >= 0) {
            QMap<int, int> ::iterator it = temp_sales.begin();
            for (; it != temp_sales.end(); it++) {
                qDebug() << *it;
                if (!MEntity->update_record(it.key(), it.value())) {
                    qDebug() << MEntity->lastError();
                }
            }
        }
        revertact->setEnabled(false);
        saveact->setEnabled(false);
    }
}

/*!
 * \brief Отмена изменений в таблице.
 */
void SellHistWindow::clicked_on_revert() {
    temp_sales.clear();
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
            if (SEntity->get_record(index.row()).field(0).value().isNull()) {
                temp_sales.remove(SEntity->get_record(index.row()).field(3).value().toInt());
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


