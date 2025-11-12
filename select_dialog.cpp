#include <QMap>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlTableModel>

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QAbstractTableModel>
#include <QPersistentModelIndex>
#include <QSqlError>
#include <QSqlField>
#include <QSqlIndex>
#include <QTableView>

#include <QDialog>
#include <QLabel>
#include <QMainWindow>
#include <QObject>
#include <QPrintDialog>
#include <QPrinter>
#include <QString>

#include <QFile>       //REMOVE?
#include <QFileDialog> //REMOVE?
#include <QLineEdit>
#include <QMessageBox>
#include <QRadioButton>
#include <QTextStream> //REMOVE?

#include <QDebug>
#include <QTextEdit>
#include <QVariant>

#include "mainwindow.h"
#include "select_dialog.h"
#include "ui_select_dialog.h"

Select_Dialog::Select_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui4(new Ui::Select_Dialog)
{
    ui4->setupUi(this);

    qDebug() << "Select_Dialog::Select_Dialog Constructor - STARTED";
    setWindowTitle("Select From Database - ChemCalc");

    qDebug() << "In select_dialog constructor, g_db.isValid() = " << MainWindow::g_db.isValid();
    if (!MainWindow::g_db.isValid()) {
        qDebug() << "In select_dialog -B";
        MainWindow::g_db = QSqlDatabase::addDatabase("QSQLITE");
    }
    MainWindow::g_db.setDatabaseName(MainWindow::g_dataBaseFilePath);

    bool DB_opened_OK = MainWindow::g_db.open();
    qDebug() << "DB_opend_OK = " << DB_opened_OK;
    if (DB_opened_OK == false) {
        MainWindow myObj;
        myObj.cc_msgBox("Unable to connect to the local database");
        qDebug() << "Unable to connect to the database";
        return;
    }
    model = new QSqlTableModel(this);
    model->setTable("USER");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    view = ui4->tableView;
    view->setModel(model);
    view->setColumnWidth(0, 510);
    view->setColumnWidth(1, 75);
    view->setWordWrap(true);
    view->setSortingEnabled(true);
    view->resizeRowsToContents();
    //-------------------------------
    qDebug() << "Select_Dialog Constructor - PARENT = " << parent;
    qDebug() << "Select_Dialog Constructor - THIS = " << this;

    qDebug() << "Select_Dialog Constructor - ENDED";
}

Select_Dialog::~Select_Dialog()
{
    delete ui4;
}

void Select_Dialog::on_pushButton_Cancel_clicked()
{
    qDebug() << "Select_Dialog CANCEL clicked.";
    // This works just like this.  Changed data rolls back if not submitted.
    close();
}

void Select_Dialog::on_pushButton_OK_clicked()
{
    close();
}

void Select_Dialog::on_tableView_clicked(const QModelIndex &index)
{
    // This function is a slot called whenever a user clicks on (selects)
    // a table item from a Select_Dialog box (signal) of
    // reagents and molecular weights. It sets LOCAL VARIABLES.
    // It sets 2 (Edit_Dialog) global or class-wide variables.

    qDebug() << "Select_Dialog::on_tableView_clicked(const QModelIndex &index)--index = " << index;

    //Use local variables in this function.
    int selected_row = index.row();
    qDebug() << "selected_row = " << selected_row;

    int selected_column = index.column();
    qDebug() << "selected_column = " << selected_column;

    QString mw = index.siblingAtColumn(1).data().toString();
    qDebug() << "mw = " << mw;         //MW as a QSTRING, local variable only.
    Edit_Dialog::g_mw = mw.toDouble(); //MW as a DOUBLE
    Edit_Dialog::g_mw_string = mw;     //MW as a String
    qDebug() << "Edit_Dialog::g_mw = " << Edit_Dialog::g_mw;
    qDebug() << "Edit_Dialog::g_mw_string = " << Edit_Dialog::g_mw_string;

    QString reagent = index.siblingAtColumn(0).data().toString();
    qDebug() << "Reagent = " << reagent;
    Edit_Dialog::g_reagent = reagent; // ("Reagent");
    qDebug() << "Edit_Dialog::g_reagent = " << Edit_Dialog::g_reagent;
    qDebug() << "End of on_tableview_clicked()";
}

int Select_Dialog::chem_db_read(QString reagent_fragment)
{
    // Finds and reads reagents and MW from database.
    // Data is selected in QSqlTableModel (*model).

    qDebug() << "STARTED Select_Dialog::chem_db_read()";
    // The sql syntax is different (truncated) from other times.
    // Don't use the long SQL syntax, use the short version below.
    // sql = ("SELECT * FROM USER WHERE reagent LIKE '" + reagent_fragment +"%'") //Long, incorrect version.
    QString sql = ("reagent LIKE '" + reagent_fragment
                   + "%'"); // Correct short version for sqlite and Qt.
    qDebug() << "sql = " << sql;
    model->setFilter(sql);
    model->select(); // Selects all data left after filter above.
    int mySize = model->rowCount();
    qDebug() << "Select_Dialog::chem_db_read()-MYSIZE = " << mySize;
    qDebug() << "ENDED Select_Dialog::chem_db_read()";
    //Do cc_msgBox info upon return to MainWindow (using int 'MySize').
    return mySize;
}
