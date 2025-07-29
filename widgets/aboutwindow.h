#ifndef INFOWINDOW_H
#define INFOWINDOW_H
#include <QDialog>

/**
 * @brief Класс окна информации о приложении.
 * @details Содержит копирайт Qt, версию номер сборки приложения.
 * Удаляется при закрытии по флагу Qt::WA_DeleteOnClose.
 */
class AboutWindow : public QDialog {
    Q_OBJECT
public:
    explicit AboutWindow(QWidget* parent = nullptr);
    ~AboutWindow();
private:

};

#endif // INFOWINDOW_H
