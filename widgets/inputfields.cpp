#include "inputfields.h"
#include "ui_inputfields.h"
#include "medicineswindow.h"
#include <QDebug>
#include <QMessageBox>

inputFields::inputFields(QWidget *parent,
                         const QMap<QString, QString>& titles,
                         const QMap<QString, QString>& fields,
                         const QVector<QString>& keys) :
    aChildWin(parent),
    ui(new Ui::inputFields)
{
    ui->setupUi(this);
    this->setWindowTitle("Добавить");
    this->setGeometry(prnt->geometry());
    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->back, &QPushButton::clicked, this, &inputFields::goback);
    this->titles = titles;
    this->fields = fields;
    this->keys = keys;
    qDebug() << "CRT InputFields";
}

inputFields::~inputFields() {
    titles.clear();
    fields.clear();
    keys.clear();
    qDebug() << "DEL InputFields";
    delete ui;
}

void inputFields::goback() {
    emit goback_signal(this);
}

// Создание объектов компоновки для полей и для кнопок
// а также создание заголовка окна
void inputFields::buildLayout() {
    vlay = new QVBoxLayout(this);
    flay = new QFormLayout();
    flay->setObjectName("form_layout");
    flay->setVerticalSpacing(7);
    flay->setHorizontalSpacing(7);
    QLabel* title = new QLabel(titles.value("title", "Undefined"), this);
    title->setStyleSheet("QLabel { font: italic; font-size: 20px; }");
    title->setMaximumHeight(30);
    title->setAlignment(Qt::AlignCenter);
    //vlay->addWidget(title, Qt::AlignCenter);
    flay->addRow(title);
}



MedsAbstr::MedsAbstr(const QMap<QString, QString>& titles,
                       const QMap<QString, QString>& fields,
                       const QVector<QString>& keys,
                       QSharedPointer<Implement> impl,
                       QWidget* parent) : inputFields(parent, titles, fields, keys) {
    this->impl = impl;
    row = -1;
    buildLayout();
    produceField1();
    produceField2();
    produceField3();
    produceField4();
    produceField5();
    produceField6();
    produceField7();
    finalization();
    connect(exec_button, &QPushButton::clicked, this, &MedsAbstr::exec_clicked);
}

MedsAbstr::~MedsAbstr() {
    qDebug() << "DEL MedsAbstr";
}

// нажание на кнопку выполнения операции
void MedsAbstr::exec_clicked() {
    // если нет ошибок, то объект реализации внутри
    // себя вызвал сигнал с нужным объектом
    QString error = impl->processFields(this);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Ошибка!", error);
    }
}

// построение полей ввода данных
void MedsAbstr::produceField1() {
    QLabel* label_1 = new QLabel(fields[keys[1]], this);
    label_1->setObjectName("label_1");
    label_1->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_1);
    manufactorer = new QLineEdit("", this);
    manufactorer->setObjectName(keys[1]);
    manufactorer->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    manufactorer->setMinimumHeight(30);
    flay->addRow(label_1, manufactorer);
    label_1 = nullptr;
}

void MedsAbstr::produceField2() {
    QLabel* label_2 = new QLabel(fields[keys[2]], this);
    label_2->setObjectName("label_2");
    label_2->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_2);
    name = new QLineEdit("", this);
    name->setObjectName(keys[2]);
    name->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    name->setMinimumHeight(30);
    flay->addRow(label_2, name);
    label_2 = nullptr;

}

void MedsAbstr::produceField3() {
    QLabel* label_3 = new QLabel(fields[keys[3]], this);
    label_3->setObjectName("label_3");
    label_3->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_3);
    date_of_manuf = new QLineEdit("", this);
    date_of_manuf->setObjectName(keys[3]);
    date_of_manuf->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    QRegularExpression regexp("\\d\\d-\\d\\d-\\d{4}");
    QValidator* valid = new QRegularExpressionValidator(regexp, this);
    date_of_manuf->setValidator(valid);
    date_of_manuf->setMinimumHeight(30);
    flay->addRow(label_3, date_of_manuf);
    label_3 = nullptr;
}

