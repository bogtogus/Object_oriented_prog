#ifndef INPUTFIELDS_H
#define INPUTFIELDS_H

#include "achildwin.h"
#include "entities.h"
#include <memory>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QFormLayout>
#include <QLabel>
#include <QFont>
#include <QCheckBox>
#include <QMap>
#include <QRegularExpressionValidator>
#include <QPushButton>
#include <QtSql>
#include <QSpinBox>

namespace Ui {
class inputFields;
}


class inputFields : public aChildWin
{
    Q_OBJECT
protected:
    virtual void exec_clicked() = 0;
    void buildLayout();
    Ui::inputFields* ui;
    // Обработчик полей (реализация)
    QSharedPointer<class Implement> impl;
    // сопоставление "Системное имя столбца"-"Читаемое пользователем название столбца"
    QMap<QString, QString> fields;
    // список системных имён столбцов в порядке,
    // представленном в таблице(не отсортированном)
    QVector<QString> keys;
    // "Читаемое пользователем название столбца"
    QMap<QString, QString> titles;
    QPushButton* exec_button; // кнопка выполнения операции
    QPushButton* back; // кнопка назад
    QFormLayout* flay; // компоновка пар Метка-Поле
    QVBoxLayout* vlay; // компоновка flay с кнопками
public:
    explicit inputFields(QWidget *parent,
                         const QMap<QString, QString>& titles,
                         const QMap<QString, QString>& fields,
                         const QVector<QString>& keys);
    virtual ~inputFields();
    void goback() override;
};

// класс формы. Является абстракцией в паттерне Мост
class MedsAbstr : public inputFields {
    Q_OBJECT
private:
    // Элементы формы
    QLineEdit* manufactorer;
    QLineEdit* name;
    QLineEdit* date_of_manuf;
    QLineEdit* expiry_date;
    QCheckBox* on_prescription;
    QLineEdit* pieces;
    QHBoxLayout* hlay;


    void exec_clicked() override;
    // Формирование построчно нужных полей для ввода
    void produceField1();
    void produceField2();
    void produceField3();
    void produceField4();
    void produceField5();
    void produceField6();
    // добавления кнопки назад и кнопки совершения операции
    void finalization();

    friend class AddMedsImplement;
    friend class FindMedsImplement;
public:
    explicit MedsAbstr(const QMap<QString, QString>& titles,
                        const QMap<QString, QString>& fields,
                        const QVector<QString>& keys,
                        QSharedPointer<Implement> impl,
                        QWidget *parent);
    virtual ~MedsAbstr();
};

class BonusAbstr : public inputFields {
    Q_OBJECT
private:
    // Элементы формы
    QLineEdit* card_number;
    QLineEdit* name;
    QLineEdit* bonus_amount;
    QHBoxLayout* hlay;

    void exec_clicked() override;
    void produceField1();
    void produceField2();
    void produceField3();
    void finalization();

    friend class AddBonusImplement;
    friend class FindBonusImplement;
public:
    explicit BonusAbstr(const QMap<QString, QString>& titles,
                        const QMap<QString, QString>& fields,
                        const QVector<QString>& keys,
                        QSharedPointer<Implement> impl,
                        QWidget *parent = nullptr);
    virtual ~BonusAbstr();
};

class SellAbstr : public inputFields {
    Q_OBJECT
private:
    // Элементы формы
    QLineEdit* date_of_buy;
    QComboBox* customer;
    QComboBox* medicines;
    QSpinBox* pieces;
    QHBoxLayout* hlay;
    const SellEntity* entity;

    void exec_clicked() override;
    void produceField1();
    void produceField2();
    void produceField3();
    void produceField4();
    void finalization();
    void med_changed(int);

    friend class AddSellImplement;
    friend class FindSellImplement;
public:
    explicit SellAbstr(const QMap<QString, QString>& titles,
                        const QMap<QString, QString>& fields,
                        const QVector<QString>& keys,
                        const SellEntity* en,
                        QSharedPointer<Implement> impl,
                        QWidget *parent = nullptr);
    virtual ~SellAbstr();
};

// Интерфейс обработки полученных полей формы.
// Отличие в методе обработки полей для поиска и добавления
// заключается в том, что при добавлении нужно указать все или
// почти все данные, а при поиске минимум одно данное
class Implement : public QObject {
Q_OBJECT
public:
    virtual ~Implement() { qDebug() << "del Impl"; }
    virtual QString processField(inputFields*) = 0;
signals:
    void exec_clicked_signal(QSqlRecord*);
    void exec_clicked_signal(QString&);
};

// конкретный обработчик полей формы(реализация)
class AddMedsImplement : public Implement {
Q_OBJECT
public:
    QString processField(inputFields*) override;

};

// конкретный обработчик полей формы(реализация)
class FindMedsImplement : public Implement {
Q_OBJECT
public:
    QString processField(inputFields*) override;
};

// конкретный обработчик полей формы(реализация)
class AddBonusImplement : public Implement {
Q_OBJECT
public:
    AddBonusImplement() {};
    QString processField(inputFields*) override;
};

// конкретный обработчик полей формы(реализация)
class FindBonusImplement : public Implement {
Q_OBJECT
public:
    FindBonusImplement() {};
    QString processField(inputFields*) override;
};

// конкретный обработчик полей формы(реализация)
class AddSellImplement : public Implement {
Q_OBJECT
public:
    AddSellImplement() {};
    QString processField(inputFields*) override;
};

// конкретный обработчик полей формы(реализация)
class FindSellImplement : public Implement {
Q_OBJECT
public:
    FindSellImplement() {};
    QString processField(inputFields*) override;
};

#endif // INPUTFIELDS_H
