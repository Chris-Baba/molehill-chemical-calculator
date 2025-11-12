#ifndef SELECT_DIALOG_H
#define SELECT_DIALOG_H

// #include "qmainwindow.h"
#include <QDialog>
#include <QObject>
#include <QSql>
#include <QSqlTableModel>
#include <QTableView>
#include <QtGui>

namespace Ui {
class Select_Dialog;
}

class Select_Dialog : public QDialog
{
    Q_OBJECT

public:
    Select_Dialog(QWidget *parent = nullptr);
    ~Select_Dialog();
    Ui::Select_Dialog *ui4;
    void select_from_database(QString reagent_fragment);
    void chem_db_insert(QString reagent, double mw);
    int chem_db_read(QString reagent_fragment);

    friend class MainWindow; //Allows access to MainWindow functions/

    //------------------------------------------------
    //These declarations have class(?) scope,
    // Use scope resolution operator (::) to use these public variables.
    // For example: "Edit_Dialog::g_selected_row".

    int g_selected_row;
    int g_selected_column;
    int g_OK_clicked;
    QSqlTableModel *model;
    QTableView *view;
    //QSqlDatabase g_db;

    //------------------------------------------------
public slots:

signals:

private slots:
    void on_pushButton_Cancel_clicked();
    void on_tableView_clicked(const QModelIndex &index);
    void on_pushButton_OK_clicked();

private:
};

#endif // SELECT_DIALOG_H