void MedsAbstr::produceField4() {
    QLabel* label_4 = new QLabel(fields[keys[4]], this);
    label_4->setObjectName("label_4");
    label_4->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_4);
    expiry_date = new QLineEdit("", this);
    expiry_date->setObjectName(keys[4]);
    expiry_date->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    QRegularExpression regexp("\\d\\d-\\d\\d-\\d{4}");
    QValidator* valid = new QRegularExpressionValidator(regexp, this);
    expiry_date->setValidator(valid);
    expiry_date->setMinimumHeight(30);
    flay->addRow(label_4, expiry_date);
    label_4 = nullptr;
}

void MedsAbstr::produceField5() {
    on_prescription = new QCheckBox(fields[keys[5]], this);
    on_prescription->setObjectName(keys[5]);
    on_prescription->setStyleSheet("QCheckBox { font: regular 12px; }"
                           "QCheckBox::indicator {"
                           "width: 16px;"
                           "height: 16px;"
                           "}");
    on_prescription->setMinimumHeight(30);
    flay->addRow(new QLabel("", this), on_prescription);
    //on_prescription = nullptr;
}

void MedsAbstr::produceField6() {
    QLabel* label_6 = new QLabel(fields[keys[6]], this);
    label_6->setObjectName("label_6");
    label_6->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_6);
    pieces = new QLineEdit("", this);
    pieces->setObjectName(keys[6]);
    pieces->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    QRegularExpression regexp("[+]?\\d{1,4}");
    QValidator* valid = new QRegularExpressionValidator(regexp, this);
    pieces->setValidator(valid);
    pieces->setMinimumHeight(30);
    flay->addRow(label_6, pieces);
    label_6 = nullptr;
}

void MedsAbstr::produceField7() {
    QLabel* label_7 = new QLabel(fields[keys[7]], this);
    label_7->setObjectName("label_7");
    label_7->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_7);
    price = new QLineEdit("", this);
    price->setObjectName(keys[7]);
    price->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    QRegularExpression regexp("[+]?\\d{1,5}([.,]\\d{1,2})?");
    QValidator* valid = new QRegularExpressionValidator(regexp, this);
    price->setValidator(valid);
    price->setMinimumHeight(30);
    flay->addRow(label_7, price);
    label_7 = nullptr;
}

void MedsAbstr::finalization() {
    vlay->setSpacing(7);
    vlay->setContentsMargins(20, 20, 20 ,20);
    //vlay->addWidget(this);
    vlay->addItem(flay);
    vlay->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    hlay = new QHBoxLayout();
    hlay->addWidget(ui->back);
    exec_button = new QPushButton(titles.value("exec", "Undefined"), this);
    exec_button->setObjectName("exec_button");
    exec_button->setStyleSheet("QPushButton {font: regular 12px; }");
    exec_button->setMaximumHeight(ui->back->maximumHeight());
    exec_button->setMaximumWidth(200);
    exec_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    hlay->addWidget(ui->back, 0, Qt::Alignment(Qt::AlignLeft));
    hlay->addWidget(exec_button, 0, Qt::Alignment(Qt::AlignRight));
    hlay->setSizeConstraint(QHBoxLayout::SetMaximumSize);
    vlay->addItem(hlay);
}

void MedsAbstr::fill_fields(const QSqlRecord & record, const int row) {
    manufactorer->setText(record.field(keys[1]).value().toString());
    name->setText(record.field(keys[2]).value().toString());
    date_of_manuf->setText(record.field(keys[3]).value().toString());
    expiry_date->setText(record.field(keys[4]).value().toString());
    on_prescription->setChecked(record.field(keys[5]).value().toBool());
    pieces->setText(record.field(keys[6]).value().toString());
    price->setText(record.field(keys[7]).value().toString());
    this->row = row;
}

