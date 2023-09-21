#include "achildwin.h"

/*!
 * \file checked
 */

aChildWin::aChildWin(QWidget* parent) : QWidget(parent) {
    this->prnt = parent;
}

aChildWin::~aChildWin() {
    qDebug() << "DEL child";
}

