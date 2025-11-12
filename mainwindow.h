#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>

#include "edit_dialog.h"
#include "mprintdialog.h"
#include "preferences_dialog.h"
#include "qnetworkreply.h"
#include "qsqltablemodel.h"
#include "qtableview.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;

    friend class Select_Dialog;
    friend class Edit_Dialog;
    friend class Preferences_Dialog;

    void cc_msgBox(QString msgText,
                   QString msgText2 = "",
                   QString msgText3 = "",
                   MainWindow *parent = nullptr);

    void on_pushButton_DeleteRow_clicked();
    QString read_starting_reagent();
    int db_Save(QString reagent_name, QString mw, bool skipConfirmationMessage = false);

    QString UIDString;
    struct entrezData
    {
        QString WebEnv;
        QString QueryKey;
        QString UIDString;
    };

    bool g_strictSearch;
    static QString g_dataBaseFilePath;
    static QString g_configFilePath;
    static bool g_NetworkAccessPermission;
    static bool g_NetworkAccessAsked;
    static bool g_NetworkAccessDontAskAgain;
    static QSqlDatabase g_db;

    void entrezQuery1(QString myReagentText, bool strictSearch);
    void entrezQuery2(entrezData myEntrezDataStruct);

public slots:

signals:

private slots:

    void on_pushButton_Quit_clicked();
    void on_actionQuit_triggered();
    void on_pushButton_Show_clicked();
    void on_pushButton_Calculate_clicked();
    QString read_starting_radioButtons();

    QString read_desired_radioButtons();
    double read_density();
    double validate_raw_number(QString raw_num);
    double read_starting_conc();

    QString read_starting_suffix();
    double read_desired_conc();
    QString read_desired_suffix();
    QString read_total_amount_suffix();
    double read_total_amount();

    double read_MW();
    double sol_is_molar(QString str);
    bool is_desired_reagent_total_by_moles();
    bool is_desired_reagent_total_by_weight();
    double calc_solution_molarity(double starting_conc, QString starting_conc_suffix);

    double g_per_L(double starting_conc, QString starting_conc_suffix);
    double calc_moles(double input_num, QString suffix);
    double calc_grams(double input_num, QString suffix);
    double read_desired_volume();
    void format_ml_answer(double ml_needed);

    void format_gram_answer(double grams_needed);
    void write_answer(QString text);
    QString on_pushButton_AddToPrintList_clicked();
    QString read_result();

    void on_pushButton_FindMW_clicked();
    void on_pushButton_SaveMW_clicked();
    void on_actionDatabase_triggered();
    void on_actionChemCalc_Help_triggered();
    void on_actionAbout_ChemCalc_triggered();

    void on_textEdit_StartingReagent_cursorPositionChanged();
    void replyFinished1(QNetworkReply *reply1);
    void replyFinished2(QNetworkReply *reply2);
    entrezData processReturnedText1(QByteArray returnedText1);
    void processReturnedText2(QByteArray returnedText2);
    void on_actionEnterez_Search_triggered(bool strictSearch);

    void askToUseAndSave(QString chemNameReturned, QString molecularWeight);
    void on_actionOpen_PubChem_Website_triggered();
    void runAtStart();
    bool checkForDataBase();
    bool checkForConfigurationFile();

    static QString parseConfigFile(QString searchTerm, bool replace, bool replacementTerm);
    void setConfigGlobals();
    void on_actionPreferences_triggered();

private:
    mPrintDialog *pd;
    QSqlTableModel *model;
    QTableView *view;
};
#endif // MAINWINDOW_H