// Реализация для окна ввода данных для добавления в таблицу
QString AddMedsImplement::processFields(inputFields* abs) {
    if (abs == nullptr) return "Undefined";
    MedsAbstr* concreteAbs = qobject_cast<MedsAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
    if (concreteAbs->manufactorer->text().length() < 3) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->manufactorer->objectName()] +
                             "\"!";
    }
    else if (concreteAbs->name->text().length() < 3) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->name->objectName()] +
                             "\"!";
    }
    else if (!concreteAbs->date_of_manuf->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->date_of_manuf->objectName()] +
                             "\"!";
    }
    else if (!concreteAbs->expiry_date->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->expiry_date->objectName()] +
                             "\"!";
    }
    else if (concreteAbs->pieces->text().isEmpty() ||
            !concreteAbs->pieces->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->pieces->objectName()] +
                             "\"!";
    }
    else if (concreteAbs->price->text().isEmpty() ||
            !concreteAbs->price->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->price->objectName()] +
                             "\"!";
    }
    else {
        QSqlRecord* rec = new QSqlRecord();
        rec->append(QSqlField(concreteAbs->keys[1]));
        rec->setValue(concreteAbs->keys[1], concreteAbs->manufactorer->text());
        rec->append(QSqlField(concreteAbs->keys[2]));
        rec->setValue(concreteAbs->keys[2], concreteAbs->name->text());
        rec->append(QSqlField(concreteAbs->keys[3]));
        rec->setValue(concreteAbs->keys[3], concreteAbs->date_of_manuf->text());
        rec->append(QSqlField(concreteAbs->keys[4]));
        rec->setValue(concreteAbs->keys[4], concreteAbs->expiry_date->text());
        rec->append(QSqlField(concreteAbs->keys[5]));
        rec->setValue(concreteAbs->keys[5], concreteAbs->on_prescription->isChecked());
        rec->append(QSqlField(concreteAbs->keys[6]));
        rec->setValue(concreteAbs->keys[6], concreteAbs->pieces->text());
        rec->append(QSqlField(concreteAbs->keys[7]));
        rec->setValue(concreteAbs->keys[7], concreteAbs->price->text());
        emit Implement::exec_clicked_signal(rec);
        delete rec;
    }
    return NULL;
}


// Реализация для окна ввода данных для поиска в таблице
QString FindMedsImplement::processFields(inputFields * abs) {
    if (abs == nullptr) return "Undefined";
    MedsAbstr* concreteAbs = qobject_cast<MedsAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
    QString where;
    if (concreteAbs->manufactorer->text().length() >= 3) {
        where += concreteAbs->manufactorer->objectName() +
                " LIKE \"%" + concreteAbs->manufactorer->text() + "%\" AND ";
    }
    if (concreteAbs->name->text().length() >= 3) {
        where += concreteAbs->name->objectName() +
                " LIKE \"%" + concreteAbs->name->text() + "%\" AND ";
    }
    if (concreteAbs->date_of_manuf->hasAcceptableInput()) {
        where += concreteAbs->date_of_manuf->objectName() +
                " = \"" + concreteAbs->date_of_manuf->text() + "\" AND ";
    }
    if (concreteAbs->expiry_date->hasAcceptableInput()) {
        where += concreteAbs->expiry_date->objectName() +
                " = \"" + concreteAbs->expiry_date->text() + "\" AND ";
    }
    if (concreteAbs->on_prescription->isChecked()) {
        where += concreteAbs->on_prescription->objectName() +
                " = 1 AND ";
    }
    if (!concreteAbs->pieces->text().isEmpty() &&
            !concreteAbs->pieces->hasAcceptableInput()) {
        where += concreteAbs->pieces->objectName() +
                " = \"" + concreteAbs->pieces->text() + "\" AND ";
    }
    if (!concreteAbs->price->text().isEmpty() &&
            !concreteAbs->price->hasAcceptableInput()) {
        where += concreteAbs->price->objectName() +
                " = \"" + concreteAbs->price->text() + "\" AND ";
    }
    if (where.isEmpty()) {
        return "Все поля пусты, поиск невозможен!";
    }
    else {
        where.remove(where.lastIndexOf(" AND "), 5);
        emit Implement::exec_clicked_signal(where);
    }
    return NULL;
}

