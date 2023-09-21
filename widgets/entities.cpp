#include "entities.h"


SqlTableModel_Currency::SqlTableModel_Currency(QObject *parent, QSqlDatabase db, const int price_column) : QSqlTableModel(parent, db) {
    this->price_column = price_column;
}
SqlTableModel_Currency::~SqlTableModel_Currency() { }

void SqlTableModel_Currency::setCurrencyColumn(const int column) {
    this->price_column = column;
}

QVariant SqlTableModel_Currency::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole){
        if(price_column > 0 && index.column() == price_column) {
            int value = QSqlTableModel::data(index, role).toInt();
            return QVariant( QString("%1,%2").arg(QString::number(value / 100))
                                             .arg(QString::number(value % 100), 2, QLatin1Char('0')));
            /*QString value = QSqlTableModel::data(index, role).toString();
            int length = value.length();
            if (length > 2) {
                return QVariant( QString("%1,%2").arg(QStringRef(&value, 0, length - 2))
                                                 .arg(QStringRef(&value, length - 2, 2), 2, QLatin1Char('0')));
            }
            else {
                return QVariant( QString("0,%1").arg(QStringRef(&value, length - 2, 2), 2, QLatin1Char('0')));
            }
            */
        }
    }
    return QSqlTableModel::data(index, role);
}



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
    model = new SqlTableModel_Currency(this, db->database());
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
    this->readable_names = moved.readable_names;
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
        this->readable_names = moved.readable_names;
    }
    return *this;
}

