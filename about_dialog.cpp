#include <QTextBrowser>

#include "about_dialog.h"
#include "ui_about_dialog.h"

about_Dialog::about_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::about_Dialog)
{
    ui->setupUi(this);
}

about_Dialog::~about_Dialog()
{
    delete ui;
}
