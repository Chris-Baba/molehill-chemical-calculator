#ifndef EDIT_DATABASE_H
#define EDIT_DATABASE_H

#include <QDialog>
#include <QObject>
#include <QSqlTableModel>
#include "ui_edit_database.h"

// namespace Qsl {
namespace Ui {
class Edit_DataBase;
}

class Edit_DataBase : public QWidget
{
    Q_OBJECT

public:
    //explicit Edit_DataBase(QWidget *parent = nullptr);
    explicit Edit_DataBase(QWidget *parent);
    //explicit Edit_DataBase(const QString &tableName, QDialog *parent = nullptr);
    ~Edit_DataBase();

private:
    Ui::Edit_DataBase *ui3;
    QPushButton *submitButton;
    QPushButton *revertButton;
    QPushButton *quitButton;
    QDialogButtonBox *buttonBox;
    QSqlTableModel *model;

private slots:

    void accept();
    void reject();
    void submit();
};

#endif // EDIT_DATABASE_H