Entity::~Entity() {
    qDebug() << "del entity";
    readable_names.clear();
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
QVector<QString> Entity::get_readable_names() const {
    return readable_names;
}

QVector<QString> Entity::get_column_names() const {
    return column_names;
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
 * \brief Выбор всех записей таблицы.
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
    model->setCurrencyColumn(7);
    model->setTable("medicines");
    //model->select();
    this->readable_names = fn;
    for (int i = 0; i < model->columnCount(); i++) {
        this->column_names.append(model->headerData(i, Qt::Horizontal).toString());
        model->setHeaderData(i, Qt::Horizontal, readable_names[i]);
    }
}

MedsEntity::~MedsEntity() {
    temp_amount.clear();
}

/*!
 * \brief Получение количества лекарства с заданным id.
 * \param id - id лекарства.
 * \param result - количество лекарства.
 * \return количество лекартва на складе.
 * \details При наличии в кэше лекартсва с заданным id возарщает значение из кэша.
 */
bool MedsEntity::get_med_amount(const int med_id, int& result) const {
    bool succeed = false;
    if (temp_amount.contains(med_id)) {
        result = temp_amount[med_id];
        succeed = true;
    }
    else {
        QSqlQuery query(db->connectionName());
        int count = 0;

        query.prepare("SELECT pieces FROM medicines WHERE id = :id");
        query.bindValue(":id", med_id);
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
        }
    return succeed;
}

/*!
 * \brief Получение цены лекарства по его айди.
 * \param id - айди лекартсва.
 * \param price - возвращаемая цена лекарства.
 * \return успешность операции.
 */
bool MedsEntity::get_med_price(const int med_id, int &price) const {
    QSqlQuery query(db->connectionName());
    query.prepare("SELECT price_for_one FROM medicines WHERE id = :id");
    query.bindValue(":id", med_id);
    if (query.exec()) {
        qDebug() << query.executedQuery() << " " << med_id;
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
 * \brief Получение количества лекарств из кэша.
 * \param med_id - id лекарства.
 * \return количество лекарств из кэша.
 */
int MedsEntity::get_med_amount(const int med_id) const {
    return temp_amount.value(med_id, -1);
}

/*!
 * \brief Проверка, достаточно ли лекарства, количество которого хранится в кэше.
 * \param med_id - id лекарства.
 * \param amount - запрашиваемое количество лекартсва.
 * \return достаточно/недостаточно.
 */
bool MedsEntity::is_enough_meds(const int med_id, const int amount) const {
    if (med_id < 1) return false;
    return (bool)(temp_amount.value(med_id, -1) >= amount);
}

/*!
 * \brief Содержится ли в кэше лекарство с заданным id.
 * \param med_id - id лекарства.
 * \return содержится/ не содержится.
 */
bool MedsEntity::cache_contains(const int med_id) const {
    return temp_amount.contains(med_id);
}

/*!
 * \brief Добавление в кэш нового значения количества лекарства после продажи.
 * \param med_id - id лекарства.
 * \param sold - количество проданного лекарства.
 * \return успешность операции.
 */
bool MedsEntity::add_temp_sale(const int med_id, const int sold) {
    if (med_id < 1) return false;
    if (!temp_amount.contains(med_id)) {
        int db_med_count = 0;
        if (!get_med_amount(med_id, db_med_count)) {
            qDebug() << model->lastError();
            return false;
        }
        if (db_med_count < 0) return false;
        temp_amount.insert(med_id, db_med_count);
    }
    if (temp_amount[med_id] < sold) return false;

    temp_amount[med_id] -= sold;
    qDebug() << temp_amount;
    return true;
}

/*!
 * \brief Удаление из кэша информации об оставшемся количестве лекартсва на складе.
 * \param med_id - id лекарства.
 * \return успешность операции.
 */
bool MedsEntity::remove_temp_sale(const int med_id) {
    if (med_id < 1) return false;
    return (bool)temp_amount.remove(med_id);
}

/*!
 * \brief Очистка кэша от временных значений количества лекарств на складе.
 */
void MedsEntity::clear_cache() {
    temp_amount.clear();
}

/*!
 * \brief Обновить информацию в БД согласно информации в кэше.
 */
void MedsEntity::apply_cache_info() {
    QMap<int, int> ::iterator it = temp_amount.begin();
    for (; it != temp_amount.end(); it++) {
        qDebug() << *it;
        if (!update_amount(it.key(), it.value())) {
            qDebug() << model->lastError();
        }
    }
}


/*!
 * \brief Обновление количества лекарства на складе.
 * \param id - id лекарства.
 * \param pieces - новое значение количества лекарства.
 * \return успешность операции.
 */
bool MedsEntity::update_amount(const int med_id, const int pieces) {
    if (med_id < 1 || pieces < 0) return false;
    QSqlQuery query(db->connectionName());
    query.prepare("UPDATE medicines SET pieces = :pieces WHERE id = :id");
    query.bindValue(":pieces", pieces);
    query.bindValue(":id", med_id);
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
    model->setTable("bonus_cards");
    //model->select();
    this->readable_names = fn;
    for (int i = 0; i < model->columnCount(); i++) {
        this->column_names.append(model->headerData(i, Qt::Horizontal).toString());
        model->setHeaderData(i, Qt::Horizontal, readable_names[i]);
    }
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
    query.prepare("SELECT card_number FROM bonus_cards;");
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

bool BonusEntity::get_card_balance(const int card_number, int &balance) const {
    bool succeed = false;
    if (temp_balance.contains(card_number)) {
        balance = temp_balance[card_number];
        succeed = true;
    }
    else {
        QSqlQuery query(db->connectionName());
        int count = 0;
        query.prepare("SELECT balance FROM bonus_cards WHERE card_number = :card_number");
        query.bindValue(":card_number", card_number);
        if (query.exec()) {
            qDebug() << query.executedQuery();
            query.first();
            count = query.value(0).toInt();
            balance = count;
            succeed = true;
        }
        else {
            balance = -1;
            succeed = false;
        }
        query.finish();
    }
    return succeed;
}

int BonusEntity::get_card_balance(const int card_number) const {
    return temp_balance.value(card_number, -1);
}

bool BonusEntity::is_enough_bonuses(const int card_number, const int amount) const {
    if (card_number < 10000000) return false;
    return (bool)(temp_balance.value(card_number, -1) >= amount);
}

bool BonusEntity::cache_contains(const int card_number) const {
    if (card_number < 10000000) return false;
    return temp_balance.contains(card_number);
}

bool BonusEntity::add_temp_withdraw(const int card_number, const int withdrown) {
    if (card_number < 10000000) return false;
    if (!temp_balance.contains(card_number)) {
        int db_med_count = 0;
        if (!get_card_balance(card_number, db_med_count)) {
            qDebug() << model->lastError();
            return false;
        }
        if (db_med_count < 0) return false;
        temp_balance.insert(card_number, db_med_count);
    }
    if (temp_balance[card_number] < withdrown) return false;

    temp_balance[card_number] -= withdrown;
    qDebug() << temp_balance;
    return true;
}

bool BonusEntity::remove_temp_withdraw(const int card_number) {
    if (card_number < 10000000) return false;
    return (bool)(temp_balance.remove(card_number));
}

void BonusEntity::clear_cache() {
    temp_balance.clear();
}

void BonusEntity::apply_cache_info() {
    QMap<int, int> ::iterator it = temp_balance.begin();
    for (; it != temp_balance.end(); it++) {
        qDebug() << *it;
        if (!update_balance(it.key(), it.value())) {
            qDebug() << model->lastError();
        }
    }
}

bool BonusEntity::update_balance(const int card_number, const int balance) {
    if (card_number < 10000000) return false;
    QSqlQuery query(db->connectionName());
    query.prepare("UPDATE bonus_cards SET balance = :balance WHERE card_number = :card_number");
    query.bindValue(":balance", balance);
    query.bindValue(":card_number", card_number);
    bool ret = query.exec();
    query.finish();
    return ret;
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
    model->setCurrencyColumn(5);
    model->setTable("sales_history");
    //model->select();
    this->readable_names = fn;
    for (int i = 0; i < model->columnCount(); i++) {
        this->column_names.append(model->headerData(i, Qt::Horizontal).toString());
        model->setHeaderData(i, Qt::Horizontal, readable_names[i]);
    }
    this->meds = me;
    this->bonuscards = be;
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
bool SellEntity::get_med_price(const int id, int &price) const {
    return meds->get_med_price(id, price);
}

bool SellEntity::get_card_balance(const int card_number, int &balance) const {
    return bonuscards->get_card_balance(card_number, balance);
}
