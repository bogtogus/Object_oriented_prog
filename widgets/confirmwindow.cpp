#include "confirmwindow.h"


ConfirmWindow::ConfirmWindow(const QString& labelText, QWidget* parent) : QDialog(parent) {
    setWindowTitle("Подтверждение");
    verticalLayout = new QVBoxLayout(this);
    label = new QLabel(this);
    label->setTextFormat(Qt::PlainText);
    label->setWordWrap(true);
    label->setText(labelText);
    //label->setMinimumSize(140, 40);
    verticalLayout->addWidget(label);

    verticalSpacer = new QSpacerItem(10, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);

    horizontalLayout = new QHBoxLayout();
    no = new QPushButton(this);
    //no->setMinimumSize(QSize(80, 30));
    no->setDefault(true);
    no->setText("Отмена");
    horizontalLayout->addWidget(no);

    horizontalSpacer = new QSpacerItem(20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);

    yes = new QPushButton(this);
    //yes->setMinimumSize(QSize(80, 30));
    yes->setText("Ок");
    horizontalLayout->addWidget(yes);
    verticalLayout->addLayout(horizontalLayout);
    this->adjustSize();

    connect(yes, &QPushButton::clicked, this, &ConfirmWindow::onYesClicked);
    connect(no, &QPushButton::clicked, this, &ConfirmWindow::onNoClicked);
}

ConfirmWindow::~ConfirmWindow() {

}

void ConfirmWindow::onYesClicked() {
    accept();
}

void ConfirmWindow::onNoClicked() {
    reject();
}