QString EditMedsImplement::processFields(inputFields* abs) {
    if (abs == nullptr) return "Undefined";
    MedsAbstr* concreteAbs = qobject_cast<MedsAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
    if (concreteAbs->manufactorer->text().length() < 3) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->manufactorer->objectName()] +
                             "\"!";
    }
    else if (concreteAbs->name->text().length() < 3) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->name->objectName()] +
                             "\"!";
    }
    else if (!concreteAbs->date_of_manuf->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->date_of_manuf->objectName()] +
                             "\"!";
    }
    else if (!concreteAbs->expiry_date->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->expiry_date->objectName()] +
                             "\"!";
    }
    else if (concreteAbs->pieces->text().isEmpty() ||
            !concreteAbs->pieces->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->pieces->objectName()] +
                             "\"!";
    }
    else if (concreteAbs->price->text().isEmpty() ||
            !concreteAbs->price->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->price->objectName()] +
                             "\"!";
    }
    else {
        QSqlRecord* rec = new QSqlRecord();
        rec->append(QSqlField(concreteAbs->keys[1]));
        rec->setValue(concreteAbs->keys[1], concreteAbs->manufactorer->text());
        rec->append(QSqlField(concreteAbs->keys[2]));
        rec->setValue(concreteAbs->keys[2], concreteAbs->name->text());
        rec->append(QSqlField(concreteAbs->keys[3]));
        rec->setValue(concreteAbs->keys[3], concreteAbs->date_of_manuf->text());
        rec->append(QSqlField(concreteAbs->keys[4]));
        rec->setValue(concreteAbs->keys[4], concreteAbs->expiry_date->text());
        rec->append(QSqlField(concreteAbs->keys[5]));
        rec->setValue(concreteAbs->keys[5], concreteAbs->on_prescription->isChecked());
        rec->append(QSqlField(concreteAbs->keys[6]));
        rec->setValue(concreteAbs->keys[6], concreteAbs->pieces->text());
        rec->append(QSqlField(concreteAbs->keys[7]));
        rec->setValue(concreteAbs->keys[7], concreteAbs->price->text());
        emit Implement::exec_clicked_signal(*rec, concreteAbs->row);
        delete rec;
    }
    return NULL;
}


BonusAbstr::BonusAbstr(const QMap<QString, QString>& titles,
                       const QMap<QString, QString>& fields,
                       const QVector<QString>& keys,
                       QSharedPointer<Implement> impl,
                       QWidget* parent) : inputFields(parent, titles, fields, keys) {
    this->impl = impl;
    buildLayout();
    produceField1();
    produceField2();
    produceField3();
    finalization();
    connect(exec_button, &QPushButton::clicked, this, &BonusAbstr::exec_clicked);
}

BonusAbstr::~BonusAbstr() {
    qDebug() << "DEL BonusAbstr";
    fields.clear();
    keys.clear();
    impl.reset();
    titles.clear();
    impl.reset();
}

void BonusAbstr::exec_clicked() {
    QString error = impl->processFields(this);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Ошибка!", error);
    }
}

// построение полей ввода данных
void BonusAbstr::produceField1() {
    QLabel* label_1 = new QLabel(fields[keys[0]], this);
    label_1->setObjectName("label_1");
    label_1->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_1);
    card_number = new QLineEdit("", this);
    card_number->setObjectName(keys[0]);
    card_number->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    QRegularExpression regexp("\\d{8}");
    QValidator* valid = new QRegularExpressionValidator(regexp, this);
    card_number->setValidator(valid);
    card_number->setMinimumHeight(30);
    flay->addRow(label_1, card_number);
    label_1 = nullptr;
}

void BonusAbstr::produceField2() {
    QLabel* label_2 = new QLabel(fields[keys[1]], this);
    label_2->setObjectName("label_2");
    label_2->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_2);
    name = new QLineEdit("", this);
    name->setObjectName(keys[1]);
    name->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    name->setMinimumHeight(30);
    flay->addRow(label_2, name);
    label_2 = nullptr;

}

void BonusAbstr::produceField3() {
    QLabel* label_3 = new QLabel(fields[keys[2]], this);
    label_3->setObjectName("label_3");
    label_3->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_3);
    bonus_amount = new QLineEdit("", this);
    bonus_amount->setObjectName(keys[2]);
    bonus_amount->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    QRegularExpression regexp("\\d{1,4}");
    QValidator* valid = new QRegularExpressionValidator(regexp, this);
    bonus_amount->setValidator(valid);
    bonus_amount->setMinimumHeight(30);
    flay->addRow(label_3, bonus_amount);
    label_3 = nullptr;
}

