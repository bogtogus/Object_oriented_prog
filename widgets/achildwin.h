#ifndef ACHILDWIN_H
#define ACHILDWIN_H

#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QDebug>

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
