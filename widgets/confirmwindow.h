#ifndef CONFIRMWINDOW_H
#define CONFIRMWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

/**
 * @brief Класс подтверждения выполнения операции.
 * @details Информация об операции задаётся в параметре контруктора класса.
 */
class ConfirmWindow : public QDialog {
    Q_OBJECT

public:
    explicit ConfirmWindow(const QString&, QWidget* parent = nullptr);
    ~ConfirmWindow();

private slots:
    void onYesClicked();
    void onNoClicked();

private:
    QVBoxLayout* verticalLayout;
    QLabel* label;
    QSpacerItem* verticalSpacer;
    QHBoxLayout* horizontalLayout;
    QPushButton* no;
    QSpacerItem* horizontalSpacer;
    QPushButton* yes;
};

#endif // CONFIRMWINDOW_H
