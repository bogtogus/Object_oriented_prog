#include "aboutwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

AboutWindow::AboutWindow(QWidget* parent)
    : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("О программе");
    QVBoxLayout* vlay = new QVBoxLayout();
    QLabel* label = new QLabel();
    label->setText("Автоматизированная система учёта лекарств \"Фиалка-Системс\"<br/>"
                   "<br/>"
                   "Версия: 0.0.1<br/>"
                   "Дата выпуска: 29.07.2025<br/>"
                   "Автор: Гусев Богдан Андреевич<br/>"
                   "<br/>"
                   "Программа предназначена для автоматизации учёта лекарств аптеки<br/>");
    label->setTextFormat(Qt::RichText);
    label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    label->setOpenExternalLinks(true);
    label->setAlignment(Qt::AlignLeft);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    vlay->addWidget(label);
    QPushButton* closeButton = new QPushButton("Закрыть");
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addSpacerItem(new QSpacerItem(20, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    buttonLayout->addWidget(closeButton);
    vlay->addLayout(buttonLayout);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    setLayout(vlay);
}

AboutWindow::~AboutWindow() {

}
