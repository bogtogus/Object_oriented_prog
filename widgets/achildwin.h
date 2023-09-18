#ifndef ACHILDWIN_H
#define ACHILDWIN_H

#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QDebug>

/*!
 * \brief Базовый абстрактный класс для дочерних окон (по отношению к основному MainWindow).
 * \details Сигнал goback_signal(...) оповещает MainWindow о том, что текущее
 * окно нужно закрыть. Сигнал summoned_child(...) оповещает MainWindow о том, что
 * нужно отобразить новое дочернее окно.
 */
class aChildWin : public QWidget
{
Q_OBJECT
public:
    explicit aChildWin(QWidget* parent);
    virtual ~aChildWin();
    virtual void goback() = 0;
protected:
    QWidget* prnt;
signals:
    void goback_signal(aChildWin*);
    void summoned_child(aChildWin*);
};

#endif // ACHILDWIN_H
