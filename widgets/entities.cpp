#include "entities.h"

Entity::Entity(QSharedPointer<QSqlDatabase> odb) {
    db = odb;
    if (!db->isOpen()) {
        if (!db->open()) {
            qDebug() << "Unable to open db.";
            return;
        }
        qDebug() << "open db";
    }
    model = new QSqlTableModel(this, db->database());
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

Entity::~Entity() {
    qDebug() << "del entity";
    field_names.clear();
    if (model) {
        //model->disconnect();
        delete model;
    }
    if (db->isOpen()) {
        qDebug() << "close db";
        db->close();
    }
    db.clear();
}

// получение модели представления таблицы
QSqlTableModel* Entity::get_model() const {
    return model;
}

QSqlRecord Entity::get_record(const int number) const {
    return model->record(number);
}

QSqlError Entity::lastError() const {
    return model->lastError();
}

int Entity::rowCount() const {
    return model->rowCount();
}

int Entity::columnCount() const {
    return model->columnCount();
}

// получить фильтр
QString Entity::filter() const {
    return model->filter();
}

// Есть ли неподтверждённые изменения
bool Entity::isDirty() const {
    return model->isDirty();
}

// получить все наименования столбцов
QVector<QString> Entity::get_fnames() const {
    return field_names;
}

// установка фильтра
void Entity::setFilter(const QString& f) {
    model->setFilter(f);
}

bool Entity::addRecord(const QSqlRecord* record) {
    return model->insertRecord(-1, *record);
}

bool Entity::removeRecord(const int number) {
    return model->removeRow(number);
}

bool Entity::removeRecords(const int s, const int e) {
    return model->removeRows(s, e);
}

bool Entity::setRecord(const int row, const QSqlRecord& record){
    return model->setRecord(row, record);
}

// отменить все изменения
void Entity::revertAll() {
    model->revertAll();
}
// подтвердить все изменения
bool Entity::submitAll() {
    return model->submitAll();
}

// считать данные в модель из таблицы(обычно все, если нет фильтра)
bool Entity::select() {
    return model->select();
}


MedsEntity::MedsEntity(QSharedPointer<QSqlDatabase> odb,
                       const QVector<QString>& fn)
    : Entity(odb)
{
    if (!db->isOpen()) {
        qDebug() << "Unable to open db.";
        return;
    }
    model->setTable("medicines");
    model->select();
    this->field_names = fn;
    for (int i = 0; i < model->columnCount(); i++) {
        model->setHeaderData(i, Qt::Horizontal, field_names[i]);
    }
    qDebug() << "Meds Rows: " << (model->rowCount());
}

MedsEntity::~MedsEntity() {

}

// Получить количество лекарства с заданным id
bool MedsEntity::get_med_amount(const int id, int& result) const {
    QSqlQuery query(db->connectionName());
    int count = 0;
    query.prepare("SELECT pieces FROM medicines WHERE id = " + QString::number(id));
    if (query.exec()) {
        query.first();
        count = query.value(0).toInt();
        query.finish();
        result = count;
        return true;
    }
    else {
        result = -1;
        query.finish();
        return false;
    }
}

// Обновить количество лекарства на складе
bool MedsEntity::update_record(const int id, const int pieces) {
    QSqlQuery query(db->connectionName());
    query.prepare("UPDATE medicines SET pieces = " +
                  QString::number(pieces) +
                  " WHERE id = " + QString::number(id));
    bool ret = query.exec();
    query.finish();
    return ret;
}

// Все айди лекарств
QVector<QString> MedsEntity::get_all_ids() const {
    QVector<QString> medicines_ids;
    QSqlQuery query;
    query.prepare("SELECT id FROM medicines;");
    if (query.exec()) {
        while (query.next()) {
            medicines_ids.append(query.value(0).toString());
        }
    }
    else {
        qDebug() << query.lastError();
    }
    query.finish();
    return medicines_ids;
}

BonusEntity::BonusEntity(QSharedPointer<QSqlDatabase> odb,
                       const QVector<QString>& fn)
    : Entity(odb)
{
    if (!db->isOpen()) {
        qDebug() << "Unable to open db.";
        return;
    }
    model->setTable("reged_customers");
    model->select();
    this->field_names = fn;
    for (int i = 0; i < model->columnCount(); i++) {
        model->setHeaderData(i, Qt::Horizontal, field_names[i]);
    }
    qDebug() << "Bonus Rows: " << (model->rowCount());
}

BonusEntity::~BonusEntity() {

}

// получить все номера бонусных карт
QVector<QString> BonusEntity::get_all_cards() const {
    QVector<QString> card_numbers;
    QSqlQuery query;
    query.prepare("SELECT card_number FROM reged_customers;");
    if (query.exec()) {
        while (query.next()) {
            card_numbers.append(query.value(0).toString());
        }
    }
    else {
        qDebug() << query.lastError();
    }
    query.finish();
    return card_numbers;
}

SellEntity::SellEntity(QSharedPointer<QSqlDatabase> odb,
                       const QVector<QString>& fn,
                       const MedsEntity* me,
                       const BonusEntity* be)
    : Entity(odb)
{
    if (!db->isOpen()) {
        qDebug() << "Unable to open db.";
        return;
    }
    model->setTable("sales_history");
    model->select();
    this->field_names = fn;
    for (int i = 0; i < model->columnCount(); i++) {
        model->setHeaderData(i, Qt::Horizontal, field_names[i]);
    }
    this->meds = me;
    this->bonuscards = be;
    qDebug() << "Sales Rows: " << (model->rowCount());
}

SellEntity::~SellEntity() {
    this->meds = nullptr;
    this->bonuscards = nullptr;
}

// получить все номера бонусных карт
QVector<QString> SellEntity::get_all_cards() const {
    return bonuscards->get_all_cards();
}

// Все айди лекарств
QVector<QString> SellEntity::get_all_ids() const {
    return meds->get_all_ids();
}

// Получить количество лекарства с заданным id
bool SellEntity::get_med_amount(const int id, int& amount) const {
    return meds->get_med_amount(id, amount);
}