void BonusAbstr::finalization() {
    vlay->setSpacing(7);
    vlay->setContentsMargins(20, 20, 20 ,20);
    //vlay->addWidget(this);
    vlay->addItem(flay);
    vlay->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    hlay = new QHBoxLayout();
    hlay->addWidget(ui->back);
    exec_button = new QPushButton(titles.value("exec", "Undefined"), this);
    exec_button->setObjectName("exec_button");
    exec_button->setStyleSheet("QPushButton {font: regular 12px; }");
    exec_button->setMaximumHeight(ui->back->maximumHeight());
    exec_button->setMaximumWidth(200);
    exec_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    hlay->addWidget(ui->back, 0, Qt::Alignment(Qt::AlignLeft));
    hlay->addWidget(exec_button, 0, Qt::Alignment(Qt::AlignRight));
    hlay->setSizeConstraint(QHBoxLayout::SetMaximumSize);
    vlay->addItem(hlay);
}

void BonusAbstr::fill_fields(const QSqlRecord & record, const int row) {
    card_number->setText(record.field(keys[0]).value().toString());
    name->setText(record.field(keys[1]).value().toString());
    bonus_amount->setText(record.field(keys[2]).value().toString());
    this->row = row;
}

// Реализация для окна ввода данных для добавления в таблицу
QString AddBonusImplement::processFields(inputFields* abs) {
    if (abs == nullptr) return "Undefined";
    BonusAbstr* concreteAbs = qobject_cast<BonusAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
    //if (concreteAbs->card_number->text().length() < 3) {
    //    return "Ошибка ввода в поле \"" +
    //                         concreteAbs->fields[concreteAbs->card_number->objectName()] +
    //                         "\"!";
    //}
    else if (concreteAbs->name->text().length() < 3) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->name->objectName()] +
                             "\"!";
    }
    else if (!concreteAbs->bonus_amount->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->bonus_amount->objectName()] +
                             "\"!";
    }
    else {
        QSqlRecord* rec = new QSqlRecord();
        rec->append(QSqlField(concreteAbs->keys[0]));
        if (!concreteAbs->card_number->text().isEmpty()) {
            rec->setValue(concreteAbs->keys[0], concreteAbs->card_number->text());
        }
        rec->append(QSqlField(concreteAbs->keys[1]));
        rec->setValue(concreteAbs->keys[1], concreteAbs->name->text());
        rec->append(QSqlField(concreteAbs->keys[2]));
        rec->setValue(concreteAbs->keys[2], concreteAbs->bonus_amount->text());
        emit Implement::exec_clicked_signal(rec);
        delete rec;
    }
    return NULL;
}


// Реализация для окна ввода данных для поиска в таблице
QString FindBonusImplement::processFields(inputFields * abs) {
    if (abs == nullptr) return "Undefined";
    BonusAbstr* concreteAbs = qobject_cast<BonusAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
    QString where;
    if (concreteAbs->card_number->text().length() >= 3) {
        where += concreteAbs->card_number->objectName() +
                " = \"" + concreteAbs->card_number->text() + "\" AND ";
    }
    if (concreteAbs->name->text().length() >= 3) {
        where += concreteAbs->name->objectName() +
                " LIKE \"%" + concreteAbs->name->text() + "%\" AND ";
    }
    if (concreteAbs->bonus_amount->hasAcceptableInput()) {
        where += concreteAbs->bonus_amount->objectName() +
                " = \"" + concreteAbs->bonus_amount->text() + "\" AND ";
    }
    if (where.isEmpty()) {
        return "Все поля пусты, поиск невозможен!";
    }
    else {
        where.remove(where.lastIndexOf(" AND "), 5);
        emit Implement::exec_clicked_signal(where);
    }
    return NULL;
}

