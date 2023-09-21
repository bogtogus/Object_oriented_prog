#include "inputfields.h"
#include "ui_inputfields.h"
#include "medicineswindow.h"
#include <QDebug>
#include <QMessageBox>

#define LABEL_STYLE "QLabel { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }"


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

/*!
 * \brief Метод излучения согнала о переходе назад.
 * \details Сигнал получает основное окно.
 */
void inputFields::goback() {
    emit goback_signal(this);
}

/*!
 * \brief Создание объектов компоновки для полей и для кнопок
 * а также создание заголовка окна
 */
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


/*!
 * \brief Конструктор формы для лекарств.
 * \param titles - читаемые пользователем названия столбцов.
 * \param fields - сопоставление "Имя столбца в БД"-"Читаемое пользователем название столбца".
 * \param keys - список имён столбцов в БД в порядке,
 * представленном в таблице(не отсортированном).
 * \param impl - объект-обработчик полей формы, определяемый в соответствии с задачей
 * (добавление, изменение или поиск).
 * \param parent - родительский объект.
 *
 * \details В конструкторе создаются и размещаются в окне все элементы интерфейса.
 */
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

/*!
 * \brief Метод, выполняемый при нажании на кнопку выполнения операции.
 */
void MedsAbstr::exec_clicked() {
    // если нет ошибок, то объект реализации внутри
    // себя вызвал сигнал с нужным объектом
    QString error = impl->processFields(this);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Ошибка!", error);
    }
}

/*!
 * \brief Построение поля ввода имени производителя.
 */
void MedsAbstr::produceField1() {
    QLabel* label_1 = new QLabel(fields[keys[1]], this);
    label_1->setObjectName("label_1");
    label_1->setStyleSheet(LABEL_STYLE);
    //labels.push_back(label_1);
    manufactorer = new QLineEdit("", this);
    manufactorer->setObjectName(keys[1]);
    manufactorer->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    manufactorer->setMinimumHeight(30);
    flay->addRow(label_1, manufactorer);
    label_1 = nullptr;
}

/*!
 * \brief Построение поля ввода названия лекарства.
 */
void MedsAbstr::produceField2() {
    QLabel* label_2 = new QLabel(fields[keys[2]], this);
    label_2->setObjectName("label_2");
    label_2->setStyleSheet(LABEL_STYLE);
    //labels.push_back(label_2);
    name = new QLineEdit("", this);
    name->setObjectName(keys[2]);
    name->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    name->setMinimumHeight(30);
    flay->addRow(label_2, name);
    label_2 = nullptr;

}

/*!
 * \brief Построение поля ввода даты выпуска лекарства.
 */
void MedsAbstr::produceField3() {
    QLabel* label_3 = new QLabel(fields[keys[3]], this);
    label_3->setObjectName("label_3");
    label_3->setStyleSheet(LABEL_STYLE);
    //labels.push_back(label_3);
    date_of_manuf = new QCalendarWidget(this);
    date_of_manuf->setObjectName(keys[3]);
    date_of_manuf->setMinimumHeight(30);
    date_of_manuf->setMaximumWidth(300);
    date_of_manuf->setMinimumDate(QDate(2000, 1, 1));
    flay->addRow(label_3, date_of_manuf);
    label_3 = nullptr;
}

/*!
 * \brief Построение поля ввода даты окончания срока годности лекарства.
 */
void MedsAbstr::produceField4() {
    QLabel* label_4 = new QLabel(fields[keys[4]], this);
    label_4->setObjectName("label_4");
    label_4->setStyleSheet(LABEL_STYLE);
    //labels.push_back(label_4);
    expiry_date = new QCalendarWidget(this);
    expiry_date->setObjectName(keys[4]);;
    expiry_date->setMinimumHeight(30);
    expiry_date->setMaximumWidth(300);
    expiry_date->setMinimumDate(QDate(2000, 1, 1));
    flay->addRow(label_4, expiry_date);
    label_4 = nullptr;
}

/*!
 * \brief Построение поля ввода типа лекарства (по рецепту или без него).
 */
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

/*!
 * \brief Построение поля ввода количества лекартсва.
 */
