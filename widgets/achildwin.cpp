#include "achildwin.h"

aChildWin::aChildWin(QWidget* parent) : QWidget(parent) {
    this->prnt = parent;
}

aChildWin::~aChildWin() {
    qDebug() << "DEL child";
}