QString EditBonusImplement::processFields(inputFields* abs) {
    if (abs == nullptr) return "Undefined";
    BonusAbstr* concreteAbs = qobject_cast<BonusAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
    if (concreteAbs->card_number->text().length() < 3) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->card_number->objectName()] +
                             "\"!";
    }
    else if (concreteAbs->name->text().length() < 3) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->name->objectName()] +
                             "\"!";
    }
    else if (!concreteAbs->bonus_amount->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->bonus_amount->objectName()] +
                             "\"!";
    }
    else {
        QSqlRecord* rec = new QSqlRecord();
        rec->append(QSqlField(concreteAbs->keys[0]));
        if (!concreteAbs->card_number->text().isEmpty()) {
            rec->setValue(concreteAbs->keys[0], concreteAbs->card_number->text());
        }
        rec->append(QSqlField(concreteAbs->keys[1]));
        rec->setValue(concreteAbs->keys[1], concreteAbs->name->text());
        rec->append(QSqlField(concreteAbs->keys[2]));
        rec->setValue(concreteAbs->keys[2], concreteAbs->bonus_amount->text());
        emit Implement::exec_clicked_signal(*rec, concreteAbs->row);
        delete rec;
    }
    return NULL;
}

SellAbstr::SellAbstr(const QMap<QString, QString>& titles,
                       const QMap<QString, QString>& fields,
                       const QVector<QString>& keys,
                       const SellEntity* en,
                       QSharedPointer<Implement> impl,
                       QWidget* parent) : inputFields(parent, titles, fields, keys) {
    this->impl = impl;
    this->entity = en;
    //qDebug() << card_numbers;
    //qDebug() << medicines_ids;
    buildLayout();
    produceField1();
    produceField2();
    produceField3();
    produceField4();
    produceField5();
    finalization();
    connect(exec_button, &QPushButton::clicked, this, &SellAbstr::exec_clicked);
}

SellAbstr::~SellAbstr() {
    qDebug() << "DEL BonusAbstr";
    fields.clear();
    keys.clear();
    impl.reset();
    titles.clear();
    impl.reset();
}

void SellAbstr::exec_clicked() {
    QString error = impl->processFields(this);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Ошибка!", error);
    }
}

void SellAbstr::produceField1() {
    QLabel* label_1 = new QLabel(fields[keys[1]], this);
    label_1->setObjectName("label_1");
    label_1->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_1);
    date_of_buy = new QLineEdit("", this);
    date_of_buy->setObjectName(keys[1]);
    date_of_buy->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    QRegularExpression regexp("\\d\\d-\\d\\d-\\d{4}");
    QValidator* valid = new QRegularExpressionValidator(regexp, this);
    date_of_buy->setValidator(valid);
    date_of_buy->setMinimumHeight(30);
    flay->addRow(label_1, date_of_buy);
}

void SellAbstr::produceField2() {
    QLabel* label_2 = new QLabel(fields[keys[2]], this);
    label_2->setObjectName("label_2");
    label_2->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_2);
    customer = new QComboBox(this);
    customer->setObjectName(keys[2]);
    customer->setStyleSheet("QComboBox { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    customer->setMinimumHeight(30);
    customer->addItem("");
    //qDebug() << "R  " << card_numbers;
    //qDebug() << medicines_ids;
    QVector<QString> card_numbers = entity->get_all_cards();
    QVector<QString>::iterator it = card_numbers.begin();
    for (; it != card_numbers.end(); it++) {
        customer->addItem(*it);
    }
    flay->addRow(label_2, customer);
    label_2 = nullptr;

}

void SellAbstr::produceField3() {
    QLabel* label_3 = new QLabel(fields[keys[3]], this);
    label_3->setObjectName("label_3");
    label_3->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_3);
    medicines = new QComboBox(this);
    medicines->setObjectName(keys[3]);
    medicines->setStyleSheet("QComboBox { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    medicines->addItem("");
    QVector<QString> medicines_ids = entity->get_all_ids();
    QVector<QString>::iterator it = medicines_ids.begin();
    for (; it != medicines_ids.end(); it++) {
        medicines->addItem(*it);
    }
    medicines->setMinimumHeight(30);
    connect(medicines, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SellAbstr::med_changed);
    flay->addRow(label_3, medicines);
    label_3 = nullptr;
}

