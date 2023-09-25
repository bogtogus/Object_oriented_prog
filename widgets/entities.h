#ifndef ENTITIES_H
#define ENTITIES_H

#include <QtSql>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSharedPointer>
#include <QAbstractItemView>


/*!
 * \brief Модель таблицы Базы Данных с столбцом в денежном формате
 */
class SqlTableModel_Currency : public QSqlTableModel {
Q_OBJECT
private:
    int currency_column;
public:
    SqlTableModel_Currency(QObject* parent = nullptr, QSqlDatabase db = QSqlDatabase(), const int price_column = -1);
    ~SqlTableModel_Currency();
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void setCurrencyColumn(const int column);
};

/*!
 * \brief Базовый класс сущности с основным набором реализованных методов.
 * Работа производится с Базой Данных.
 */
class Entity : public QObject {
    Q_OBJECT
protected:
    // Указатель на БД
    QSharedPointer<QSqlDatabase> db;
    // Модель представления таблицы
    SqlTableModel_Currency* model;
    // Названия столбцов в БД
    QVector<QString> column_names;
    // Заголовки столбцов
    QVector<QString> readable_names;
public:
    Entity(QSharedPointer<QSqlDatabase> odb);
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    Entity(Entity&&);
    Entity& operator=(Entity&&);
    virtual ~Entity();
    QSqlTableModel* get_model() const;
    QSqlRecord get_record(const int id) const;
    int rowCount() const;
    int columnCount() const;
    QString filter() const;
    bool isDirty() const;
    QSqlError lastError() const;
    QVector<QString> get_readable_names() const;
    QVector<QString> get_column_names() const;

    bool addRecord(const QSqlRecord*);
    bool removeRecord(const int id);
    bool removeRecords(const int start, const int length);
    bool setRecord(const int, const QSqlRecord&);
    void setFilter(const QString& filter);
    void revertAll();
    bool submitAll();
    bool select();
};

/*!
 * \brief Сущность Склад лекарств с двумя дополнительными методами
 */
class MedsEntity : public Entity {
    Q_OBJECT
private:
    // Кэш новых значений количества лекарств на складе
    QHash<int, int> temp_amount;
    bool update_amount(const int med_id, const int amount);
public:
    MedsEntity(QSharedPointer<QSqlDatabase> odb,
                const QVector<QString>&);
    ~MedsEntity();
    // Все айди лекарств
    QVector<QString> get_all_ids() const;
    // Получить количество лекарства с заданным id
    bool get_med_amount(const int med_id, int& amount) const;
    bool get_med_price(const int med_id, int& price) const;
    int get_med_amount(const int med_id) const;
    bool is_enough_meds(const int med_id, const int amount) const;
    bool cache_contains(const int med_id) const;

    bool add_temp_sale(const int med_id, const int sold);
    bool remove_temp_sale(const int med_id);
    void clear_cache();
    bool apply_cache_info();
};

/*!
 * \brief Сущность Зарегистрированные в бонусной программе покупатели
 */
class BonusEntity : public Entity {
    Q_OBJECT
private:
    /* Кэш новых значений баланса бонусных карт */
    QHash<int, int> temp_balance;
    /* Кэш потраченной клиентом суммы */
    QHash<int, int> temp_spendings;
    bool update_balance(const int card_number, const int balance);
public:
    BonusEntity(QSharedPointer<QSqlDatabase> odb,
                const QVector<QString>&);
    ~BonusEntity();
    QVector<QString> get_all_cards() const;
    bool get_card_balance(const int card_number, int& amount) const;
    int get_card_balance(const int card_number) const;
    bool is_enough_bonuses(const int card_number, const int amount) const;
    bool cache_contains(const int card_number) const;

    bool add_temp_withdraw(const int card_number, const int points_withdrown, const int money_spent);
    bool remove_temp_withdraw(const int card_number);
    void clear_cache();
    bool apply_cache_info();
};

/*! Сущность История продаж
 *
 */
class SellEntity : public Entity {
    Q_OBJECT
private:
    // Указатели на константные объекты таблиц. Нужны для
    // безопасного получения некоторой информации из
    // соответствующих таблиц
    const MedsEntity* meds;
    const BonusEntity* bonuscards;
public:
    SellEntity(QSharedPointer<QSqlDatabase> odb,
                const QVector<QString>&,
               const MedsEntity*,
               const BonusEntity*);
    ~SellEntity();
    QVector<QString> get_all_cards() const;
    QVector<QString> get_all_ids() const;
    bool get_med_amount(const int id, int& ampunt) const;
    bool get_med_price(const int id, int& price) const;
    bool get_card_balance(const int card_number, int& amount) const;
};


#endif // ENTITIES_H
