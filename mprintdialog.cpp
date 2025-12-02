#include <QPrinter>
#include <QPrintDialog>
#include <QtWidgets>
#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QTextEdit>
#include <QObject>
#include <QTextEdit>
#include "mprintdialog.h"
#include "ui_mprintdialog.h"

QString mPrintDialog::g_print_text = "Chemical Calculator Results: \n\n";

mPrintDialog::mPrintDialog(QWidget *parent)
    : QDialog(parent)
    , ui2(new Ui::mPrintDialog)
{
    ui2->setupUi(this);
    setWindowTitle("Print List - ChemCalc");
}

mPrintDialog::~mPrintDialog()
{
    delete ui2;
}

void mPrintDialog::on_pushButton_Cancel_clicked()
{
    close();
}

void mPrintDialog::on_pushButton_Print_clicked()
{
    //Note: This produces a pdf file if file is saved via print dialog.
    QPrinter my_printer = QPrinter();
    QPrintDialog printDialog(&my_printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        ui2->textEdit->document()->print(printDialog.printer());
    }
}

void mPrintDialog::on_pushButton_Save_clicked()
{
    //Put file SAVE code here. This is called from the printlist save pushbutton.
    qDebug("pushButton_Save called.");
    //Code to be able to select file name and path.
    //TODO - Add suggested default filename (ChemCalcSave.txt ??).
    //QString fileNameHint = "ChemCalc_Output.txt";
    QString print_save_path_filename = QFileDialog::getSaveFileName(this, tr("Save File"),"~/ChemCalcResults.txt");
    qDebug() << "print_save_path_filename= " << print_save_path_filename;

    //Try to create and open the file for writing.
    //Return on no file name error, and skip rest of code.
    if (print_save_path_filename.isEmpty()){
        return;
    }
    QFile mfile(print_save_path_filename);
    if(!mfile.open(QFile::WriteOnly | QFile::Text)){
        qDebug() << "Could not open file for text writing.";
        return;
    }
    QString final_text = ui2->textEdit->toPlainText();
    QTextStream out(&mfile);
    out << final_text;
    qDebug() << "Final text= " << final_text;
    mfile.flush();
    mfile.close();  //Close file - IMPORTANT!
}


//QString mPrintDialog::show_printList(QString print_list_text)
void mPrintDialog::show_printList(QString print_list_text)
{
    qDebug() << "'show_printList' called (in mprintdialog.cpp).";
    ui2->textEdit->setPlainText(print_list_text);
    qDebug() << "'In mPrintDialog::show_printList' After 'ui2->textEdit->setPlainText(print_list_text)' " ;
}

void mPrintDialog::on_mPrintDialog_finished(int result)
{
    //This is called automatically when the mPrintDialog dialog is closed by user.

    qDebug() << "In 'mPrintDialog_finished' STARTED.";
    QString final_text = ui2->textEdit->toPlainText();
    //Add a 'newline (\n)' at end of previous text.
    final_text.append( "\n");
    qDebug() << "In 'mPrintDialog_finished', 'final_text = " << final_text;
    mPrintDialog::g_print_text = final_text;
}