void SellAbstr::produceField4() {
    QLabel* label_4 = new QLabel(fields[keys[4]], this);
    label_4->setObjectName("label_4");
    label_4->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    //labels.push_back(label_3);
    pieces = new QSpinBox(this);
    pieces->setObjectName(keys[4]);
    pieces->setStyleSheet("QSpinBox { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    pieces->setMinimumHeight(30);
    pieces->setMaximumWidth(50);
    pieces->setMinimum(1);
    flay->addRow(label_4, pieces);
    label_4 = nullptr;
}

void SellAbstr::produceField5() {
    QLabel* label_5 = new QLabel(fields[keys[5]], this);
    label_5->setObjectName("label_5");
    label_5->setStyleSheet("QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    earnings = new QLineEdit("", this);
    earnings->setObjectName(keys[5]);
    earnings->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    earnings->setMinimumHeight(30);
    QRegularExpression regexp("[+]?\\d{1,20}([.,]\\d{1,2})?");
    QValidator* valid = new QRegularExpressionValidator(regexp, this);
    earnings->setValidator(valid);
    flay->addRow(label_5, earnings);
    label_5 = nullptr;
}

void SellAbstr::finalization() {
    vlay->setSpacing(7);
    vlay->setContentsMargins(20, 20, 20 ,20);
    //vlay->addWidget(this);
    vlay->addItem(flay);
    vlay->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
    hlay = new QHBoxLayout();
    hlay->addWidget(ui->back);
    exec_button = new QPushButton(titles.value("exec", "Undefined"), this);
    exec_button->setObjectName("exec_button");
    exec_button->setStyleSheet("QPushButton {font-style: regular; font-size: 18px; }");
    exec_button->setMaximumHeight(ui->back->maximumHeight());
    exec_button->setMaximumWidth(200);
    exec_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    hlay->addWidget(ui->back, 0, Qt::Alignment(Qt::AlignLeft));
    hlay->addWidget(exec_button, 0, Qt::Alignment(Qt::AlignRight));
    hlay->setSizeConstraint(QHBoxLayout::SetMaximumSize);
    vlay->addItem(hlay);
}

void SellAbstr::med_changed(int index) {
    if (index == 0) {
        pieces->setEnabled(false);
        return;
    }
    pieces->setEnabled(true);
    int amount = 0;
    if (!entity->get_med_amount(index, amount) || amount == 0) {
        pieces->setMinimum(0);
    }
    pieces->setMaximum(amount);
}

void SellAbstr::fill_fields(const QSqlRecord & record, const int row) {
    date_of_buy->setText(record.field(keys[1]).value().toString());
    customer->setCurrentText(record.field(keys[2]).value().toString());
    medicines->setCurrentText(record.field(keys[3]).value().toString());
    pieces->setValue(record.field(keys[4]).value().toLongLong());
    earnings->setText(record.field(keys[5]).value().toString());
    this->row = row;
}

// Реализация для окна ввода данных для добавления в таблицу
QString AddSellImplement::processFields(inputFields* abs) {
    if (abs == nullptr) return "Undefined";
    SellAbstr* concreteAbs = qobject_cast<SellAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
    if (!concreteAbs->date_of_buy->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->date_of_buy->objectName()] +
                             "\"!";
    }
    //else if (!concreteAbs->customer->hasAcceptableInput()) {
    //    return "Ошибка ввода в поле \"" +
    //                         concreteAbs->fields[concreteAbs->customer->objectName()] +
    //                         "\"!";
    //}
    else if (concreteAbs->medicines->currentText().isEmpty()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->medicines->objectName()] +
                             "\"!";
    }
    else if (concreteAbs->pieces->text().isEmpty() ||
             !concreteAbs->pieces->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->pieces->objectName()] +
                             "\"!";
    }
    else if (concreteAbs->pieces->value() == 0) {
        return "Лекарства с идентификационным номером " + concreteAbs->medicines->currentText()
                + " не осталось на складе!";
    }
    else if (concreteAbs->earnings->text().isEmpty() ||
             !concreteAbs->earnings->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->earnings->objectName()] +
                             "\"!";
    }
    else {
        QSqlRecord* rec = new QSqlRecord();
        rec->append(QSqlField(concreteAbs->keys[1]));
        rec->setValue(concreteAbs->keys[1], concreteAbs->date_of_buy->text());
        rec->append(QSqlField(concreteAbs->keys[2]));
        rec->setValue(concreteAbs->keys[2], concreteAbs->customer->currentText());
        rec->append(QSqlField(concreteAbs->keys[3]));
        rec->setValue(concreteAbs->keys[3], concreteAbs->medicines->currentText());
        rec->append(QSqlField(concreteAbs->keys[4]));
        rec->setValue(concreteAbs->keys[4], concreteAbs->pieces->text());
        rec->append(QSqlField(concreteAbs->keys[5]));
        rec->setValue(concreteAbs->keys[5], concreteAbs->earnings->text());
        emit Implement::exec_clicked_signal(rec);
        delete rec;
    }
    return NULL;
}


