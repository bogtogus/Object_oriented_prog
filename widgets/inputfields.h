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
#include <QCalendarWidget>

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
    // Заголовок формы и название кнопки действия
    QMap<QString, QString> titles;
    QPushButton* exec_button; // кнопка выполнения операции
    QPushButton* back; // кнопка назад
    QFormLayout* flay; // компоновка пар Метка-Поле
    QVBoxLayout* vlay; // компоновка flay с кнопками
    bool search_mode;
public:
    explicit inputFields(QWidget *parent,
                         const QMap<QString, QString>& titles,
                         const QMap<QString, QString>& fields,
                         const QVector<QString>& keys,
                         bool search_mode=false);
    virtual ~inputFields();
    virtual void fill_fields(const QSqlRecord&, const int) = 0;
    void goback() override;
};

// класс формы. Является абстракцией в паттерне Мост
class MedsAbstr : public inputFields {
    Q_OBJECT
private:
    // Элементы формы
    QLineEdit* manufactorer;
    QLineEdit* name;
    QCalendarWidget* date_of_manuf;
    QCalendarWidget* expiry_date;
    QCheckBox* on_prescription;
    QLineEdit* pieces;
    QLineEdit* price;
    QHBoxLayout* hlay;

    QWidget* date_of_manuf_group;
    QCheckBox* date_of_manuf_switch;
    QWidget* expiry_date_group;
    QCheckBox* expiry_date_switch;

    // номер изменяемой строки при соответствующей операции
    int row;
    void exec_clicked() override;
    // Формирование построчно нужных полей для ввода
    void produceField1();
    void produceField2();
    void produceField3();
    void produceField4();
    void produceField5();
    void produceField6();
    void produceField7();
    // добавления кнопки назад и кнопки совершения операции
    void finalization();
    void fill_fields(const QSqlRecord&, const int) override;

    friend class AddMedsImplement;
    friend class FindMedsImplement;
    friend class EditMedsImplement;
public:
    explicit MedsAbstr(const QMap<QString, QString>& titles,
                        const QMap<QString, QString>& fields,
                        const QVector<QString>& keys,
                        QSharedPointer<Implement> impl,
                        QWidget *parent,
                        bool search_mode=false);
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

    int row;
    void exec_clicked() override;
    void produceField1();
    void produceField2();
    void produceField3();
    void finalization();
    void fill_fields(const QSqlRecord&, const int) override;

    friend class AddBonusImplement;
    friend class FindBonusImplement;
    friend class EditBonusImplement;
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
    QCalendarWidget* date_of_sale;
    QComboBox* bonus_card_num;
    QComboBox* medicines;
    QSpinBox* pieces;
    QLineEdit* earnings = nullptr;

    QWidget* bonuswidget;
    QCheckBox* withdraw_all_bonuses;
    QLineEdit* bonuses_to_withdraw;
    QHBoxLayout* bonuslay;
    QHBoxLayout* hlay;
    const SellEntity* entity;
    int basic_price;
    int current_unit_price;
    bool edit_mode;
    QWidget* date_of_sale_group;
    QCheckBox* date_of_sale_switch;

    int row;
    void exec_clicked() override;
    void produceField1();
    void produceField2();
    void produceField3();
    void produceField4();
    void produceField5();
    void produceField6();
    void finalization();
    void med_changed(int combobox_index);
    void med_amount_changed(int amount);
    void card_changed(int combobox_index);
    void withdraw_all_state(int state);
    void bonus_withdraw_edited(const QString&);
    void fill_fields(const QSqlRecord&, const int) override;
    void record_edit_enabled(const bool enabled);
    void recalc_earnings();

    friend class AddSellImplement;
    friend class FindSellImplement;
    friend class EditSellImplement;
public:
    explicit SellAbstr(const QMap<QString, QString>& titles,
                        const QMap<QString, QString>& fields,
                        const QVector<QString>& keys,
                        const SellEntity* en,
                        QSharedPointer<Implement> impl,
                        QWidget *parent,
                        bool searching=false);
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
    virtual QString processFields(inputFields*) = 0;
signals:
    void exec_clicked_signal(const QSqlRecord*);
    void exec_clicked_signal(const QString&);
    void exec_clicked_signal(const QSqlRecord&, const int);
};

// конкретный обработчик полей формы(реализация)
class AddMedsImplement : public Implement {
Q_OBJECT
public:
    QString processFields(inputFields*) override;

};

// конкретный обработчик полей формы(реализация)
class FindMedsImplement : public Implement {
Q_OBJECT
public:
    QString processFields(inputFields*) override;
};

// конкретный обработчик полей формы(реализация)
class EditMedsImplement : public Implement {
Q_OBJECT
public:
    QString processFields(inputFields*) override;
};

// конкретный обработчик полей формы(реализация)
class AddBonusImplement : public Implement {
Q_OBJECT
public:
    AddBonusImplement() {};
    QString processFields(inputFields*) override;
};

// конкретный обработчик полей формы(реализация)
class FindBonusImplement : public Implement {
Q_OBJECT
public:
    FindBonusImplement() {};
    QString processFields(inputFields*) override;
};

// конкретный обработчик полей формы(реализация)
class EditBonusImplement : public Implement {
Q_OBJECT
public:
    QString processFields(inputFields*) override;
};

// конкретный обработчик полей формы(реализация)
class AddSellImplement : public Implement {
Q_OBJECT
public:
    AddSellImplement() {};
    QString processFields(inputFields*) override;
};

// конкретный обработчик полей формы(реализация)
class FindSellImplement : public Implement {
Q_OBJECT
public:
    FindSellImplement() {};
    QString processFields(inputFields*) override;
};

// конкретный обработчик полей формы(реализация)
class EditSellImplement : public Implement {
Q_OBJECT
public:
    QString processFields(inputFields*) override;
};

#endif // INPUTFIELDS_H
