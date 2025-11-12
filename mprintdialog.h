#ifndef mPRINTDIALOG_H
#define mPRINTDIALOG_H

#include <QDialog>
#include <QObject>
#include <QPrinter>
#include <QTextEdit>

namespace Ui {
class mPrintDialog;
}

class mPrintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit mPrintDialog(QWidget *parent = nullptr);
    ~mPrintDialog();

    // see https://stackoverflow.com/questions/23922319/global-variables-in-qt-5-3
    static QString g_print_text; // initial declaration
    void show_printList(QString print_list_text);

private slots:
    void on_pushButton_Cancel_clicked();
    void on_pushButton_Print_clicked();
    void on_pushButton_Save_clicked();
    void on_mPrintDialog_finished(int result);

private:
    Ui::mPrintDialog *ui2;
};

#endif // mPRINTDIALOG_H
