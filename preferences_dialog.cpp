#include <QMainWindow>
#include <QObject>

#include "mainwindow.h"
#include "preferences_dialog.h"
#include "ui_preferences_dialog.h"

Preferences_Dialog::Preferences_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Preferences_Dialog)
{
    ui->setupUi(this);

    // Set checkboxes to reflect configuration file status.
    MainWindow myObj;
    if (myObj.g_NetworkAccessPermission == true) {
        ui->checkBox_Allow->setChecked(true);
    } else
        ui->checkBox_Allow->setChecked(false);

    if (myObj.g_NetworkAccessDontAskAgain == true) {
        ui->checkBox_DontAsk->setChecked(true);
    } else
        ui->checkBox_DontAsk->setChecked(false);
}

Preferences_Dialog::~Preferences_Dialog()
{
    delete ui;
}

void Preferences_Dialog::on_buttonBox_accepted()
{
    qDebug() << "OK Selected in Preferences_Dialog";

    bool allow = ui->checkBox_Allow->isChecked();
    qDebug() << "allow = " << allow;
    bool dontAsk = ui->checkBox_DontAsk->isChecked();
    qDebug() << "dontAsk = " << dontAsk;

    MainWindow myObj;
    myObj.parseConfigFile("NetworkAccessPermission ", true, allow);
    myObj.parseConfigFile("NetworkAccessAsked ", true, true);
    myObj.parseConfigFile("NetworkAccessDontAskAgain ", true, dontAsk);
    myObj.setConfigGlobals();
    done(1); //closes dialog and returns int "1"?

    qDebug() << "In runAtStart g_NetworkAccessPermission = " << myObj.g_NetworkAccessPermission;
    qDebug() << "In runAtStart g_NetworkAccessAsked = " << myObj.g_NetworkAccessAsked;
    qDebug() << "In runAtStart g_NetworkAccessDontAskAgain = " << myObj.g_NetworkAccessDontAskAgain;
}

void Preferences_Dialog::on_buttonBox_rejected()
{
    done(2); //closes dialog and returns int "2"?
}
