
#include <QDialog>
#include <QObject>
#include <QTableView>
#include <QAbstractItemView>
#include <QWidget>
#include <QTableWidget>
#include <QSql>
#include <QSqlDriver>
#include <QSqlDatabase>
#include <QDialogButtonBox>
#include <QSqlRelationalTableModel>
#include <QAbstractTableModel>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include "edit_database.h"
#include "ui_edit_database.h"


   //QDialog *edb = new Edit_DataBase(this);
   //edb->open();




Edit_DataBase::Edit_DataBase(QWidget *parent)
    : QWidget(parent)
    , ui3(new Ui::Edit_DataBase)
{
    ui3->setupUi(Ui::Edit_DataBase())
    //ui3->setupUi(this);
}

Edit_DataBase::~Edit_DataBase()
{
    delete ui3;
}

/*
void edit_database()  // This function is usually called from another class "Ui_MainWindow".
{
  /*  qDebug() << "edit_database function called";
    //This (below) is not the database name, it's the DRIVER name.
    db = QtSql.QSqlDatabase.addDatabase("QSQLITE");
    // This (below) is the database file name.
    // set Database Name to ~.local/share/ChemCalc/ChemCalc_data.db" (in linux).
    db.setDatabaseName(g_user_data_file_path);
      if not db.open():
    ui.cc_msgBox("Unable to connect to the database")
    if g_debug_on: print("Unable to connect to the database")
                    sys.exit(-1)

                    self.model = QtSql.QSqlTableModel()
      self.model.setTable("USER")
      self.model.setEditStrategy(QSqlTableModel.OnManualSubmit)

      ## The sql syntax is different (truncated) from other times.
# sql = ("SELECT * FROM USER WHERE reagent LIKE '" + reagent_fragment +"%'")
# sql = ("reagent LIKE '" + reagent_fragment +"%'")
# model.setFilter(sql)
    self.model.select()
    if g_debug_on:   print("edit_database function-after line model.select")

                    ## Changed "view" to "tableView" in next group of lines.--------------------
                    ## See https://realpython.com/python-pyqt-database/
                                  self.tableView.setModel(self.model)
                                  self.tableView.setColumnWidth(0, 510)
                                  self.tableView.setColumnWidth(1,75)
                                  self.tableView.setWordWrap(True)
                                  ## This was moved to end to give time to process previous comands.
# self.tableView.resizeRowsToContents()
# self.tableView.resizeColumnToContents(1)
                                  self.tableView.setSortingEnabled(True)
                                  self.label.setText("Double-Click On Data To Edit Items In DataBase.")
# button_OK = self.buttonBox.button.Ok
# button_OK.setEnabled(False)

# Edit_Dialog.show()  ## Shows Edit_Dialog Window. Use next line (.exec) instead.
                                  ## 'Show' returns execution to main window immwdiately, while
        ## 'exec' waits until 'OK' or 'Cancel' are clicked.
           Edit_Dialog.exec()
# Keep next line after 'Edit_Dialog.exec()' so it works due to timing issues.
# If the tableView is not fully loaded it won't resize correctly.
           self.tableView.resizeRowsToContents()

}


void Edit_DataBase::accept()  // <----Slot-----
{
  /*  // Handles "OK' button click on Edit_Dialog box.
    qDebug() << "In Ui_Edit_Database class, def 'accept' called from Ok button";
    model.submitAll()
    //Close dialog box and continue from where 'Edit_Dialog.exec()' was called.
    // (Either 'edit_database' or 'select_from_database'.)
    //(Returns '1' which can be used in 'OK_clicked' variable.)
    Edit_Dialog.done(1);  // (0 = Cancel / 1 = OK)

}


void Edit_DataBase::reject()  // <----Slot-----
{
  /*  qDebug() << "In Ui_Edit_Database class, def 'reject' called from Cancel button";
    //Handles "Cancel' button click on Edit_Dialog box.
    model.revertAll();
    // Close dialog box and continue from where 'Edit_Dialog.exec()' was called.
    // (Either 'edit_database' or 'select_from_database'.)
    // (Returns '0' which can be used in 'OK_clicked' (=false) variable.)
    Edit_Dialog.done(0);  // (0 = Cancel / 1 = OK)

}
*/


