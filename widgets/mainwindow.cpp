#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qDebug() << "C: " << ui->stackedWidget->count();
    QDir tempdir = QDir().currentPath();
    this->path = new QDir(tempdir);
    // Задание текущего пути как абсолютного пути
    // к файлам проекта, а не к .exe
    if (path->dirName().contains("build", Qt::CaseInsensitive)) {
        path->cdUp();
        path->cd("Object_oriented_prog");
    }
    // Файл .ini используется для сохранения текущего размера экрана,
    // а затем для чтения размера при открытии приложения заново
    settings = new QSettings("settings.ini", QSettings::IniFormat, this);
    loadSettings();
    initDB();
    QPixmap temp = this->logo->scaled(ui->drugery_logo->width(),
                               ui->drugery_logo->height(),
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);
    ui->drugery_logo->setPixmap(temp);
    ui->drugery_logo->setMask(temp.mask());
    ui->drugery_logo->setAlignment(Qt::AlignCenter);
    ui->drugery_logo->show();
    if (ui->stackedWidget->count() > 1) {
        ui->stackedWidget->removeWidget(ui->stackedWidget->widget(1));
    }
    connect(ui->tomedicines, &QPushButton::clicked, this, &MainWindow::tomedicines_clicked);
    connect(ui->tobonusprogram, &QPushButton::clicked, this, &MainWindow::tobonusprogram_clicked);
    connect(ui->tosellhistory, &QPushButton::clicked, this, &MainWindow::tosellhistory_clicked);
    qDebug() << "C: " << ui->stackedWidget->count();
    //ui->drugery_logo->setMinimumSize(ui->drugery_logo->minimumHeight(), ui->drugery_logo->minimumHeight());
    //ui->drugery_logo->setMaximumSize(ui->drugery_logo->minimumHeight(), ui->drugery_logo->minimumHeight());
}

MainWindow::~MainWindow() {
    qDebug() << "DEL main {{";
    delete BEntity;
    delete MEntity;
    delete SEntity;
    if (db->isOpen()) {
        qDebug() << "close db";
        db->close();
    }
    qDebug() << db->connectionNames();
    QString conname = db->connectionName();
    qDebug() << "remove db";
    //qDebug() << db.QSharedPointer::
    db.clear();
    QSqlDatabase::removeDatabase(conname);
    qDebug() << db->connectionNames();

    saveSettings();
    delete ui;
    qDebug() << "}}";
}

// Обработка нажания на кнопку закрытия окна [X]
void MainWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Выход",
                                                                "Выйти из приложения? Все несохранённые изменения будут отменены.\n",
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    }
    else {
        if (db->isOpen()) {
            qDebug() << "close db";
            db->close();
        }
        qDebug() << "close main (event) {";
        int index = ui->stackedWidget->count() - 1;
        // Закрытие всех виджетов-окон перед закрытием основного окна
        while(index >= 0) {
            QWidget* widget = ui->stackedWidget->widget(index);
            ui->stackedWidget->removeWidget(widget);
            widget->close();
            index--;
        }
        qDebug() << "}";
        event->accept();
    }
}

// Загрузка настроек
void MainWindow::loadSettings() {
    this->logo = new QPixmap(":/images/apteka.png");
    this->resize(settings->value("window_size", QSize(400, 400)).toSize());
}

// Сохранение
void MainWindow::saveSettings() {
    settings->setValue("logotype", settings->value("logotype", "images/apteka.png"));
    settings->setValue("window_size", QSize(std::max(this->minimumSize().width(), this->geometry().width()),
                                            std::max(this->minimumSize().height(),this->geometry().height())));
}

// Получения пути к файлам проекта
QDir MainWindow::getCurPath() const {
    return *path;
}

// Получение указателя на базу данных
QSharedPointer<QSqlDatabase> MainWindow::getDB() const {
    return this->db;
}

