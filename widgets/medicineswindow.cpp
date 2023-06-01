#include "medicineswindow.h"
#include "ui_medicineswindow.h"
#include "inputfields.h"
#include <QDebug>
#include <QMessageBox>

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
    ui->reset->setEnabled(false);
    ui->delete_found->setEnabled(false);
    ui->revert->setEnabled(false);
    ui->submit->setEnabled(false);
    connect(ui->back, &QPushButton::clicked, this, &MedicinesWindow::goback);
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

// нажание на кнопку возврата назад
void MedicinesWindow::goback() {
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

void MedicinesWindow::closeEvent(QCloseEvent*) {
    qDebug() << "close MedicinesWindow";
}

// переход к форме добавления записи
void MedicinesWindow::on_add_clicked() {
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
    ui->submit->setEnabled(true);
    ui->revert->setEnabled(true);

}

// удаление выбранной строки
void MedicinesWindow::on_delete_selected_clicked() {
    int selected_row = ui->table->currentIndex().row();
    if (selected_row < 0) return;
    QMessageBox::StandardButton resBtn =
            QMessageBox::question(this, "Удаление",
                                  "Удалить запись №" +
                                  QString::number(entity->get_record(selected_row).field(0).value().toLongLong())
                                  + "?\n",
                                  QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                  QMessageBox::No);
    if (resBtn != QMessageBox::Yes) {
        return;
    }
    else {
        entity->removeRecord(selected_row);
        ui->submit->setEnabled(true);
        ui->revert->setEnabled(true);
    }
    //if (ui->table->)
}

// подтверждение изменений в таблице
void MedicinesWindow::on_submit_clicked() {
    if (!entity->submitAll()) {
        qDebug() << entity->lastError();
    }
    else {
        ui->revert->setEnabled(false);
        ui->submit->setEnabled(false);
    }
}

// отмена изменений в таблице
void MedicinesWindow::on_revert_clicked() {
    ui->revert->setEnabled(false);
    ui->submit->setEnabled(false);
    entity->revertAll();
}

// переход к форме поиска записи
void MedicinesWindow::on_find_clicked() {
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

// отмена изменений в таблице
void MedicinesWindow::on_reset_clicked(){
    entity->setFilter("");
    if (entity->rowCount() == 0) {
        entity->select();
    }
    ui->reset->setEnabled(false);
    ui->delete_found->setEnabled(false);
}

// удаление всех найденных записей
void MedicinesWindow::on_delete_found_clicked() {
    if (entity->filter() == "") return;
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Удаление найденных.",
                                                                "Удалить все найденные записи?\n",
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    qDebug() << entity->rowCount();
    //model->
    if (resBtn == QMessageBox::Yes) {
        entity->removeRecords(0, entity->rowCount());
        QMessageBox::information(this, "Успех!",
                             "Все найденные записи помечены на удаление. "
                             "Чтобы сохранить изменения, нажмите \"Сохранить\" в меню управления таблицей.");
        ui->submit->setEnabled(true);
        ui->revert->setEnabled(true);
    }
}