void MedsAbstr::produceField6() {
    QLabel* label_6 = new QLabel(fields[keys[6]], this);
    label_6->setObjectName("label_6");
    label_6->setStyleSheet(LABEL_STYLE);
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

/*!
 * \brief Построение поля ввода цены лекарства.
 */
void MedsAbstr::produceField7() {
    QLabel* label_7 = new QLabel(fields[keys[7]], this);
    label_7->setObjectName("label_7");
    label_7->setStyleSheet(LABEL_STYLE);
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

/*!
 * \brief Завершение построения интерфейса формы.
 * \details Поля ввода и кнопки действий помещаются в вертикальный компоновщик.
 */
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

/*!
 * \brief Заполнение полей, если форма предназначена для редактирования.
 * \param record - объект записи.
 * \param row - номер строки  в модели таблицы.
 */
void MedsAbstr::fill_fields(const QSqlRecord & record, const int row) {
    manufactorer->setText(record.field(keys[1]).value().toString());
    name->setText(record.field(keys[2]).value().toString());
    QDate* date = new QDate(QDate::fromString(record.field(keys[3]).value().toString(), "dd-MM-yyyy"));
    if (date && date->isValid()) {
        date_of_manuf->setSelectedDate(*date);
    } else {
        date_of_manuf->setSelectedDate(QDate::currentDate());
    }
    delete date;
    date = new QDate(QDate::fromString(record.field(keys[4]).value().toString(), "dd-MM-yyyy"));
    if (date && date->isValid()) {
        expiry_date->setSelectedDate(*date);
    } else {
        expiry_date->setSelectedDate(QDate::currentDate());
    }
    delete date;
    on_prescription->setChecked(record.field(keys[5]).value().toBool());
    pieces->setText(record.field(keys[6]).value().toString());
    QString val = record.field(keys[7]).value().toString();
    int length = val.length();
    if (length > 2) {
        val.insert(length - 2, '.');
    }
    else {
        val.push_front("0.");
    }
    price->setText(val);
    this->row = row;
}

/*!
 * \brief Реализация для окна ввода данных для добавления в таблицу.
 * \param abs - объект окна формы с введёнными данными.
 * \return текст ошибки.
 */
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
    else if (concreteAbs->date_of_manuf->selectedDate().daysTo(concreteAbs->expiry_date->selectedDate()) <= 0) {
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
        rec->setValue(concreteAbs->keys[3], concreteAbs->date_of_manuf->selectedDate().toString("dd-MM-yyyy"));
        rec->append(QSqlField(concreteAbs->keys[4]));
        rec->setValue(concreteAbs->keys[4], concreteAbs->expiry_date->selectedDate().toString("dd-MM-yyyy"));
        rec->append(QSqlField(concreteAbs->keys[5]));
        rec->setValue(concreteAbs->keys[5], (int)concreteAbs->on_prescription->isChecked());
        rec->append(QSqlField(concreteAbs->keys[6]));
        rec->setValue(concreteAbs->keys[6], concreteAbs->pieces->text());
        rec->append(QSqlField(concreteAbs->keys[7]));
        int val = concreteAbs->price->text().remove('.').remove(',').toInt();
        rec->setValue(concreteAbs->keys[7], val);
        emit Implement::exec_clicked_signal(rec);
        delete rec;
    }
    return NULL;
}


/*!
 * \brief Реализация для окна ввода данных для поиска в таблице.
 * \param abs - объект окна формы с введёнными данными.
 * \return текст ошибки.
 * \return
 */
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
    if (concreteAbs->date_of_manuf->selectedDate().isValid()) {
        where += concreteAbs->date_of_manuf->objectName() +
                " = \"" + concreteAbs->date_of_manuf->selectedDate().toString("dd-MM-yyyy") + "\" AND ";
    }
    if (concreteAbs->expiry_date->selectedDate().isValid()) {
        where += concreteAbs->expiry_date->objectName() +
                " = \"" + concreteAbs->expiry_date->selectedDate().toString("dd-MM-yyyy") + "\" AND ";
    }
    if (concreteAbs->on_prescription->isChecked()) {
        where += concreteAbs->on_prescription->objectName() +
                " = 1 AND ";
    }
    if (!concreteAbs->pieces->text().isEmpty() &&
            concreteAbs->pieces->hasAcceptableInput()) {
        where += concreteAbs->pieces->objectName() +
                " = \"" + concreteAbs->pieces->text() + "\" AND ";
    }
    qDebug() << concreteAbs->price->hasAcceptableInput();
    if (!concreteAbs->price->text().isEmpty() &&
            concreteAbs->price->hasAcceptableInput()) {
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

/*!
 * \brief Реализация для окна ввода данных для изменения записи в таблице.
 * \param abs - объект окна формы с введёнными данными.
 * \return текст ошибки.
 */
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
    else if (concreteAbs->date_of_manuf->selectedDate().daysTo(concreteAbs->expiry_date->selectedDate()) <= 0) {
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
        rec->setValue(concreteAbs->keys[3], concreteAbs->date_of_manuf->selectedDate().toString("dd-MM-yyyy"));
        rec->append(QSqlField(concreteAbs->keys[4]));
        rec->setValue(concreteAbs->keys[4], concreteAbs->expiry_date->selectedDate().toString("dd-MM-yyyy"));
        rec->append(QSqlField(concreteAbs->keys[5]));
        rec->setValue(concreteAbs->keys[5], (int)concreteAbs->on_prescription->isChecked());
        rec->append(QSqlField(concreteAbs->keys[6]));
        rec->setValue(concreteAbs->keys[6], concreteAbs->pieces->text());
        rec->append(QSqlField(concreteAbs->keys[7]));
        int val = concreteAbs->price->text().remove('.').remove(',').toInt();
        rec->setValue(concreteAbs->keys[7], val);
        emit Implement::exec_clicked_signal(*rec, concreteAbs->row);
        delete rec;
    }
    return NULL;
}


/*!
 * \brief Конструктор формы для бонусных карт.
 * \param titles - читаемые пользователем названия столбцов.
 * \param fields - сопоставление "Имя столбца в БД"-"Читаемое пользователем название столбца".
 * \param keys - список имён столбцов в БД в порядке,
 * представленном в таблице(не отсортированном).
 * \param impl - объект-обработчик полей формы, определяемый в соответствии с задачей
 * (добавление, изменение или поиск).
 * \param parent - родительский объект.
 *
 * \details В конструкторе создаются и размещаются в окне все элементы интерфейса.
 */
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

/*!
 * \brief Метод, выполняемый при нажании на кнопку выполнения операции.
 */
void BonusAbstr::exec_clicked() {
    QString error = impl->processFields(this);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Ошибка!", error);
    }
}