// Реализация для окна ввода данных для поиска в таблице
QString FindSellImplement::processFields(inputFields * abs) {
    if (abs == nullptr) return "Undefined";
    SellAbstr* concreteAbs = qobject_cast<SellAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
    QString where;
    if (concreteAbs->date_of_buy->hasAcceptableInput()) {
        where += concreteAbs->date_of_buy->objectName() +
                " = \"" + concreteAbs->date_of_buy->text() +
                "\" AND ";
    }
    if (concreteAbs->customer->currentText().length() == 8) {
        where += concreteAbs->customer->objectName() +
                " = \"" + concreteAbs->customer->currentText() +
                "\" AND ";
    }
    if (!concreteAbs->medicines->currentText().isEmpty()) {
        where += concreteAbs->medicines->objectName() +
                " = \"" + concreteAbs->medicines->currentText() +
                "\" AND ";
    }
    if (where.isEmpty()) {
        return "Все поля пусты или некорректно введены, поиск невозможен!";
    }
    else {
        where.remove(where.lastIndexOf(" AND "), 5);
        emit Implement::exec_clicked_signal(where);
    }
    return NULL;
}

QString EditSellImplement::processFields(inputFields * abs) {
    if (abs == nullptr) return "Undefined";
    SellAbstr* concreteAbs = qobject_cast<SellAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
    if (!concreteAbs->date_of_buy->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->date_of_buy->objectName()] +
                             "\"!";
    }
    //else if (concreteAbs->customer->currentText().isEmpty()) {
    //    return "Ошибка ввода в поле \"" +
    //                         concreteAbs->fields[concreteAbs->customer->objectName()] +
    //                         "\"!";
    //}
    else if (concreteAbs->medicines->currentText().isEmpty()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->medicines->objectName()] +
                             "\"!";
    }
    else if (concreteAbs->pieces->text().isEmpty() ||
             !concreteAbs->pieces->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->pieces->objectName()] +
                             "\"!";
    }
    else if (concreteAbs->pieces->value() == 0) {
        return "Лекарства с идентификационным номером " + concreteAbs->medicines->currentText()
                + " не осталось на складе!";
    }
    else if (concreteAbs->earnings->text().isEmpty() ||
             !concreteAbs->earnings->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->earnings->objectName()] +
                             "\"!";
    }
    else {
        QSqlRecord* rec = new QSqlRecord();
        rec->append(QSqlField(concreteAbs->keys[1]));
        rec->setValue(concreteAbs->keys[1], concreteAbs->date_of_buy->text());
        rec->append(QSqlField(concreteAbs->keys[2]));
        rec->setValue(concreteAbs->keys[2], concreteAbs->customer->currentText());
        rec->append(QSqlField(concreteAbs->keys[3]));
        rec->setValue(concreteAbs->keys[3], concreteAbs->medicines->currentText());
        rec->append(QSqlField(concreteAbs->keys[4]));
        rec->setValue(concreteAbs->keys[4], concreteAbs->pieces->text());
        emit Implement::exec_clicked_signal(*rec, concreteAbs->row);
        delete rec;
    }
    return NULL;
}

