#include "sellhistwindow.h"
#include "ui_sellhistwindow.h"
#include <QMessageBox>
#include <QDebug>

SellHistWindow::SellHistWindow(QWidget *parent,
                               MedsEntity* me,
                               BonusEntity* be,
                               SellEntity* se) :
    aChildWin(parent),
    ui(new Ui::SellHistWindow)
{
    qDebug() << "CRT hist";
    prnt = parent;
    MEntity = me;
    BEntity = be;
    SEntity = se;
    InFAbs = nullptr;
    first_add = -1;
    ui->setupUi(this);
    this->setWindowTitle("История продаж");
    this->setGeometry(prnt->geometry());
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->back, &QPushButton::clicked, this, &SellHistWindow::goback);
    ui->reset->setEnabled(false);
    ui->delete_found->setEnabled(false);
    ui->revert->setEnabled(false);
    ui->submit->setEnabled(false);
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
    impl.reset();
    temp_sales.clear();
    delete ui;
    qDebug() << "}";
}

// Реагирование на изменение размеров окна
// задание нужной ширины каждого столбца таблицы
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
}

// нажание на кнопку возврата назад
void SellHistWindow::goback() {
    if (SEntity->isDirty()) {
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Возврат назад",
                                                                    "В базу данных не внемены изменения. Отменить их и вернуться назад?\n",
                                                                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::No);
        if (resBtn == QMessageBox::Yes) {
            SEntity->revertAll();
            emit goback_signal(this);
        }
    }
    else {
        emit goback_signal(this);
    }
}

void SellHistWindow::closeEvent(QCloseEvent *event) {
    qDebug() << "close SellHistWindow";
    event->accept();
}

// переход к форме добавления записи
void SellHistWindow::on_add_clicked() {
    qDebug() << fields;
    impl = QSharedPointer<Implement>(new AddSellImplement());
    QMap<QString, QString> titles = {{"title", "Добавить запись."}, {"exec", "Добавить"}};
    InFAbs = new SellAbstr(titles, fields, keys, SEntity, impl, this);
    titles.clear();
    connect(impl.get(), QOverload<QSqlRecord*>::of(&Implement::exec_clicked_signal), this, &SellHistWindow::add_record_db);
    impl.reset();
    emit summoned_child(InFAbs);
}

// добавление записи
void SellHistWindow::add_record_db(QSqlRecord* record) {
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
    ui->submit->setEnabled(true);
    ui->revert->setEnabled(true);
}

// удаление выбранной строки
void SellHistWindow::on_delete_selected_clicked() {
    int selected_row = ui->table->currentIndex().row();
    if (selected_row < 0) return;
    QMessageBox::StandardButton resBtn =
            QMessageBox::question(this, "Удаление",
                                  "Удалить запись №" +
                                  QString::number(SEntity->get_record(selected_row).field(0).value().toLongLong())
                                  + "?\n",
                                  QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                  QMessageBox::No);
    if (resBtn != QMessageBox::Yes) {
        return;
    }
    else {
        qDebug() << "DEL ROW &" << this->SEntity->removeRecord(selected_row);
        ui->submit->setEnabled(true);
        ui->revert->setEnabled(true);
    }
    //if (ui->table->)
}

// подтверждение изменений в таблице
void SellHistWindow::on_submit_clicked() {
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
        ui->revert->setEnabled(false);
        ui->submit->setEnabled(false);
    }
}

// отмена изменений в таблице
void SellHistWindow::on_revert_clicked() {
    temp_sales.clear();
    ui->revert->setEnabled(false);
    ui->submit->setEnabled(false);
    SEntity->revertAll();
}

// переход к форме поиска записи
void SellHistWindow::on_find_clicked() {
    impl = QSharedPointer<Implement>(new FindSellImplement());
    QMap<QString, QString> titles = {{"title", "Найти запись."}, {"exec", "Найти"}};
    InFAbs = new SellAbstr(titles, fields, keys, SEntity, impl, this);
    connect(impl.get(), QOverload<QString&>::of(&Implement::exec_clicked_signal), this, &SellHistWindow::find_record_db);
    impl.reset();
    emit summoned_child(InFAbs);
}

// установка фильтра по введённым данным в поля
void SellHistWindow::find_record_db(QString& where) {
    qDebug() << where;
    SEntity->setFilter(where);
    ui->reset->setEnabled(true);
    if (SEntity->rowCount() == 0) {
        QMessageBox::warning(this, "Ошибка!",
                             "По заданному запросу ничего не найдено!");
    }
    else {
        InFAbs->goback();
        ui->delete_found->setEnabled(true);
    }
}

// отмена изменений в таблице
void SellHistWindow::on_reset_clicked(){
    SEntity->setFilter("");
    if (SEntity->rowCount() == 0) {
        SEntity->select();
    }
    ui->reset->setEnabled(false);
    ui->delete_found->setEnabled(false);
}

// удаление всех найденных записей
void SellHistWindow::on_delete_found_clicked() {
    if (SEntity->filter() == "") return;
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Удаление найденных.",
                                                                "Удалить все найденные записи?\n",
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    qDebug() << SEntity->rowCount();
    //model->
    if (resBtn == QMessageBox::Yes) {
        qDebug() << "DER ROWS " << SEntity->removeRecords(0, SEntity->rowCount());
        QMessageBox::information(this, "Успех!",
                             "Все найденные записи помечены на удаление. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
        ui->submit->setEnabled(true);
        ui->revert->setEnabled(true);
    }
}