/*!
 * \brief Построение поля ввода номера бонусной карты.
 */
void BonusAbstr::produceField1() {
    QLabel* label_1 = new QLabel(fields[keys[0]], this);
    label_1->setObjectName("label_1");
    label_1->setStyleSheet(LABEL_STYLE);
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

/*!
 * \brief Построение поля ввода имени клиента с бонусной картой.
 */
void BonusAbstr::produceField2() {
    QLabel* label_2 = new QLabel(fields[keys[1]], this);
    label_2->setObjectName("label_2");
    label_2->setStyleSheet(LABEL_STYLE);
    //labels.push_back(label_2);
    name = new QLineEdit("", this);
    name->setObjectName(keys[1]);
    name->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    name->setMinimumHeight(30);
    flay->addRow(label_2, name);
    label_2 = nullptr;

}

/*!
 * \brief Построение поля ввода колиечтсва бонусов на карте.
 */
void BonusAbstr::produceField3() {
    QLabel* label_3 = new QLabel(fields[keys[2]], this);
    label_3->setObjectName("label_3");
    label_3->setStyleSheet(LABEL_STYLE);
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

/*!
 * \brief Завершение построения интерфейса формы.
 * \details Поля ввода и кнопки действий помещаются в вертикальный компоновщик.
 */
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

/*!
 * \brief Заполнение полей, если форма предназначена для редактирования.
 * \param record - объект записи.
 * \param row - номер строки в модели таблицы.
 */
void BonusAbstr::fill_fields(const QSqlRecord & record, const int row) {
    card_number->setText(record.field(keys[0]).value().toString());
    name->setText(record.field(keys[1]).value().toString());
    bonus_amount->setText(record.field(keys[2]).value().toString());
    this->row = row;
}

/*!
 * \brief Реализация для окна ввода данных для добавления в таблицу.
 * \param abs - объект окна формы с введёнными данными.
 * \return текст ошибки.
 */
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


/*!
 * \brief Реализация для окна ввода данных для поиска в таблице.
 * \param abs - объект окна формы с введёнными данными.
 * \return текст ошибки.
 * \return
 */
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

/*!
 * \brief Реализация для окна ввода данных для изменения записи в таблице.
 * \param abs - объект окна формы с введёнными данными.
 * \return текст ошибки.
 */
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


/*!
 * \brief Конструктор формы для истории продаж.
 * \param titles - читаемые пользователем названия столбцов.
 * \param fields - сопоставление "Имя столбца в БД"-"Читаемое пользователем название столбца".
 * \param keys - список имён столбцов в БД в порядке,
 * представленном в таблице(не отсортированном).
 * \param impl - объект-обработчик полей формы, определяемый в соответствии с задачей
 * (добавление, изменение или поиск).
 * \param parent - родительский объект.
 *
 * \details В конструкторе создаются и размещаются в окне все элементы интерфейса.
 */
SellAbstr::SellAbstr(const QMap<QString, QString>& titles,
                       const QMap<QString, QString>& fields,
                       const QVector<QString>& keys,
                       const SellEntity* en,
                       QSharedPointer<Implement> impl,
                       QWidget* parent) : inputFields(parent, titles, fields, keys) {
    this->impl = impl;
    this->entity = en;
    edit_mode = false;
    //qDebug() << card_numbers;
    //qDebug() << medicines_ids;
    buildLayout();
    produceField1();
    produceField2();
    produceField3();
    produceField4();
    produceField5();
    produceField6();
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

/*!
 * \brief Метод, выполняемый при нажании на кнопку выполнения операции.
 */
void SellAbstr::exec_clicked() {
    QString error = impl->processFields(this);
    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Ошибка!", error);
    }
}

/*!
 * \brief Построение поля ввода даты продажи.
 */
void SellAbstr::produceField1() {
    QLabel* label_1 = new QLabel(fields[keys[1]], this);
    label_1->setObjectName("label_1");
    label_1->setStyleSheet(LABEL_STYLE);
    //labels.push_back(label_1);
    date_of_buy = new QCalendarWidget(this);
    date_of_buy->setObjectName(keys[1]);
    date_of_buy->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    date_of_buy->setMaximumWidth(300);
    flay->addRow(label_1, date_of_buy);
}

/*!
 * \brief Построение поля выбора бонусной карты покупателя.
 */
void SellAbstr::produceField2() {
    QLabel* label_2 = new QLabel(fields[keys[2]], this);
    label_2->setObjectName("label_2");
    label_2->setStyleSheet(LABEL_STYLE);
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
    connect(customer, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &SellAbstr::card_changed);
    flay->addRow(label_2, customer);
    label_2 = nullptr;

}

/*!
 * \brief Построение поля выбора купленного лекарства.
 */
void SellAbstr::produceField3() {
    QLabel* label_3 = new QLabel(fields[keys[3]], this);
    label_3->setObjectName("label_3");
    label_3->setStyleSheet(LABEL_STYLE);
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
    connect(medicines, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &SellAbstr::med_changed);
    flay->addRow(label_3, medicines);
    label_3 = nullptr;
}

/*!
 * \brief Построение поля ввода количества купленного лекартсва.
 */
void SellAbstr::produceField4() {
    QLabel* label_4 = new QLabel(fields[keys[4]], this);
    label_4->setObjectName("label_4");
    label_4->setStyleSheet(LABEL_STYLE);
    //labels.push_back(label_3);
    pieces = new QSpinBox(this);
    pieces->setObjectName(keys[4]);
    pieces->setStyleSheet("QSpinBox { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    pieces->setMinimumHeight(30);
    pieces->setMaximumWidth(50);
    connect(pieces, QOverload<int>::of(&QSpinBox::valueChanged), this, &SellAbstr::med_amount_changed);
    pieces->setMinimum(1);
    flay->addRow(label_4, pieces);
    label_4 = nullptr;
}

/*!
 * \brief Построение поля с выручкой.
 */
void SellAbstr::produceField5() {
    QLabel* label_5 = new QLabel(fields[keys[5]], this);
    label_5->setObjectName("label_5");
    label_5->setStyleSheet(LABEL_STYLE);
    earnings = new QLineEdit("", this);
    earnings->setObjectName(keys[5]);
    earnings->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    earnings->setMinimumHeight(30);
    earnings->setReadOnly(true);
    QRegularExpression regexp("[+]?\\d{1,20}([.,]\\d{1,2})?");
    QValidator* valid = new QRegularExpressionValidator(regexp, this);
    earnings->setValidator(valid);
    flay->addRow(label_5, earnings);
    label_5 = nullptr;
}

/*!
 * \brief Построение поля с выручкой.
 */
void SellAbstr::produceField6() {
    bonuswidget = new QWidget(this);
    bonuswidget->setObjectName("bonuswidget");
    bonuslay = new QHBoxLayout(bonuswidget);
    QLabel* bonuses_withdraw_text = new QLabel("Бонусов к списанию: ", bonuswidget);
    bonuses_withdraw_text->setObjectName("bonuses_withdraw_text");
    bonuses_withdraw_text->setStyleSheet(LABEL_STYLE);
    bonuslay->addWidget(bonuses_withdraw_text);

    bonuses_to_withdraw = new QLineEdit("0", bonuswidget);
    bonuses_to_withdraw->setObjectName("bonuses_to_withdraw");
    bonuses_to_withdraw->setStyleSheet("QLineEdit { font-family: \"Sans Serif\"; font-style: regular; font-size: 17px; }");
    bonuses_to_withdraw->setMinimumWidth(30);
    bonuses_to_withdraw->setMaximumWidth(200);
    bonuses_to_withdraw->setMinimumHeight(30);
    QRegularExpression regexp("\\d{1,20}");
    QValidator* valid = new QRegularExpressionValidator(regexp, this);
    bonuses_to_withdraw->setValidator(valid);
    bonuses_to_withdraw->setReadOnly(true);
    bonuswidget->setObjectName("bonuses_to_withdraw");
    connect(bonuses_to_withdraw, &QLineEdit::textEdited, this, &SellAbstr::bonus_withdraw_edited);
    bonuslay->addWidget(bonuses_to_withdraw);
    withdraw_all_bonuses = new QCheckBox("Снять все бонусы");
    withdraw_all_bonuses->setObjectName("withdraw_all_bonuses");
    withdraw_all_bonuses->setTristate(false);
    withdraw_all_bonuses->setStyleSheet("QCheckBox { font-family: \"Sans Serif\"; font-style: regular; font-size: 18px; }");
    bonuslay->addStretch();
    bonuslay->addWidget(withdraw_all_bonuses);
    connect(withdraw_all_bonuses, &QCheckBox::stateChanged, this, &SellAbstr::withdraw_all_state);

    QSizePolicy sp_retain = bonuswidget->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    bonuswidget->setSizePolicy(sp_retain);
    flay->addWidget(bonuswidget);
    bonuswidget->setVisible(false);
}

/*!
 * \brief Завершение построения интерфейса формы.
 * \details Поля ввода и кнопки действий помещаются в вертикальный компоновщик.
 */
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

/*!
 * \brief Метод изменения параметров выбора количества лекартсва.
 * \param med_id - выбранный айди лекарства.
 * \details Вызывается при смене выбранного лекарства. Выставляет
 * максимальное допустимое значение лекарства в соответствии с количеством
 * этого лекарства на складе. Также меняет значение в поле выручки.
 */
void SellAbstr::med_changed(const QString& med_id) {
    if (medicines->currentIndex() == 0) {
        pieces->setEnabled(false);
        if (earnings) earnings->setText("");
        bonuswidget->setVisible(false);
        bonuses_to_withdraw->setReadOnly(true);
        return;
    }
    bonuswidget->setVisible((bool)(customer->currentIndex() != 0));
    pieces->setEnabled(true);
    int amount = 0;
    //int price = 0;
    if (!entity->get_med_amount(med_id.toInt(), amount)) {
        pieces->setMinimum(0);
        pieces->setMaximum(0);
        basic_price = -1;
        current_unit_price = -1;
    }
    else {
        pieces->setMaximum(amount);
        if (entity->get_med_price(med_id.toInt(), current_unit_price)) {
            basic_price = current_unit_price * pieces->value();
        } else {
            basic_price = -1;
            current_unit_price = -1;
        }
    }
    recalc_earnings();
}

/*!
 * \brief Метод изменения содержащейся суммы в поле выручки.
 * \param value - выбранное количество лекарства.
 */
void SellAbstr::med_amount_changed(int value) {
    if (current_unit_price >= 0) {
        basic_price = current_unit_price * value;
    }
    else {
        basic_price = -1;
    }
    recalc_earnings();
}

void SellAbstr::card_changed(const QString&) {
    bonuswidget->setVisible((bool)(customer->currentIndex() != 0 && medicines->currentIndex() != 0));
    if (customer->currentIndex() != 0) recalc_earnings();
}

void SellAbstr::withdraw_all_state(int) {
    recalc_earnings();
}

void SellAbstr::bonus_withdraw_edited(const QString &) {
    if (!edit_mode) recalc_earnings();
    else bonuses_to_withdraw->setReadOnly(false);
}

/*!
 * \brief Пересчёт выручки.
 * \details Вызывается при изменении купленного лекарства, количества лекартсва, бонусной карты
 * или выбора списать баллы с карты.
 */
void SellAbstr::recalc_earnings() {
    if (medicines->currentIndex() > 0) {
        if (withdraw_all_bonuses->isVisible() && withdraw_all_bonuses->checkState() == Qt::Checked) {
            int bonuses = 0;
            if (entity->get_card_balance(customer->currentText().toInt(), bonuses)) {
                //bonuses_to_withdraw->setEnabled(true);
                int result = 0;
                int bonuses_kop = bonuses * 100;
                if (bonuses_kop <= basic_price) {
                    bonuses_to_withdraw->setText(QString::number(bonuses));
                    result = basic_price - bonuses_kop;
                }
                else {
                    bonuses_to_withdraw->setText(QString::number(basic_price / 100));
                }
                earnings->setText(QString("%1,%2").arg(QString::number(result / 100))
                                                   .arg(QString::number(result % 100), 2, QLatin1Char('0')));
            }
            else {
                bonuses_to_withdraw->setReadOnly(true);
                bonuses_to_withdraw->setText("Бонусов на карте нет.");
            }
        }
        else if (withdraw_all_bonuses->isVisible()) {
            //bonuses_to_withdraw->setEnabled(true);
            bonuses_to_withdraw->setText("0");
            earnings->setText(QString("%1,%2").arg(QString::number(basic_price / 100))
                                               .arg(QString::number(basic_price % 100), 2, QLatin1Char('0')));
        }
        else {
            bonuses_to_withdraw->setText("0");
            earnings->setText(QString("%1,%2").arg(QString::number(basic_price / 100))
                                               .arg(QString::number(basic_price % 100), 2, QLatin1Char('0')));
        }
    }
}

/*!
 * \brief Заполнение полей, если форма предназначена для редактирования.
 * \param record - объект записи.
 * \param row - номер строки в модели таблицы.
 * \details Включает редактивроание выручки.
 */
void SellAbstr::fill_fields(const QSqlRecord & record, const int row) {
    QDate* date = new QDate(QDate::fromString(record.field(keys[1]).value().toString(), "dd-MM-yyyy"));
    if (date && date->isValid()) {
        date_of_buy->setSelectedDate(*date);
    }
    else {
        date_of_buy->setSelectedDate(QDate::currentDate());
    }
    if (date) delete date;
    customer->setCurrentText(record.field(keys[2]).value().toString());
    medicines->setCurrentText(record.field(keys[3]).value().toString());
    pieces->setValue(record.field(keys[4]).value().toLongLong());
    QString val = record.field(keys[5]).value().toString();
    int length = val.length();
    if (length > 2) {
        val.insert(length - 2, ',');
    }
    else {
        val.push_front("0,");
    }
    earnings->setText(val);
    QString bonuses = record.field(keys[6]).value().toString();
    if (!bonuses.isEmpty()) {
        withdraw_all_bonuses->setChecked(true);
    }
    bonuses_to_withdraw->setText(bonuses);
    this->row = row;
    record_edit_enabled(true);
}

/*!
 * \brief Включение или отключение возможности редактирования выручки.
 * \param enabled - режим (вкл/выкл).
 */
void SellAbstr::record_edit_enabled(const bool enabled) {
    edit_mode = true;
    earnings->setReadOnly(!enabled);
    bonuses_to_withdraw->setReadOnly(!enabled);
}

/*!
 * \brief Реализация для окна ввода данных для добавления в таблицу.
 * \param abs - объект окна формы с введёнными данными.
 * \return текст ошибки.
 */
QString AddSellImplement::processFields(inputFields* abs) {
    if (abs == nullptr) return "Undefined";
    SellAbstr* concreteAbs = qobject_cast<SellAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
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
    else if (concreteAbs->bonuses_to_withdraw->text().isEmpty() ||
             !concreteAbs->bonuses_to_withdraw->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->bonuses_to_withdraw->objectName()] +
                             "\"!";
    }
    else {
        QSqlRecord* rec = new QSqlRecord();
        rec->append(QSqlField(concreteAbs->keys[1]));
        qDebug() << concreteAbs->date_of_buy->selectedDate().toString("dd-MM-yyyy");
        rec->setValue(concreteAbs->keys[1], concreteAbs->date_of_buy->selectedDate().toString("dd-MM-yyyy"));
        rec->append(QSqlField(concreteAbs->keys[2]));
        if (!concreteAbs->customer->currentText().isEmpty()) {
            rec->setValue(concreteAbs->keys[2], concreteAbs->customer->currentText());
        }
        rec->append(QSqlField(concreteAbs->keys[3]));
        rec->setValue(concreteAbs->keys[3], concreteAbs->medicines->currentText());
        rec->append(QSqlField(concreteAbs->keys[4]));
        rec->setValue(concreteAbs->keys[4], concreteAbs->pieces->text());
        rec->append(QSqlField(concreteAbs->keys[5]));
        QString earnings_str = concreteAbs->earnings->text().replace('.', ',');
        int val = 0;
        if (earnings_str.contains(',')) {
            val = earnings_str.remove(',').toInt();
        }
        else {
            val = earnings_str.toInt() * 100;
        }
        rec->setValue(concreteAbs->keys[5], val);
        rec->append(QSqlField(concreteAbs->keys[6]));
        rec->setValue(concreteAbs->keys[6], concreteAbs->bonuses_to_withdraw->text());
        emit Implement::exec_clicked_signal(rec);
        delete rec;
    }
    return NULL;
}


/*!
 * \brief Реализация для окна ввода данных для поиска в таблице.
 * \param abs - объект окна формы с введёнными данными.
 * \return текст ошибки.
 * \return
 */
QString FindSellImplement::processFields(inputFields * abs) {
    if (abs == nullptr) return "Undefined";
    SellAbstr* concreteAbs = qobject_cast<SellAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
    QString where;
    if (concreteAbs->date_of_buy->isEnabled()) {
        where += concreteAbs->date_of_buy->objectName() +
                " = \"" + concreteAbs->date_of_buy->selectedDate().toString("dd-MM-yyyy") +
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
    if (!concreteAbs->earnings->text().isEmpty() && concreteAbs->earnings->hasAcceptableInput()) {
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

/*!
 * \brief Реализация для окна ввода данных для изменения записи в таблице.
 * \param abs - объект окна формы с введёнными данными.
 * \return текст ошибки.
 */
QString EditSellImplement::processFields(inputFields * abs) {
    if (abs == nullptr) return "Undefined";
    SellAbstr* concreteAbs = qobject_cast<SellAbstr*>(abs);
    if (concreteAbs == nullptr) return "Undefined";
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
    else if (concreteAbs->bonuses_to_withdraw->text().isEmpty() ||
             !concreteAbs->bonuses_to_withdraw->hasAcceptableInput()) {
        return "Ошибка ввода в поле \"" +
                             concreteAbs->fields[concreteAbs->bonuses_to_withdraw->objectName()] +
                             "\"!";
    }
    else {
        QSqlRecord* rec = new QSqlRecord();
        rec->append(QSqlField(concreteAbs->keys[1]));
        rec->setValue(concreteAbs->keys[1], concreteAbs->date_of_buy->selectedDate().toString("dd-MM-yyyy"));
        rec->append(QSqlField(concreteAbs->keys[2]));
        rec->setValue(concreteAbs->keys[2], concreteAbs->customer->currentText());
        rec->append(QSqlField(concreteAbs->keys[3]));
        rec->setValue(concreteAbs->keys[3], concreteAbs->medicines->currentText());
        rec->append(QSqlField(concreteAbs->keys[4]));
        rec->setValue(concreteAbs->keys[4], concreteAbs->pieces->text());
        rec->append(QSqlField(concreteAbs->keys[5]));
        QString earnings_str = concreteAbs->earnings->text().replace('.', ',');
        int val = 0;
        if (earnings_str.contains(',')) {
            val = earnings_str.remove(',').toInt();
        }
        else {
            val = earnings_str.toInt() * 100;
        }
        rec->setValue(concreteAbs->keys[5], val);
        rec->append(QSqlField(concreteAbs->keys[6]));
        rec->setValue(concreteAbs->keys[6], concreteAbs->bonuses_to_withdraw->text());
        emit Implement::exec_clicked_signal(*rec, concreteAbs->row);
        delete rec;
    }
    return NULL;
}

