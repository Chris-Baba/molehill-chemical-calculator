#ifndef EDIT_DIALOG_H
#define EDIT_DIALOG_H

#include <QDialog>
#include <QSql>
#include <QObject>
#include <QSqlTableModel>
#include <QTableView>

namespace Ui {
class Edit_Dialog;
}

class Edit_Dialog : public QDialog
{
    Q_OBJECT

public:
    friend class MainWindow;

    explicit Edit_Dialog(QWidget *parent = nullptr);
    ~Edit_Dialog();
    Ui::Edit_Dialog *ui3;

    //------------------------------------------------
    //These declarations have class(?) scope,
    // Use scope resolution operator (::) to use these public variables.
    // For example: "Edit_Dialog::g_selected_row".

    // QSqlDatabase g_db;  // Delete?
    static int g_selected_row;
    static int g_selected_column;
    static QString g_reagent;
    static double g_mw;
    static QString g_mw_string;
    int g_OK_clicked;
    static QSqlTableModel *model;
    QTableView *view;
    //------------------------------------------------

public slots:


private slots:
    void on_pushButton_Cancel_clicked();
    // void on_pushButton_OK_clicked(const QModelIndex &index);
    void on_pushButton_DeleteRow_clicked();
    void on_pushButton_AddRow_clicked();
    void user_selected_index(QModelIndex selected_index);
  //  void select_from_database(QString reagent_fragment);
    void on_tableView_clicked(const QModelIndex &index);
    void on_pushButton_OK_clicked();

private:
    // Ui::Edit_Dialog *ui3;

};

#endif // EDIT_DIALOG_H
