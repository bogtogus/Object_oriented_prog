#include "bonusprogwindow.h"
#include "ui_bonusprogwindow.h"
#include <QDebug>
#include <QMessageBox>

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
    connect(ui->back, &QPushButton::clicked, this, &BonusProgWindow::goback);

    this->setWindowTitle("Склад лекарств и препаратов");
    this->setGeometry(prnt->geometry());
    setAttribute(Qt::WA_DeleteOnClose);
    ui->reset->setEnabled(false);
    ui->delete_found->setEnabled(false);
    ui->revert->setEnabled(false);
    ui->submit->setEnabled(false);
    connect(ui->back, &QPushButton::clicked, this, &BonusProgWindow::goback);
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
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Возврат назад",
                                                                    "В базу данных не внемены изменения. Отменить их и вернуться назад?\n",
                                                                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                    QMessageBox::No);
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

// переход к форме добавления записи
void BonusProgWindow::add_record_db(QSqlRecord* record) {
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
    ui->submit->setEnabled(true);
    ui->revert->setEnabled(true);
}

// добавление записи
void BonusProgWindow::on_add_clicked() {
    impl = QSharedPointer<Implement>(new AddBonusImplement());
    QMap<QString, QString> titles = {{"title", "Добавить бонусную карту."}, {"exec", "Добавить"}};
    InFAbs = new BonusAbstr(titles, fields, keys, impl, this);
    titles.clear();
    connect(impl.get(), QOverload<QSqlRecord*>::of(&Implement::exec_clicked_signal), this, &BonusProgWindow::add_record_db);
    impl.reset();
    emit summoned_child(InFAbs);
}

// переход к форме поиска записи
void BonusProgWindow::on_find_clicked() {
    impl = QSharedPointer<Implement>(new FindBonusImplement());
    QMap<QString, QString> titles = {{"title", "Найти бонусную карту."}, {"exec", "Найти"}};
    InFAbs = new BonusAbstr(titles, fields, keys, impl, this);
    connect(impl.get(), QOverload<QString&>::of(&Implement::exec_clicked_signal), this, &BonusProgWindow::find_record_db);
    impl.reset();
    emit summoned_child(InFAbs);
}

// установка фильтра по введённым данным в поля
void BonusProgWindow::find_record_db(QString& where) {
    qDebug() << where;
    entity->setFilter(where);
    ui->reset->setEnabled(true);
    if (entity->rowCount() == 0) {
        QMessageBox::warning(this, "Ошибка!",
                             "По заданному запросу ничего не найдено!");
    }
    else {
        InFAbs->goback();
        ui->delete_found->setEnabled(true);
    }
}

// удаление выбранной строки
void BonusProgWindow::on_delete_selected_clicked() {
    int selected_row = ui->table->currentIndex().row();
    if (selected_row < 0) return;
    QMessageBox::StandardButton resBtn =
            QMessageBox::question(this, "Удаление",
                                  "Удалить бонусную карту №" +
                                  QString::number(entity->get_record(selected_row).field(0).value().toLongLong())
                                  + "?\n",
                                  QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                  QMessageBox::No);
    if (resBtn != QMessageBox::Yes) {
        return;
    }
    else {
        this->entity->removeRecord(selected_row);
        ui->submit->setEnabled(true);
        ui->revert->setEnabled(true);
    }
}

// отмена изменений в таблице
void BonusProgWindow::on_reset_clicked() {
    entity->setFilter("");
    if (entity->rowCount() == 0) {
        entity->select();
    }
    ui->reset->setEnabled(false);
    ui->delete_found->setEnabled(false);
}

// удаление всех найденных записей
void BonusProgWindow::on_delete_found_clicked() {
    if (entity->filter() == "") return;
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Удаление найденных.",
                                                                "Удалить все найденные карты?\n",
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    qDebug() << entity->rowCount();
    //model->
    if (resBtn == QMessageBox::Yes) {
        entity->removeRecords(0, entity->rowCount());
        QMessageBox::information(this, "Успех!",
                             "Все найденные бонусные карты помечены на удаление. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
        ui->submit->setEnabled(true);
        ui->revert->setEnabled(true);
    }
}

// подтверждение изменений в таблице
void BonusProgWindow::on_submit_clicked() {
    if (!entity->submitAll()) {
        qDebug() << entity->lastError();
    }
    else {
        ui->revert->setEnabled(false);
        ui->submit->setEnabled(false);
    }
}

// отмена изменений в таблице
void BonusProgWindow::on_revert_clicked() {
    ui->revert->setEnabled(false);
    ui->submit->setEnabled(false);
    entity->revertAll();
}

