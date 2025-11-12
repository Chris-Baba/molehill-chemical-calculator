#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <QDialog>

namespace Ui {
class about_Dialog;
}

class about_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit about_Dialog(QWidget *parent = nullptr);
    ~about_Dialog();
    Ui::about_Dialog *ui;

private:
};

#endif // ABOUT_DIALOG_H