// Инициализация базы данных
void MainWindow::initDB() {
    qDebug() << "open db";
    this->db = QSharedPointer<QSqlDatabase>(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE")));
    qDebug() << getCurPath().filePath("database.db");
    db->setDatabaseName(getCurPath().filePath("database.db"));
    if (!db->open()) {
        qDebug() << "Error when opening DB." << db->lastError().text();
    }
    /* Здесь и далее по методу: создание таблиц и триггеров,
     * если они не существуют
     * в директории проекта
    */

    QString qu = "CREATE TABLE IF NOT EXISTS medicines ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                 "manufactorer STRING NOT NULL,"
                 "name STRING NOT NULL,"
                 "date_of_manuf DATE NOT NULL,"
                 "expiry_date DATE NOT NULL,"
                 "on_prescription INTEGER DEAFULT (0),"
                 "pieces INTEGER CHECK (pieces >= 0) DEFAULT (1),"
                 "price_for_one INTEGER NOT NULL DEFAULT (0) CHECK (price_for_one >= 0)"
                 ");";
    QSqlQuery* query = new QSqlQuery(this->db->connectionName());
    if(!query->exec(qu)) {
        qDebug() << "Error when creating table \"medicines\".";
    }
    qu = "CREATE TRIGGER date_valid_med "
         "       BEFORE INSERT "
         "       ON medicines "
         "BEGIN "
         "    SELECT CASE WHEN strftime(NEW.date_of_manuf) != NEW.date_of_manuf OR "
         "        strftime(NEW.expiry_date) != NEW.expiry_date "
         "             THEN RAISE(ABORT, \"Invalid date\") END; "
         "END;";
    if(!query->exec(qu)) {
        qDebug() << "Error when creating trigger in table \"medicines\"." << query->lastError();
    }
    qu = "CREATE TABLE IF NOT EXISTS reged_customers ("
                     "card_number INTEGER PRIMARY KEY,"
                     "name STRING NOT NULL,"
                     "balance INTEGER CHECK (balance >= 0) DEFAULT (0)"
                     ");";
    if(!query->exec(qu)) {
        qDebug() << "Error when creating table \"reged_customers\".";
    }
    qu = "CREATE TABLE IF NOT EXISTS sales_history ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "date_of_buy DATE NOT NULL,"
            "customer STRING,"
            "medicines TEXT NOT NULL,"
            "pieces INTEGER CHECK (pieces > 0) NOT NULL,"
            "earnings BIGINT DEFAULT (0) NOT NULL,"
            "FOREIGN KEY(customer) references reged_customers(card_number) "
            "   ON DELETE NO ACTION,"
            "FOREIGN KEY(medicines) references medicines(id)"
            "ON DELETE NO ACTION ON UPDATE NO ACTION"
         ");";
    if(!query->exec(qu)) {
        qDebug() << "Error when creating table \"sales_history\".";
    }
    qu = "UPDATE SQLITE_SEQUENCE SET seq = 10000000 WHERE name = \"reged_customers\";";
    if(!query->exec(qu)) {
        qDebug() << "Error when setting autoincrement in table \"reged_customers\".";
    }
    qu = "CREATE TRIGGER date_valid "
         "   BEFORE INSERT "
         "       ON sales_history "
         "BEGIN "
         "    SELECT CASE WHEN strftime(NEW.date_of_buy) != NEW.date_of_buy "
         "        THEN RAISE(ABORT, \"Invalid date\") END; "
         "END; ";
    if(!query->exec(qu)) {
        qDebug() << "Error when creating trigger in table \"sales_history\"." << query->lastError();
    }
    query->finish();
    query->clear();
    delete query;
    if (db->isOpen()) {
        qDebug() << "close db";
        db->close();
    }
    QVector<QString> Mfn = {"ID", "Производитель", "Название лек-ва",
                            "Дата изгот.", "Годен до", "Рецепт необходим",
                            "Количество", "Цена за шт."};
    QVector<QString> Bfn = {"Номер карты", "ФИО", "Кол-во баллов"};
    QVector<QString> Sfn = {"ID", "Дата покупки", "Бонусная карта",
                            "ID лекарства",
                            "Количество", "Выручка"};
    // Создание объектов, работающих с конкретными таблицами БД
    MEntity = new MedsEntity(db, Mfn);
    BEntity = new BonusEntity(db, Bfn);
    SEntity = new SellEntity(db, Sfn, MEntity, BEntity);
}

void MainWindow::tomedicines_clicked() {
    push_back_widget(new MedicinesWindow(this, MEntity));
}

void MainWindow::tobonusprogram_clicked() {
    push_back_widget(new BonusProgWindow(this, BEntity));
}

void MainWindow::tosellhistory_clicked() {
    push_back_widget(new SellHistWindow(this, MEntity, BEntity, SEntity));
}

// Удаление отображаемого виджета из QStackedWidget
// и разъединение с сигналами этого виджета
void MainWindow::pop_back_widget(aChildWin* widget) {
    qDebug() << "close childw ( ";
    if (ui->stackedWidget->indexOf(widget) >= 0) {
        ui->stackedWidget->removeWidget(widget);
        qDebug() << "Disconnect with " << widget;
        disconnect(widget, nullptr, nullptr, nullptr);
        widget->close();
        qDebug() << "C: " << ui->stackedWidget->count();
        ui->stackedWidget->setCurrentIndex(ui->stackedWidget->count() - 1);
    }
    qDebug() << ")";
}

// Добавление нового(дочернего) виджета в QStackedWidget
// и соединение с его сигнала о возврате назад и вызове
// своего дочернего окна
void MainWindow::push_back_widget(aChildWin* widget) {
    qDebug() << "open childw (";
    if (ui->stackedWidget->indexOf(widget) < 0) {
        qDebug() << "Connect with " << widget;
        connect(widget, &aChildWin::goback_signal, this, &MainWindow::pop_back_widget);
        connect(widget, &aChildWin::summoned_child, this, &MainWindow::push_back_widget);
        ui->stackedWidget->addWidget(widget);
        ui->stackedWidget->setCurrentWidget(widget);
        qDebug() << "C: " << ui->stackedWidget->count();
    }
    qDebug() << ")";
}

// О бонусной программе
void MainWindow::on_aboutbonuses_clicked() {
    QMessageBox* about = new QMessageBox(this);
    QFont* font = new QFont();
    font->setPointSize(11);
    about->setWindowTitle("Бонусная программа");
    about->setFont(*font);
    about->setText("Бонусная программа - специальная система накопления баллов "
                   "покупателями. Для участия в программе покупателю необходимо оформить "
                   "карту аптеки. При покупке лекарств на счёт этой карты начисляются "
                   "бонусы в размере 3% от стоимости покупки. Накопленные бонусы можно "
                   "потратить на компенсацию стоимости последующих покупок.");
    about->setDefaultButton(QMessageBox::Ok);
    QPixmap* px = new QPixmap( 32, 32 );
    px->fill( Qt::transparent );
    about->setWindowIcon(QIcon(*px));
    about->setIcon(QMessageBox::Information);
    about->exec();
    delete font;
    delete about;
    delete px;
}
