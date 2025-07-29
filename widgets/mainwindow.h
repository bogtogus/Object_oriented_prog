#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QDir>
#include <QTextStream>
#include <QSettings>
#include <QPixmap>
#include <QBitmap>
#include <QtSql>
#include <QSharedPointer>
#include <QMessageBox>
#include <string>
#include <math.h>


#include "medicineswindow.h"
#include "bonusprogwindow.h"
#include "sellhistwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/*!
 * \brief Класс основного окна.
 * \details Это окно содержит в себе стэк-виджет, в котором
 * отображаются главное меню, а также любые окна-виджеты.
 * Благодаря этому при переходе к другому окну-виджету нет
 * ситуации закрытия предыдущего и открытия следующего окна.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSharedPointer<QSqlDatabase> getDB() const;
    QDir getCurPath() const;
    // Удаление отображаемого виджета из QStackedWidget
    void pop_back_widget(aChildWin*);
    // Добавление нового(дочернего) виджета в QStackedWidget
    void push_back_widget(aChildWin*);

private slots:
    void saveSettings();
    void loadSettings();
    void initDB();
    void tomedicines_clicked();
    void tobonusprogram_clicked();
    void tosellhistory_clicked();
    void closeEvent(QCloseEvent *event) override;
    void on_aboutbonuses_clicked();
    void openAbout();

private:
    Ui::MainWindow *ui;
    // Наспройки, читаемые из .ini
    QSettings* settings;
    QPixmap* logo;
    QSharedPointer<QSqlDatabase> db;
    QDir* path;
    // Класс таблицы лекарств
    MedsEntity* MEntity;
    // Класс таблицы бонусных карт
    BonusEntity* BEntity;
    // Класс таблицы продаж
    SellEntity* SEntity;
};
#endif // MAINWINDOW_H
