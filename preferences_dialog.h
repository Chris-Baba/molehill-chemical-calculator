#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QDialog>


namespace Ui {
class Preferences_Dialog;
}

class Preferences_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Preferences_Dialog(QWidget *parent = nullptr);
    ~Preferences_Dialog();
    friend class MainWindow;  //Allows access to MainWindow functions/
    QString parseConfigFile(QString, bool, bool);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::Preferences_Dialog *ui;
};

#endif // PREFERENCES_DIALOG_H
