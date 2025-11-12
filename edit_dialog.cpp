#include <QSql>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTableView>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QAbstractTableModel>
#include <QPersistentModelIndex>
#include <QSqlIndex>
#include <QSqlError>
#include <QSqlField>

#include <QPrinter>
#include <QString>
#include <QPrintDialog>
#include <QDialog>
#include <QLabel>
#include <QMainWindow>
#include <QObject>

#include <QRadioButton>
#include <QLineEdit>
#include <QMessageBox>
#include <QFile>    //REMOVE?
#include <QFileDialog>  //REMOVE?
#include <QTextStream>  //REMOVE?
#include <QDebug>
#include <QTextEdit>
#include <QVariant>

#include "edit_dialog.h"
#include "ui_edit_dialog.h"
#include "mainwindow.h"

// Define these public STATIC (class?) variables that were declared in header file.
// They must defined outside of any functions??
// Example 'Edit_Dialog::g_mw' to access from anywhere.
QString Edit_Dialog::g_reagent = "BLANK";
double Edit_Dialog::g_mw = 0.0;
QString Edit_Dialog::g_mw_string = "0.0";
QSqlTableModel* Edit_Dialog::model = NULL;
int Edit_Dialog::g_selected_row = 0;
int Edit_Dialog::g_selected_column = 0;

// Setup database connection outside of any function.
//QSqlDatabase MainWindow::g_db = QSqlDatabase::addDatabase("QSQLITE", \
                                                          "dbConnection");
//--------------------------------------

Edit_Dialog::Edit_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui3(new Ui::Edit_Dialog)
{
    ui3->setupUi(this);  //This is QtCreator code to make GUI.

    qDebug() << "Edit_Dialog::Edit_Dialog Constructor - STARTED";
    setWindowTitle("Edit Database");
    // QSqlDatabase db;

    qDebug() << "In Edit_Dialog, g_db.isValid() = " <<
        MainWindow::g_db.isValid();
    if (!MainWindow::g_db.isValid()){
        qDebug() << "In db_save -B";
        MainWindow::g_db = QSqlDatabase::addDatabase("QSQLITE");
    }

    // MainWindow::g_db = QSqlDatabase::addDatabase("QSQLITE");
    // QString 'g_dataBaseFilePath' is static class scope, declared in MainWindow header file.
    MainWindow::g_db.setDatabaseName(MainWindow::g_dataBaseFilePath);
    qDebug() << "In Edit_Dialog, g_dataBaseFilePath = " << \
                MainWindow::g_dataBaseFilePath;

    if (!MainWindow::g_db.isOpen()){
        bool DB_opened_OK = MainWindow::g_db.open();
        qDebug() << "DB_opend_OK = " << DB_opened_OK;
        if (DB_opened_OK == false) {
            qDebug() << "Unable to connect to the database";
            MainWindow myObj;
            myObj.cc_msgBox("Unable to connect to the local database");
            return;
        }
    }
    model = new QSqlTableModel(this);
    model->setTable("USER");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    // QTableView *view = new QTableView;
    view = ui3->tableView;
    view->setModel(model);
    view->setColumnWidth(0, 510);
    view->setColumnWidth(1,75);
    view->setWordWrap(true);
    view->setSortingEnabled(true);
    view->resizeRowsToContents();
    //-------------------------------
    qDebug() << "Edit_Dialog Constructor - ENDED";
}

Edit_Dialog::~Edit_Dialog()
{
    delete ui3;
}

void Edit_Dialog::on_pushButton_Cancel_clicked()
{
    qDebug() << "Edit_Dialog CANCEL clicked.";
    // This works just like this.  Changed data rolls back if not submitted.
    close();
}

void Edit_Dialog::on_pushButton_OK_clicked()
{
    qDebug() << "Edit_Dialog OK clicked-1.";
    model->submitAll();
    model->select();
    close();
}

void Edit_Dialog::on_pushButton_AddRow_clicked()
{
    qDebug() << "Edit_Dialog ADD ROW clicked-1.";

    model->insertRecord(-1, model->record());
    model->submitAll();
    if (model->lastError().isValid())
        qDebug() << "Last Error = " << model->lastError();
    model->select();
}

void Edit_Dialog::on_pushButton_DeleteRow_clicked()
{
    qDebug() << "Edit_Dialog DELETE ROW clicked.";
    qDebug() << "In 'delete_row', 'g_selected_row' = " << Edit_Dialog::g_selected_row;
    model->removeRows(g_selected_row, 1);
    model->submitAll();
    model->select();
}

void Edit_Dialog::user_selected_index(QModelIndex selected_index)   // <--SLOT
{
    // This function is a slot called whenever a user clicks on (selects)
    // a table item from any instance of an Edit_Dialog box (signal) of
    // reagents and molecular weights.  It sets 4 class-wide variables
    // (g_selected_row, g_selected_column g_reagent, g_mw) Which are declared
    // in header for class scope, for use in other functions.
    // It can also derive selected_index_model, but I use previousely defined 'model'.

    // Start setting 3 class-wide variables.
    Edit_Dialog::g_selected_row = selected_index.row();
    qDebug() << "g_selected_row = " << Edit_Dialog::g_selected_row;

    Edit_Dialog::g_selected_column = selected_index.column();
    qDebug() << "g_selected_column = " << Edit_Dialog::g_selected_column;

    // Don't use next form of model.
    qDebug() << "selected_index_model() = " <<  selected_index.model();
    // Use next line instead (the class scope form, QSqlTableModel *model;).
    qDebug() << "Edit_Dialog::model = " << Edit_Dialog::model;

    Edit_Dialog::g_reagent = model->record(g_selected_row).value("Reagent").toString();   // ("Reagent");
    qDebug() << "Edit_Dialog::g_reagent = " << Edit_Dialog::g_reagent;

    Edit_Dialog::g_mw_string = model->record(g_selected_row).value("MW").toString();   // ("MW");
    Edit_Dialog::g_mw = model->record(g_selected_row).value("MW").toDouble();   // ("MW");
    qDebug() << "Edit_Dialog::g_mw = " << Edit_Dialog::g_mw;
    qDebug() << "Edit_Dialog::g_mw_string = " << Edit_Dialog::g_mw_string;
}


void Edit_Dialog::on_tableView_clicked(const QModelIndex &index)
{
    qDebug() << "Edit_Dialog::on_tableView_clicked(const QModelIndex &index)--index = " << index;
    Edit_Dialog::user_selected_index(index);
    qDebug() << "End of on_tableview_clicked()";
}


