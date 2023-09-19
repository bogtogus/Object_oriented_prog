#include "entities.h"

/*!
 * \brief Конструктор сущности.
 * \param odb - разделяемый указатель на объект базы данных.
 * \details Если БД не открыта, она открывается в конструкторе.
 */
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

/*!
 * \brief Конструктор перемещения.
 * \param moved - другой объект сущности, в последствии очищаемый.
 */
Entity::Entity(Entity && moved) {
    this->db = moved.db;
    this->model = moved.model;
    moved.model = nullptr;
    moved.db.clear();
    this->field_names = moved.field_names;
}

/*!
 * \brief Оператор присвоения перемещением.
 * \param moved - другой объект сущности, в последствии очищаемый.
 * \return
 */
Entity &Entity::operator=(Entity && moved) {
    if (&moved != this) {
        this->db = moved.db;
        moved.db.clear();
        if (this->model) delete this->model;
        this->model = moved.model;
        moved.model = nullptr;
        this->field_names = moved.field_names;
    }
    return *this;
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

/*!
 * \brief Получение модели представления таблицы.
 * \return модель представления таблицы.
 */
QSqlTableModel* Entity::get_model() const {
    return model;
}

/*!
 * \brief Получение записи по её номеру.
 * \param number - номер записи в таблице.
 * \return запись.
 */
QSqlRecord Entity::get_record(const int number) const {
    return model->record(number);
}

/*!
 * \brief Последняя ошибка.
 * \return последняя ошибка.
 */
QSqlError Entity::lastError() const {
    return model->lastError();
}

/*!
 * \brief Количество строк.
 * \return количество строк.
 */
int Entity::rowCount() const {
    return model->rowCount();
}

/*!
 * \brief Количество столбцов.
 * \return количество столбцов.
 */
int Entity::columnCount() const {
    return model->columnCount();
}

/*!
 * \brief Получение фильтра модели.
 * \return фильтр модели.
 */
QString Entity::filter() const {
    return model->filter();
}

/*!
 * \brief Проверка, есть ли неподтверждённые изменения.
 * \return
 */
bool Entity::isDirty() const {
    return model->isDirty();
}

/*!
 * \brief Получить все наименования столбцов.
 * \return все наименования столбцов.
 */
QVector<QString> Entity::get_fnames() const {
    return field_names;
}

/*!
 * \brief Установка фильтра
 * \param f - фильтр.
 */
void Entity::setFilter(const QString& f) {
    model->setFilter(f);
}

/*!
 * \brief Добавление записи в конец.
 * \param record - запись.
 * \return
 */
bool Entity::addRecord(const QSqlRecord* record) {
    return model->insertRecord(-1, *record);
}

/*!
 * \brief Удаление записи по её номеру.
 * \param number - номер записи.
 * \return
 */
bool Entity::removeRecord(const int number) {
    return model->removeRow(number);
}

/*!
 * \brief Удаление записей по диапазону.
 * \param s - первый номер записи.
 * \param e - последний номер записи.
 * \return
 */
bool Entity::removeRecords(const int s, const int e) {
    return model->removeRows(s, e);
}

/*!
 * \brief Изменение записи.
 * \param row - номер записи.
 * \param record - новая запись.
 * \return успешность операции.
 */
bool Entity::setRecord(const int row, const QSqlRecord& record){
    return model->setRecord(row, record);
}

/*!
 * \brief Отмена изменений в таблице.
 */
void Entity::revertAll() {
    model->revertAll();
}

/*!
 * \brief Подтверждение изменений в таблице.
 * \return успешность операции.
 */
bool Entity::submitAll() {
    return model->submitAll();
}

/*!
 * \brief Выбор всех записей тиаблицы.
 * \return успешность операции.
 */
bool Entity::select() {
    return model->select();
}


/*!
 * \brief Конструктор сущности склада лекарств.
 * \param odb - разделяемый указатель на БД.
 * \param fn - имена столбцов таблицы.
 */
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

/*!
 * \brief Получение количества лекарства с заданным id.
 * \param id - id лекарства.
 * \param result - количество лекарства.
 * \return
 */
bool MedsEntity::get_med_amount(const int id, int& result) const {
    QSqlQuery query(db->connectionName());
    int count = 0;
    bool succeed = false;
    query.prepare("SELECT pieces FROM medicines WHERE id = :id");
    query.bindValue(":id", id);
    if (query.exec()) {
        qDebug() << query.executedQuery();
        query.first();
        count = query.value(0).toInt();
        result = count;
        succeed = true;
    }
    else {
        result = -1;
        succeed = false;
    }
    query.finish();
    return succeed;
}

/*!
 * \brief Получение цены лекарства по его айди.
 * \param id - айди лекартсва.
 * \param price - возвращаемая цена лекарства.
 * \return успешность операции.
 */
int MedsEntity::get_med_price(const int id, int &price) const {
    QSqlQuery query(db->connectionName());
    query.prepare("SELECT price_for_one FROM medicines WHERE id = :id");
    query.bindValue(":id", id);
    if (query.exec()) {
        qDebug() << query.executedQuery() << " " << id;
        query.first();
        int temp = query.value(0).toInt();
        query.finish();
        price = temp;
        return true;
    }
    else {
        price = -1;
        query.finish();
        return false;
    }
}

/*!
 * \brief Обновление количества лекарства на складе.
 * \param id - id лекарства.
 * \param pieces - новое значение количества лекарства.
 * \return успешность операции.
 */
bool MedsEntity::update_record(const int id, const int pieces) {
    if (pieces < 0) return false;
    QSqlQuery query(db->connectionName());
    query.prepare("UPDATE medicines SET pieces = :pieces WHERE id = :id");
    query.bindValue(":pieces", pieces);
    query.bindValue(":id", id);
    bool ret = query.exec();
    query.finish();
    return ret;
}

/*!
 * \brief Получение всех айди лекарств в строковом виде.
 * \return массив айди лекарств в строковом виде.
 */
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

/*!
 * \brief Конструктор сущности бонусных карт.
 * \param odb - разделяемый указатель на БД.
 * \param fn - имена столбцов таблицы.
 */
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

/*!
 * \brief Получение всех номеров карт в строковом виде.
 * \return массив номеров карт в строковом виде.
 */
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

/*!
 * \brief Конструктор сущности продаж лекарств.
 * \param odb - разделяемый указатель на БД.
 * \param fn - имена столбцов таблицы.
 * \param me - указатель на сущность лекарств.
 * \param be - указатель на сущность бонусных карт.
 */
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

/*!
 * \brief Получение всех номеров карт в строковом виде.
 * \details Операция делегируется сущности бонусных карт.
 * \return массив номеров карт в строковом виде.
 */
QVector<QString> SellEntity::get_all_cards() const {
    return bonuscards->get_all_cards();
}

/*!
 * \brief Получение всех айди лекарств в строковом виде.
 * \return массив айди лекарств в строковом виде.
 */
QVector<QString> SellEntity::get_all_ids() const {
    return meds->get_all_ids();
}

/*!
 * \brief Получение количества лекарства с заданным id.
 * \param id - айди.
 * \param amount - количество лекарства.
 * \return успешность операции.
 */
bool SellEntity::get_med_amount(const int id, int& amount) const {
    return meds->get_med_amount(id, amount);
}

/*!
 * \brief Получение стоимости лекарства с заданным id.
 * \param id - айди.
 * \param amount - стоимость лекарства.
 * \return успешность операции.
 */
int SellEntity::get_med_price(const int id, int &price) const {
    return meds->get_med_price(id, price);
}
