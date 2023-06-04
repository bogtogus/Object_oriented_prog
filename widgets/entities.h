#ifndef ENTITIES_H
#define ENTITIES_H

#include <QtSql>
#include <QSqlTableModel>
#include <QSqlDatabase>
#include <QSharedPointer>
#include <QAbstractItemView>

// Базовый класс сущности с основным набором реализованных методов.
// Работа производится с Базой Данных.
class Entity : public QObject {
    Q_OBJECT
protected:
    // Указатель на БД
    QSharedPointer<QSqlDatabase> db;
    // Модель представления таблицы
    QSqlTableModel* model;
    // Названия столбцов
    QVector<QString> field_names;
public:
    Entity(QSharedPointer<QSqlDatabase> odb);
    virtual ~Entity();
    QSqlTableModel* get_model() const;
    QSqlRecord get_record(const int id) const;
    int rowCount() const;
    int columnCount() const;
    QString filter() const;
    // Есть ли неподтверждённые изменения
    bool isDirty() const;
    QSqlError lastError() const;
    QVector<QString> get_fnames() const;

    bool addRecord(const QSqlRecord*);
    bool removeRecord(const int id);
    bool removeRecords(const int start, const int length);
    bool setRecord(const int, const QSqlRecord&);
    void setFilter(const QString& filter);
    void revertAll();
    bool submitAll();
    bool select();
};

// Сущность Склад лекарств с двумя дополнительными методами
class MedsEntity : public Entity {
    Q_OBJECT
public:
    MedsEntity(QSharedPointer<QSqlDatabase> odb,
                const QVector<QString>&);
    ~MedsEntity();
    // Все айди лекарств
    QVector<QString> get_all_ids() const;
    // Получить количество лекарства с заданным id
    bool get_med_amount(const int id, int& amount) const;
    // Обновить количество лекарства на складе
    bool update_record(const int id, const int amount);
};

// Сущность Зарегистрированные в бонусной программе покупатели
class BonusEntity : public Entity {
    Q_OBJECT
public:
    BonusEntity(QSharedPointer<QSqlDatabase> odb,
                const QVector<QString>&);
    ~BonusEntity();
    // получить все номера бонусных карт
    QVector<QString> get_all_cards() const;
};

// Сущность История продаж
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
    bool get_med_amount(const int, int&) const;
};


#endif // ENTITIES_H
