#include <QButtonGroup>
#include <QDialog>
#include <QLabel>
#include <QPrintDialog>
#include <QPrinter>
#include <QString>
#include <QWidget>

#include <QIcon>
#include <QIconEngine>
#include <QMainWindow>
#include <QObject>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QTabWidget>
#include <QWindow> //??

#include <QDir>
#include <QFile>
#include <QLineEdit>
#include <QMessageBox>
#include <QRadioButton>
#include <QResource>

#include <QDebug>
#include <QFileDialog> //REMOVE?
#include <QTextDocument>
#include <QTextEdit>
#include <QTextStream> //REMOVE?

#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>

#include <QDesktopServices>
#include <QGuiApplication> //?
#include <QList>
#include <QProgressBar>
#include <QStandardPaths>
#include <QUrl>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlStreamEntityDeclaration>
#include <QXmlStreamReader>

#include <QDomAttr>
#include <QDomDocument>
#include <QDomElement>
#include <QDomEntity>
#include <QDomImplementation>
#include <QDomNode>
#include <QDomNodeList>

#include <QMutex>
#include <QSize>
#include <QString>
#include <QStringView>
#include <QTextBrowser>
#include <QTimer>

#include "./ui_mainwindow.h"
#include "about_dialog.h"
#include "edit_dialog.h"
#include "mainwindow.h"
#include "mprintdialog.h"

#include "preferences_dialog.h"
#include "select_dialog.h"
#include "ui_about_dialog.h"
#include "ui_select_dialog.h"

// Declare some global variables.--------------------
QString MainWindow::g_dataBaseFilePath = "";
QString MainWindow::g_configFilePath = "";

bool MainWindow::g_NetworkAccessPermission = true;
bool MainWindow::g_NetworkAccessAsked = false;
bool MainWindow::g_NetworkAccessDontAskAgain = false;
//---------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Chemical Calculator");

    // Runs once at start, to setup file locations, database, config files etc.
    runAtStart();

    // "g_db" is declared in main.cpp outside of any function.
    //--------------Copied from Edit_Dialog------
    qDebug() << "In MainWindow constructor-A";
    qDebug() << "In MainWindow constructor-A2, g_db.isValid() = " << g_db.isValid();
    if (!g_db.isValid()) {
        qDebug() << "In MainWindow constructor-A3";
        MainWindow::g_db = QSqlDatabase::addDatabase("QSQLITE");
    }
    // MainWindow::g_db = QSqlDatabase::addDatabase("QSQLITE");
    qDebug() << "In MainWindow constructor-B";
    // QString 'g_dataBaseFilePath' is static class scope,
    // declared in MainWindow header file.
    MainWindow::g_db.setDatabaseName(MainWindow::g_dataBaseFilePath);
    qDebug() << "In MainWindow constructor, g_dataBaseFilePath = "
             << MainWindow::g_dataBaseFilePath;

    if (!MainWindow::g_db.isOpen()) {
        bool DB_opened_OK = MainWindow::g_db.open();
        qDebug() << "DB_opend_OK = " << DB_opened_OK;
        if (DB_opened_OK == false) {
            qDebug() << "Unable to connect to the database";
            MainWindow myObj;
            qDebug() << "In MainWindow constructor-C";
            myObj.cc_msgBox("Unable to connect to the local database");
            return;
        }
    }
    qDebug() << "In MainWindow constructor-END";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::runAtStart()
{
    // Runs once at start, to setup file locations, database, config files etc.
    qDebug() << "runAtStart STARTED.";

    // 1) check for database.------
    bool dbOK = checkForDataBase();
    qDebug() << "dbOK = " << dbOK;
    if (!dbOK) {
        // Add error code here.
    }

    // 2) check for or make configuration file.------
    bool configOK = checkForConfigurationFile();
    qDebug() << "configOK = " << configOK;

    // ---For Testing----------
    // Remember the final space (for name search).
    // Only change last bool (value), first bool means replace.
    // parseConfigFile("NetworkAccessPermission ", true, true);
    // parseConfigFile("NetworkAccessAsked ", true, true);
    // parseConfigFile("NetworkAccessDontAskAgain ", true, true);
    //-----------------

    // 3) Set Configuration Globals.
    // This reads the configuration file and sets the following
    // three global variables:
    //   (bool) g_NetworkAccessDontAskAgain
    //   (bool) g_NetworkAccessAsked
    //   (bool) g_NetworkAccessPermission

    qDebug() << "In runAtStart A g_NetworkAccessPermission = " << g_NetworkAccessPermission;
    qDebug() << "In runAtStart A g_NetworkAccessAsked = " << g_NetworkAccessAsked;
    qDebug() << "In runAtStart A g_NetworkAccessDontAskAgain = " << g_NetworkAccessDontAskAgain;

    if (configOK) {
        setConfigGlobals();
    }

    // 4) Ask for network access permission, if needed.

    qDebug() << "In runAtStart B g_NetworkAccessPermission = " << g_NetworkAccessPermission;
    qDebug() << "In runAtStart B g_NetworkAccessAsked = " << g_NetworkAccessAsked;
    qDebug() << "In runAtStart B g_NetworkAccessDontAskAgain = " << g_NetworkAccessDontAskAgain;

    if ((g_NetworkAccessAsked == false)
        or ((g_NetworkAccessDontAskAgain == false) and (g_NetworkAccessPermission == false))) {
        // Ask for permission to access PubChem via internet.
        // If permission granted set all 3 globals aove to true.
        // Use a timer to wait 1000ms so MainWindow constructor completes.
        QTimer::singleShot(1000, this, &MainWindow::on_actionPreferences_triggered);
    }
    setConfigGlobals();
    qDebug() << "In runAtStart C g_NetworkAccessPermission = " << g_NetworkAccessPermission;
    qDebug() << "In runAtStart C g_NetworkAccessAsked = " << g_NetworkAccessAsked;
    qDebug() << "In runAtStart C g_NetworkAccessDontAskAgain = " << g_NetworkAccessDontAskAgain;

    /*--------Future Use-------
    // Testing retriving maps.
    // suffix_molar_map_A2 =  {{"M",1}, {"mM",1e3}, {"uM",1e6}, {"nM",1e9},
    // {"pM",1e12}, {"fM",1e15}};
    QString myMapString =
        parseConfigFile("suffix_molar_map_A2", false, "CB_TEST");
    // Replace
    myMapString = "{{'M',1}, {'mM',1e3}, {'uM',1e6}, {'nM',1e9}, {'pM',1e12}, {'fM',1e15}}";
    qDebug() << "myMapString = " << myMapString;

    QList<QString> myMapList(myMapString);
    qDebug() << "myMapList = " << myMapList;
    QMap<QString, double> myMap_A2;
    // myMap_A2.insert(myMapList);
    // for ( ;  begin !=  end ; ++ begin )
     qDebug() << "myMap_A2 = " << myMap_A2;
     //----------------
    */
}

bool MainWindow::checkForDataBase()
{
    // Checks for existence of ChemCalc_data.db and paths to it.
    // If they don't exist it tries to create any needed paths/files.
    // Returns false if it couldn't be created.  true= OK to use.

    //--Returns 1 directory (as QString) OR an empty string. Use for DATA and DATABASE.
    // See Qt QStandardPaths Class docs.
    // Returns 1 directory OR an empty string, but is writable if returned.
    // ("/home/chris/.local/share/ChemCalc" on my computer.)
    // It may need to be created, but is writable.
    QString myAppDataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    qDebug() << "myAppDataLocation (writableLocation) = " << myAppDataLocation;
    // If data directories don't exist, create them.
    if (!QDir(myAppDataLocation).exists()) {
        bool dirOK = QDir().mkpath(myAppDataLocation);
        if (!dirOK) {
            qDebug() << "Could not create DATA DIRECTORY PATH!";
            // Add error code here?
            return false;
        }
    }

    // QString 'g_dataBaseFilePath' is static class scope, declared in header file.
    g_dataBaseFilePath = myAppDataLocation + "/ChemCalc_data.db";
    QFile myDB = QFile(g_dataBaseFilePath);
    // If database doesn't exist in users file system, create by copying resource db.
    if (!myDB.exists()) {
        // Copy nearly empty database in resource file to user's file system.
        // resource file = :/CC/DB1/ChemCalc_data.db  (notice ":" at start).
        QFile resourceFileDB;
        resourceFileDB.setFileName(":/CC/DB1/ChemCalc_data.db");
        bool fileCopyOK = resourceFileDB.copy(g_dataBaseFilePath);
        if (fileCopyOK) {
            // Set permissions.
            int myPermissions = (QFileDevice::ReadOwner | QFileDevice::WriteOwner
                                 | QFileDevice::ReadUser | QFileDevice::WriteUser);
            bool permissionsSetOK = myDB.setPermissions(QFileDevice::Permissions(myPermissions));
            qDebug() << "In checkForDatabase(), permissionsSetOK = " << permissionsSetOK;
            return true;
        }
        if (!fileCopyOK) {
            qDebug() << "Database file could not be created.";
            // Maybe add error code here.
            return false;
        }
    }
    return true;
}

bool MainWindow::checkForConfigurationFile()
{
    // Checks for existence of ChemCalc.conf and paths to it.
    // ("$HOME/.config/ChemCalc" on my linux computer.)
    // If they don't exist it tries to create any needed paths/files.
    // Returns false if it couldn't be created.  true= OK to use.

    //--Returns 1 directory (as QString) OR an empty string. Use for CONFIG.
    // See Qt QStandardPaths Class docs.
    // Returns 1 directory OR an empty string, but is writable if returned.
    // It may need to be created, but is writable.
    QString myConfigLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    qDebug() << "myConfigLocation (writableLocation) = " << myConfigLocation;

    // If config directories don't exist, create them.
    if (!QDir(myConfigLocation).exists()) {
        bool dirOK = QDir().mkpath(myConfigLocation);
        if (!dirOK) {
            qDebug() << "Could not create CONFIGURATION DIRECTORY PATH!";
            // Add error code here?
            return false;
        }
    }

    // QString 'g_configFilePath' is static class scope, declared in header file.
    g_configFilePath = myConfigLocation + "/ChemCalc.conf";
    QFile myConfig = QFile(g_configFilePath);
    // If configuration file doesn't exist in user's file system, create
    // by copying resource configuration.
    if (!myConfig.exists()) {
        // Copy configuration in resource file to user's file system.
        // resource file = :/ChemCalc.conf  (notice ":" at start).
        QFile resourceFileConfig;
        resourceFileConfig.setFileName(":/ChemCalc.conf");
        bool fileCopyOK = resourceFileConfig.copy(g_configFilePath);
        if (fileCopyOK) {
            // Set permissions.
            int myPermissions = (QFileDevice::ReadOwner | QFileDevice::WriteOwner
                                 | QFileDevice::ReadUser | QFileDevice::WriteUser);
            bool permissionsSetOK = myConfig.setPermissions(QFileDevice::Permissions(myPermissions));

            // Don't use QSettings as it mangles (deletes) comments.
            // Also set g_ccSettings (to use QSettings methods).
            // QSettings g_ccSettings(g_configFilePath, QSettings::IniFormat);
            // testing-------------<<--------FINISH HERE--------------------------
            // g_ccSettings.beginGroup("FirstRun");
            // g_ccSettings.setValue("myQString1", "Molar");
            // g_ccSettings.setValue("myQString2", "Micro-Molar");
            // g_ccSettings.endGroup();
            //--end testing.----------
            return true;
        }
        if (!fileCopyOK) {
            qDebug() << "Configuration file could not be created.";
            // Maybe add error code here.
            return false;
        }
    }
    return true;
}

QString MainWindow::parseConfigFile(QString searchTerm,
                                    bool replace = false,
                                    bool replacementTerm = true)
{
    // NOTE: WORKS, BUT VERY UGLY CODE!
    // ---Single argumnet mode (find)------
    // Parse ($HOME/.config/ChemCalc/ChemCalc.conf) for searchTerm.
    // Returns QString of line after equals sign of matching searchTerm,
    // or "NOT FOUND" on error or not found.
    //
    // ----Triple argument mode (find and replace)------
    // If "replace" is set to true then the "searchTerm" is replaced by
    // "replaceTerm" in the user's config file (and returns "replaceTerm).
    //
    // These 3 (bool) items (Return QString of "True" or "False", which
    // must be converted to bool (true or false) lower case after return.
    // 1) g_NetworkAccessPermission  (in file: "NetworkAccessPermission = False").
    // 2) g_NetworkAccessAsked
    // 3) g_NetworkAccessDontAskAgain

    QFile file(g_configFilePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        return QString("FILE OPEN ERROR-NOT FOUND"); //Return if file open error.
    }

    while (!file.atEnd()) {
        int lineStartPosition = file.pos();
        QString line = file.readLine();
        // do pre-processing
        // Skip comment lines (# and //) and newlines (\n).
        if (!(line.startsWith("#")) and (!line.startsWith("\n")) and (!line.startsWith("//"))) {
            // Trim trailing newlines ("\n").
            if (line.endsWith("\n")) {
                line.removeLast(); // Note that "\n" counts as one character.
            }
            // Check for equals sign.
            if (line.contains("=")) {
                // Do processing.
                // qDebug() << "In parseConfigFile: Config File Pos/Line = " << \
                    lineStartPosition << line;
                if (line.contains(searchTerm)) {
                    qDebug() << "In parseConfigFile,Found Search Term: " << searchTerm
                             << "at position " << file.pos();
                    // Replace term if called.
                    if (replace == true) {
                        file.seek(lineStartPosition + line.indexOf("=") + 1);
                        if (replacementTerm == true) {
                            file.write(" True \n");
                            return "True";
                        }
                        // Is false:
                        else {
                            file.write("False \n");
                            return "False";
                        }
                    }

                    // Get text after equals sign
                    QString answer = line.section("=", 1);
                    return answer; //return answer
                }
            }
        }
    }
    // If everything fails return "NOT FOUND".
    return QString("In parseConfigFile: NOT FOUND-B"); //Return if file open error.
}

void MainWindow::setConfigGlobals()
{
    // Parse configuration file for 3 network access variables.
    // (g_NetworkAccessPermission, g_NetworkAccessAsked etc.)
    // Note: Append space to searchTerm to prevent longer matching strings.
    QString text1 = parseConfigFile("NetworkAccessPermission ");
    QString text2 = parseConfigFile("NetworkAccessAsked ");
    QString text3 = parseConfigFile("NetworkAccessDontAskAgain ");
    qDebug() << "NetworkAccessPermission = " << text1;
    qDebug() << "NetworkAccessAsked = " << text2;
    qDebug() << "NetworkAccessDontAskAgain = " << text3;

    //-------------
    if (text1.contains("True", Qt::CaseInsensitive)) {
        g_NetworkAccessPermission = true;
        qDebug() << "In setConfigGlobals Q: g_NetworkAccessPermission = "
                 << g_NetworkAccessPermission;
    } else {
        // Anything except "True" or "true" is false by default (inc. null).
        g_NetworkAccessPermission = false;
        qDebug() << "In setConfigGlobals R: g_NetworkAccessPermission = "
                 << g_NetworkAccessPermission;
    }
    //-------------
    if (text2.contains("True", Qt::CaseInsensitive)) {
        g_NetworkAccessAsked = true;
    } else {
        // Anything except "True" or "true" is false by default (inc. null).
        g_NetworkAccessAsked = false;
    }
    //----------
    if (text3.contains("True", Qt::CaseInsensitive)) {
        g_NetworkAccessDontAskAgain = true;
    } else {
        // Anything except "True" or "true" is false by default (inc. null).
        g_NetworkAccessDontAskAgain = false;
    }
    //----------
}

void MainWindow::on_pushButton_Quit_clicked()
{
    QApplication::quit();
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionChemCalc_Help_triggered()
{
    // Display ChemCalc Help - Quick Start.
    qDebug() << "MainWindow Menu ChemCalc HELP triggered.";
    QTextBrowser *ccHelp = new QTextBrowser(this);
    ccHelp->setWindowFlag(Qt::Window);
    ccHelp->setWindowModality(Qt::NonModal);
    ccHelp->setSource(QUrl("qrc:/ChemCalc_help1.md"));
    ccHelp->resize(500, 700); // WxH adjust W for width of images + 40.
    ccHelp->show();
}

void MainWindow::on_actionAbout_ChemCalc_triggered()
{
    qDebug() << "MainWindow Menu ABOUT-ChemCalc triggered.";

    about_Dialog *myDialog = new about_Dialog(this);
    // Set first tab as current tab when opening.
    myDialog->ui->tabWidget->setCurrentWidget(myDialog->ui->tab);
    myDialog->open();
}

void MainWindow::cc_msgBox(QString msgText, QString msgText2, QString msgText3, MainWindow *parent)
{
    //This creates a modal message box with a message and OK button.
    QMessageBox msgBox;

    qDebug() << "cc_msgBox called";
    qDebug() << "This = " << this;
    qDebug() << "Parent = " << parent;

    if (msgText == "")
        msgText = "ERROR-No text in message box.";
    msgBox.setText(msgText);
    msgBox.setInformativeText(msgText2);
    msgBox.setDetailedText(msgText3);
    msgBox.setParent(parent);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setWindowTitle("ChemCalc");
    msgBox.exec();
}

int MainWindow::db_Save(QString reagent_name, QString mw_string, bool skipConfirmationMessage)
{
    // This is mostly used for 'Save MW' button.
    // If no matches in db (database), insert new row of data.
    // If 1 or more matches in db, notify and return with db unchanged.

    qDebug() << "In db_Save - STARTED";
    qDebug() << "In db_Save, g_db.isValid() = " << g_db.isValid();
    if (!g_db.isValid()) {
        qDebug() << "In db_save -B";
        MainWindow::g_db = QSqlDatabase::addDatabase("QSQLITE");
    }

    //g_db= QSqlDatabase::addDatabase("QSQLITE");
    // "g_db" is global from MainWindow header file.
    g_db.setDatabaseName(g_dataBaseFilePath);
    qDebug() << "In db_Save - B";

    if (!g_db.isOpen()) {
        qDebug() << "In db_Save - C";
        bool DB_opened_OK = g_db.open();
        qDebug() << "DB_opend_OK = " << DB_opened_OK;
        if (DB_opened_OK == false) {
            MainWindow::cc_msgBox("Unable to connect to the database", "", "", this);
            qDebug() << "Unable to connect to the database";
            return -2; //An error condition.
        }
    } else
        qDebug() << "In db_Save - D";

    qDebug() << "In db_Save - E";
    // Correct short version for sqlite and Qt.
    QString sql = ("reagent LIKE '" + reagent_name + "'"); // Query needs an exact match.
    qDebug() << "sql = " << sql;
    QSqlTableModel *model;
    model = new QSqlTableModel;
    model->setTable("USER");
    model->setFilter(sql);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    int myNumRows = model->rowCount();
    qDebug() << "myNumRows = " << myNumRows;

    // If no matches in db, insert new row of data.
    if (myNumRows == 0) {
        // Correct syntax example:
        // query.exec("INSERT INTO USER ('reagent', 'mw') VALUES ('newreagent2', '333.33')");   //THIS WORKS!!!
        QString sql2 = ("INSERT INTO USER ('reagent', 'mw') VALUES ('" + reagent_name + "', '"
                        + mw_string + "')");
        QSqlQuery query;
        query.exec(sql2);
        // Confirmation of  row addition, skip if 'skipConfirmationMessage' == true, default is 'false'.
        if (skipConfirmationMessage == false) {
            cc_msgBox("Reagent and molecular weight were saved to database.",
                      "Use Edit-Database (in Menu) to edit.",
                      "",
                      this);
        }
        // Maybe add a cancel procedure?
    }
    // If 1 or more matches in db, notify and return with db unchanged.
    if (myNumRows >= 1) {
        // Notification that (at least one) exact matching reagent exists in databaseb.
        if (skipConfirmationMessage == false) {
            cc_msgBox("There is already an exact match for this reagent in the database.",
                      "No changes were made to the databse.",
                      "Use Edit-Database (in Menu) to edit this reagent.",
                      this);
        }
    }
    g_db.close();
    return myNumRows;
}

void MainWindow::on_pushButton_Show_clicked()
{
    // Shows 'print list' (Dialog Box).
    // mPrintDialog *pd is declared in header file (MainWindow.h).
    pd->show();
}

QString MainWindow::read_starting_radioButtons()
{
    /*Reads the 3 Starting Reagent radioButtons and returns
    a string; either "solution", solid" or "liquid".*/

    bool starting_solution_checked = ui->radioButton_StartingSolution->isChecked();
    bool starting_solid_checked = ui->radioButton_Solid->isChecked();
    bool starting_liquid_checked = ui->radioButton_Liquid->isChecked();

    if (starting_solution_checked)
        return ("solution");
    if (starting_solid_checked)
        return ("solid");
    if (starting_liquid_checked)
        return ("liquid");
    else
        return ("Error in 'read_starting_radiobuttons function.");
}

QString MainWindow::read_desired_radioButtons()
{
    /*Reads the 2 Desired Reagent radioButtons and returns
    a string; either "solution", or "total_amount".*/

    bool desired_solution_checked = ui->radioButton_DesiredSolution->isChecked();
    bool desired_total_amount_checked = ui->radioButton_TotalAmount->isChecked();

    if (desired_solution_checked)
        return ("solution");
    if (desired_total_amount_checked)
        return ("total_amount");
    else
        return ("Error in 'read_starting_radiobuttons function.");
}

double MainWindow::validate_raw_number(QString raw_num)
{
    //This function converts text input to double numbers,
    //and checks for >= 0. Function returm a positive float or -1 on error.

    qDebug() << "Running 'validate_raw_number' function with raw_num = " << raw_num;
    QDoubleValidator val;
    val.setDecimals(3);
    val.setLocale(QLocale::C);
    val.setNotation(QDoubleValidator::StandardNotation);
    val.fixup(raw_num); // input is a QString, out is double
    bool ok;            //return if conversion is ok in next line.
    double validated_num = raw_num.toDouble(&ok);
    qDebug() << "Running 'validate_raw_number' bool VAL OK = " << ok;
    qDebug() << "Running 'validate_raw_number' function RETURNED double = " << validated_num;
    if (ok and (validated_num >= 0.0)) {
        return validated_num;
    } else {
        cc_msgBox("Number validation error.", "", "", this);
        return -1;
    }
}

double MainWindow::read_starting_conc()
{
    QString raw_starting_solution = ui->lineEdit_StartingSolution->text();
    double validated_num = validate_raw_number(raw_starting_solution);
    return (validated_num);
}

QString MainWindow::read_starting_suffix()
{
    QString starting_solution_suffix = ui->comboBox_StartingSolutionSuffix->currentText();
    return (starting_solution_suffix);
}

double MainWindow::read_desired_conc()
{
    QString raw_desired_solution = ui->lineEdit_DesiredSolution->text();
    double validated_num = validate_raw_number(raw_desired_solution);
    return (validated_num);
}

QString MainWindow::read_desired_suffix()
{
    QString desired_solution_suffix = ui->comboBox_DesiredSolutionSuffix->currentText();
    return (desired_solution_suffix);
}

double MainWindow::calc_solution_molarity(double starting_conc, QString starting_conc_suffix)
{
    //This function combines input molarity (NO BY WEIGHT) and suffix and returns a double.
    QMap<QString, double> suffix_molar_map_A2
        = {{"M", 1}, {"mM", 1e3}, {"uM", 1e6}, {"nM", 1e9}, {"pM", 1e12}, {"fM", 1e15}};
    //double suffix_conversion_factor = suffix_molar_map_A2[starting_conc_suffix];
    double molarity = starting_conc / suffix_molar_map_A2[starting_conc_suffix];
    qDebug() << "calc_solution_molarity returns: " << molarity;
    return molarity;
}

double MainWindow::g_per_L(double starting_conc, QString starting_conc_suffix)
{
    // This Function takes raw conentrations and conc suffix,
    // and returns the (by weight) concentration in g/L.

    QMap<QString, double> suffix_weight_per_liter_map_A3 = {{"%", 100},
                                                            {"g/L", 1e3},
                                                            {"mg/L", 1e6},
                                                            {"ug/L", 1e9},
                                                            {"ng/L", 1e12},
                                                            {"pg/L", 1e15},
                                                            {"fg/L", 1e18}};
    double final_conc = ((starting_conc / suffix_weight_per_liter_map_A3[starting_conc_suffix])
                         * 1000);
    qDebug() << "conc, in g_per_L = " << final_conc;
    return final_conc;
}

double MainWindow::read_MW()
{
    QString raw_mw = ui->lineEdit_MW->text();
    double mw = validate_raw_number(raw_mw);
    qDebug() << "raw_mw, (validated) mw = " << raw_mw << mw;
    return (mw);
}

double MainWindow::read_density()
{
    QString raw_density = ui->lineEdit_Density->text();
    double density = validate_raw_number(raw_density);
    qDebug() << "raw_density, density = " << raw_density << density;
    return (density);
}

double MainWindow::sol_is_molar(QString str)
{
    //This returns 0 if NOT molar, or the conversion factor if found.
    // Remember (0=false, 1=true, any non-zero=true?)
    QMap<QString, double> g_suffix_molar_map_A2
        = {{"M", 1}, {"mM", 1e3}, {"uM", 1e6}, {"nM", 1e9}, {"pM", 1e12}, {"fM", 1e15}};
    QString myStr = str;
    double retVal;
    if (g_suffix_molar_map_A2.contains(myStr))
        return g_suffix_molar_map_A2.value(myStr);
    else
        return (0.0);
}

double MainWindow::read_total_amount()
{
    // This returns a double of the total amount box.
    if (ui->radioButton_TotalAmount->isChecked()) {
        QString raw_total_amount = ui->lineEdit_TotalAmount->text();
        double total_amount = validate_raw_number(raw_total_amount);
        return total_amount;
    } else {
        qDebug() << "ERROR-read_total_amount was called but the Total Amount RadioButton is not "
                    "checked.";
        return 0; //An ERROR condition
    }
}

QString MainWindow::read_total_amount_suffix()
{
    // This returns a QString of the total amount suffix comboBox.
    if (ui->radioButton_TotalAmount->isChecked()) {
        QString total_amount_suffix = ui->comboBox_TotalAmountSuffix->currentText();
        return total_amount_suffix;
    } else {
        qDebug() << "ERROR-read_total_amount_SUFFIX was called but the Total Amount RadioButton is "
                    "not checked.";
        return 0; //An ERROR condition
    }
}

bool MainWindow::is_desired_reagent_total_by_moles()
{
    // This return bool (true or false) if desired reagent is total by moles.

    QMap<QString, double> total_amount_suffix_map_moles_C2 = {{"moles", 1},
                                                              {"millimoles", 1e3},
                                                              {"micromoles", 1e6}};
    //QMap<QString, double> total_amount_suffix_map_weight_C3 = {{"g",1}, {"mg",1e3}, {"ug",1e6}};
    if (ui->radioButton_TotalAmount->isChecked()) {
        QString total_amount_suffix = ui->comboBox_TotalAmountSuffix->currentText();
        qDebug() << "In is_desired_reagent_total_by_moles()"
                 << "Total_amount_suffix = " << total_amount_suffix;
        if (total_amount_suffix_map_moles_C2.contains(total_amount_suffix))
            return true; //TODO FIX HERE
        else
            return false;
    } else {
        qDebug() << "ERROR-read_total_amount_SUFFIX was called but the Total Amount RadioButton is "
                    "not checked.";
        return false; //An ERROR condition
    }
}

bool MainWindow::is_desired_reagent_total_by_weight()
{
    // This return bool (true or false) if desired reagent is total by WEIGHT.

    QMap<QString, double> total_amount_suffix_map_weight_C3 = {{"g", 1}, {"mg", 1e3}, {"ug", 1e6}};
    if (ui->radioButton_TotalAmount->isChecked()) {
        QString total_amount_suffix = ui->comboBox_TotalAmountSuffix->currentText();
        qDebug() << "In is_desired_reagent_total_by_weight()"
                 << "Total_amount_suffix = " << total_amount_suffix;
        if (total_amount_suffix_map_weight_C3.contains(total_amount_suffix))
            return true; //TODO FIX HERE
        else
            return false;
    } else {
        qDebug() << "ERROR-read_total_amount_SUFFIX was called but the Total Amount RadioButton is "
                    "not checked.";
        return false; //An ERROR condition
    }
}

double MainWindow::calc_moles(double input_num, QString suffix)
{ //This function combines input moles and suffix and returns a double.

    QMap<QString, double> total_amount_suffix_map_moles_C2 = {{"moles", 1},
                                                              {"millimoles", 1e3},
                                                              {"micromoles", 1e6}};
    double suffix_conversion_factor = total_amount_suffix_map_moles_C2[suffix];
    double moles = (input_num / suffix_conversion_factor);
    return moles;
}

double MainWindow::calc_grams(double input_num, QString suffix)
{ //This function combines input grams and suffix and returns a double.

    QMap<QString, double> total_amount_suffix_map_weight_C3 = {{"g", 1}, {"mg", 1e3}, {"ug", 1e6}};
    double suffix_conversion_factor = total_amount_suffix_map_weight_C3[suffix];
    double grams = (input_num / suffix_conversion_factor);
    return grams;
}

double MainWindow::read_desired_volume()
{
    //This reads the desired volume and returns (a double) desired volume in liters.
    QMap<QString, double> volume_suffix_map_B2 = {{"ml", 1e3}, {"L", 1}, {"ul", 1e6}};

    QString raw_desired_volume = ui->lineEdit_DesiredVolume->text();
    double desired_volume = validate_raw_number(raw_desired_volume);
    QString desired_volume_suffix = ui->comboBox_DesiredVolumeSuffix->currentText();
    // Convert desired volume to liters.

    qDebug() << "volume_suffix_map_B2[desired_volume_suffix] = "
             << volume_suffix_map_B2[desired_volume_suffix];
    qDebug() << "desired_volume = " << desired_volume;
    qDebug() << "desired_volume_suffix = " << desired_volume_suffix;

    double desired_volume_liters = (desired_volume / volume_suffix_map_B2[desired_volume_suffix]);
    qDebug() << "raw_desired_volume = " << raw_desired_volume;
    qDebug() << "final_desired_volume (in L) = " << desired_volume_liters;
    return desired_volume_liters;
}

void MainWindow::format_gram_answer(double grams_needed)
{
    // This checks and formats the answer, and sends it to write_answer function.

    // Initialize some variables.
    double amt_needed = 0.0;
    QString amt_suffix;
    QString formatted_answer;
    QString str;

    qDebug() << "Started: format_gram_answer";
    if (grams_needed >= 1000) { // use kilograms (kg) in final answer.
        amt_needed = (grams_needed / 1000);
        amt_suffix = "kg";
        // Last digit below (3) sets the number of digits to the right of the decimal point.
        str = QString("Use %1 kg").arg(amt_needed, 0, 'F', 3);
    }
    if ((grams_needed < 1000) and (grams_needed >= 1)) { // use grams (g) in final answer.
        amt_needed = (grams_needed);
        amt_suffix = "g";
        // Last digit below (3) sets the number of digits to the right of the decimal point.
        str = QString("Use %1 g").arg(amt_needed, 0, 'F', 3);
    }
    if (grams_needed < 1) { // use milligram (mg) in final answer.
        amt_needed = (grams_needed * 1000);
        amt_suffix = "mg";
        // Last digit below (3) sets the number of digits to the right of the decimal point.
        str = QString("Use %1 mg").arg(amt_needed, 0, 'F', 3);
    }
    //formatted_answer = ("Use " + str(amt_needed) + " " + amt_suffix);
    formatted_answer = str; // ("Use " << amt_needed << " " << amt_suffix);
    qDebug() << "formatted_answer = " << formatted_answer;
    write_answer(formatted_answer);
}

void MainWindow::format_ml_answer(double ml_needed)
{
    // This checks and formats the answer, and sends it to write_answer function.

    // Declare some variables.
    double desired_volume_liters;
    double amt_needed;
    QString amt_suffix;
    QString formatted_answer;
    QString str;

    qDebug() << "Started: format_ml_answer. input ml_needed = " << ml_needed;
    // Check if answer is larger than desired volume.
    desired_volume_liters = read_desired_volume();
    if (ml_needed > (desired_volume_liters * 1000) and (desired_volume_liters > 0.0)) {
        cc_msgBox("ERROR \n Calculated Volume needed is MORE than Desired Volume.",
                  "Your Desired Solution concentration may be more than your Starting Solution "
                  "concentration.",
                  "",
                  this);
    }
    // use liters in final answer.
    if (ml_needed >= 1000) {
        amt_needed = (ml_needed / 1000);
        // Last digit below (3) sets the number of digits to the right of the decimal point.
        str = QString("Use %1 L").arg(amt_needed, 0, 'F', 3);
        amt_suffix = "L";
    }
    // use ml in final answer.
    if ((ml_needed < 1000) and (ml_needed >= 1)) {
        amt_needed = (ml_needed);
        // Last digit below (3) sets the number of digits to the right of the decimal point.
        str = QString("Use %1 ml").arg(amt_needed, 0, 'F', 3);
        amt_suffix = "ml";
    }
    // use ul in final answer.
    if (ml_needed < 1) {
        amt_needed = (ml_needed) * 1000;
        // Last digit below (3) sets the number of digits to the right of the decimal point.
        str = QString("Use %1 ul").arg(amt_needed, 0, 'F', 3);
        amt_suffix = "ul";
    }
    formatted_answer = str; //("Use " + amt_needed + " " + amt_suffix);
    qDebug() << "formatted_answer = " << formatted_answer;
    write_answer(formatted_answer);
}

void MainWindow::write_answer(QString text)
{
    ui->textEdit_Results->clear();
    ui->textEdit_Results->setText(text);
}

QString MainWindow::read_result()
{
    QString result = ui->textEdit_Results->toPlainText();
    qDebug() << "read_result = " << result;
    return result;
}

QString MainWindow::read_starting_reagent()
{
    //Read Starting Reagent text and do some (minimal) processing.
    QString raw_starting_reagent = ui->textEdit_StartingReagent->toPlainText();
    qDebug() << "raw_starting_reagent = " << raw_starting_reagent;

    // If 'Starting Reagent' text box is empty, return "Unknown Reagent".
    qDebug() << "In read_starting_reagent, Starting Reagent box is empty.";
    if (raw_starting_reagent == "") {
        return "Unknown Reagent";
    }
    // On first click of 'Starting Reagent' text box, clear original text.
    qDebug() << "Clearing Starting Reagent defauilt text.";
    if (raw_starting_reagent == "Enter reagent name here.") {
        ui->textEdit_StartingReagent->setPlainText(""); //Clear text box.
        return "Unknown Reagent";
    }
    return raw_starting_reagent;
}

QString MainWindow::on_pushButton_AddToPrintList_clicked()
{
    /* This creates a text answer formated like:
    partA = "Use 32 grams of " or "Use 5 ml of 1 molar "
    partB = starting reagent
    partC = "(MW = 123)" (only if needed)
    partD = "to get "
    partE = "250 ml " or total amount + suffix
    partF = "of 10 mM " + starting reagent.
    */

    qDebug() << "'onPushButton_add_to_printList_clicked' called.";
    //Declare some variables upfront to avoid double declarations:
    QString partA = "";
    QString starting_reagent_text = "";
    QString partB = "";
    QString mw = "";
    QString partC = "";
    QString partD = "";
    QString desired_radioButton = "";
    QString raw_desired_volume = "";
    QString desired_volume_suffix = "";
    QString partE = "";
    QString desired_conc = "";
    QString desired_conc_suffix = "";
    QString partF = "";
    QString starting_radioButton = "";
    QString t_amount_num = "";
    QString t_amount_suffix = "";
    QString solution_A = "";
    QString solution_B = "";

    //------------------------
    //Call 'calculate_clicked' to recalculate current answer.
    qDebug() << "'calculate_clicked' called from 'onPushButton_add_to_printList_clicked' function";
    qDebug() << "Existing g_print_text = " << mPrintDialog::g_print_text;
    MainWindow::on_pushButton_Calculate_clicked();
    //-------------
    partA = MainWindow::read_result();
    starting_reagent_text = read_starting_reagent();
    if (starting_reagent_text == "") {
        starting_reagent_text = "reagent";
    };
    starting_reagent_text.truncate(80);
    partB = starting_reagent_text;
    mw = ui->lineEdit_MW->text();
    //Maybe setup a validator to check for empty MW text box.  TODO.
    partC += " (MW = ";
    partC += mw;
    partC += ") ";
    partD = "to get ";
    desired_radioButton
        = MainWindow::read_desired_radioButtons(); // either "solution" or "total_amount".
    if (desired_radioButton == "solution") {
        raw_desired_volume = ui->lineEdit_DesiredVolume->text();
        desired_volume_suffix = ui->comboBox_DesiredVolumeSuffix->currentText();
        partE = raw_desired_volume + " " + desired_volume_suffix + " ";
        desired_conc = ui->lineEdit_DesiredSolution->text();
        desired_conc_suffix = MainWindow::read_desired_suffix();
        partF = "of " + desired_conc + " " + desired_conc_suffix + " " + partB;
    }
    if (desired_radioButton == "total_amount") {
        t_amount_num = ui->lineEdit_TotalAmount->text();          //a QString.
        t_amount_suffix = MainWindow::read_total_amount_suffix(); //a QString
        partE += t_amount_num;
        partE += " ";
        partE += t_amount_suffix;
        partE += " ";
        partF = "of " + partB;
    }
    //Next line returns either "solution", solid" or "liquid".
    starting_radioButton = MainWindow::read_starting_radioButtons();
    if (starting_radioButton == "solution") {
        solution_A = ui->lineEdit_StartingSolution->text(); // QString of molar number
        solution_B = MainWindow::read_starting_suffix();    // molar suffix
        partA += " of ";
        partA += solution_A;
        partA += " ";
        partA += solution_B;
        partA += " ";
    }
    if (starting_radioButton == "solid") {
        //append " of " to partA via QString append command.
        partA += " of ";
    }
    if (starting_radioButton == "liquid") {
        partA += " of ";
    }
    QString answer_text_string = (partA + partB + partC + partD + partE + partF + ".\n\n");
    qDebug() << "In on_pushButton_AddToPrintList_clicked, answer_text_string = "
             << answer_text_string;
    mPrintDialog::g_print_text += answer_text_string;
    qDebug() << "In on_pushButton_AddToPrintList_clicked, FINAL mPrintDialog::g_print_text = "
             << mPrintDialog::g_print_text; //answer_text_string;
    //----------------------
    pd = new mPrintDialog(this); //Works but may not be correct.
    pd->open();

    MainWindow::pd->show_printList(mPrintDialog::g_print_text);
    return ("Returned Text Goes Here"); // returned_text;
}

void MainWindow::on_pushButton_Calculate_clicked()
{
    //Start calculating chemical amounts, etc.
    // This function runs every time the "Calculate" button is clicked.

    //Clear results area.
    ui->textEdit_Results->clear();

    //Initalize these variables.
    //bool sol_is_molar; //This is a function.
    bool starting_solution_is_weight = false;
    bool starting_solution_is_molar = false;
    bool starting_reagent_is_solid = false;
    bool starting_reagent_is_liquid = false;
    bool desired_solution_is_molar = false;
    bool desired_solution_is_weight = false;
    bool desired_reagent_is_total_by_moles = false;
    bool desired_reagent_is_total_by_weight = false;
    bool skipMW = false;
    double total_amount = 0.0;
    QString total_amount_suffix = "CB-error";
    double desired_volume_liters = 0.0;
    double calculated_desired_moles = 0.0;
    double desired_molarity = 0.0;
    double desired_conc = 0.0;
    double starting_conc = 0.0;
    double ml_needed = 0.0;
    double grams_needed = 0.0;
    double starting_molarity = 0.0;
    double desired_moles = 0.0;
    double desired_grams = 0.0;
    double desired_g_per_L = 0.0;
    double mw = 0.0;
    double calculated_starting_molarity = 0.0;
    double starting_g_per_L = 0.0;
    //double liquid_g_per_L = 0.0;  // Use density instead. -DELETE?
    double density = 0.0;
    QString starting_radio_button_selected = "NONE";
    QString desired_radio_button_selected = "NONE";
    QString starting_conc_suffix = "NONE";
    QString desired_conc_suffix = "NONE";

    // Read starting reagent radio buttons.
    starting_radio_button_selected = MainWindow::read_starting_radioButtons();
    qDebug() << "starting_radio_button_selected = " << starting_radio_button_selected;
    // Read desired reagent radio buttons.
    desired_radio_button_selected = MainWindow::read_desired_radioButtons();
    qDebug() << "desired_radio_button_selected = " << desired_radio_button_selected;

    /*Sets the following variables:  --------------------------------

    (Starting Reagents-sets 1):                 (Desired Reagents-sets 1):
    1) starting_solution_is_molar*              A) desired_solution_is_molar*
    2) starting_solution_is_weight              B) desired_solution_is_weight
    3) starting_reagent_is_solid (by weight)    C) desired_reagent_is_total_by_moles*
    4) starting_reagent_is_liquid (by weight)   D) desired_reagent_is_total_by_weight

    -This means 16 different combinations are needed (labeled 1A to 4D).
    -The following 8 are molar-molar combinations, no MW needed:
        1A, 1C, 2B, 2D, 3B, 3D, 4B, 4D
    -----------------------------------------------------------------*/

    // Starting Reagent 1 and 2 above.
    // STARTING reagent is a SOLUTION.
    if (starting_radio_button_selected == "solution") {
        starting_conc = read_starting_conc();
        starting_conc_suffix = read_starting_suffix();

        //This returns the conversion factor or 0 if not found --NO ???.
        if (sol_is_molar(starting_conc_suffix) != 0) {
            starting_molarity = calc_solution_molarity(starting_conc, starting_conc_suffix);
            starting_solution_is_molar = true;
            starting_solution_is_weight = false;
            qDebug() << "The starting Solution is Molar. "
                     << "The start suffix = " << starting_conc_suffix;
            qDebug() << "The starting molarity = " << starting_molarity;
        }

        else {
            qDebug() << "The starting Solution is by weight. ";
            starting_solution_is_weight = true;
            starting_solution_is_molar = false;
            starting_g_per_L = g_per_L(starting_conc, starting_conc_suffix);
            qDebug() << "starting_g_per_L = " << starting_g_per_L;
            mw = read_MW();
            starting_molarity = (starting_g_per_L / mw);
            qDebug() << "starting_molarity via g/L = " << starting_molarity;
            qDebug() << "starting_conc and suffix = " << starting_conc << starting_conc_suffix;
        }
    }
    // Starting Reagent 3 above.
    //Starting is a solid. No other info needed here (ex. MW).
    if (read_starting_radioButtons() == "solid") {
        starting_reagent_is_solid = true;
        qDebug() << "Starting reagent is a SOLID.";
    }
    // Starting Reagent 4 above.
    // If STARTING reagent is a LIQUID.
    if (read_starting_radioButtons() == "liquid") {
        density = read_density();
        starting_reagent_is_liquid = true;
        qDebug() << "Starting reagent is a LIQUID. density =" << density;
    }
    //--------------------------------------------
    // Desired Reagent A and B above.
    if (desired_radio_button_selected == "solution") { // if DESIRED is a SOLUTION.
        desired_conc = read_desired_conc();
        desired_conc_suffix = read_desired_suffix();
        qDebug() << "desired_conc and suffix = " << desired_conc << desired_conc_suffix;
        //This returns the conversion factor or 0 if not found --NO ???.
        // If DESIRED SOLUTION is MOLAR.
        if (sol_is_molar(desired_conc_suffix) != 0) {
            desired_solution_is_molar = true;
            desired_molarity = calc_solution_molarity(desired_conc, desired_conc_suffix);
            qDebug() << "The DESIRED Solution is Molar. "
                     << "The DESIRED suffix = " << desired_conc_suffix;
            qDebug() << "The DESIRED molarity = " << desired_molarity;
        } else {
            desired_solution_is_weight = true;
            desired_solution_is_molar = false;
            desired_g_per_L = g_per_L(desired_conc, desired_conc_suffix);
            qDebug() << "The DESIRED Solution is by WEIGHT. "
                     << "The DESIRED suffix = " << desired_conc_suffix;
            qDebug() << "The DESIRED g_per_L = " << desired_g_per_L;
        }
    }
    //-----------------------------
    // Desired Reagent C and D above.
    // If DESIRED solution is a TOTAL AMOUNT.
    if (desired_radio_button_selected == "total_amount") {
        total_amount = read_total_amount();
        total_amount_suffix = read_total_amount_suffix();
        qDebug() << "Total amount and suffix = " << total_amount << total_amount_suffix;
        desired_reagent_is_total_by_moles = is_desired_reagent_total_by_moles();
        desired_reagent_is_total_by_weight = is_desired_reagent_total_by_weight();

        qDebug() << "desired_reagent_is_total_by_moles = " << desired_reagent_is_total_by_moles;
        qDebug() << "desired_reagent_is_total_by_weight = " << desired_reagent_is_total_by_weight;

        if (desired_reagent_is_total_by_moles) {
            desired_moles = calc_moles(total_amount, total_amount_suffix);
        }
        if (desired_reagent_is_total_by_weight) {
            desired_grams = calc_grams(total_amount, total_amount_suffix);
        }
    } // This is the curly bracket for the end of "Desired reagent C and D" code.

    /*--------------------Repeated Note -----------------------------------------
            (Starting Reagents-sets 1):              (Desired Reagents-sets 1):
         1) starting_solution_is_molar*              A) desired_solution_is_molar*
         2) starting_solution_is_weight              B) desired_solution_is_weight
         3) starting_reagent_is_solid (by weight)    C) desired_reagent_is_total_by_moles*
         4) starting_reagent_is_liquid (by weight)   D) desired_reagent_is_total_by_weight

         -This means 16 different combinations are needed (labeled 1A to 4D).
         -The following 8 are molar-molar combinations, no MW needed:
                 1A, 1C, 2B, 2D, 3B, 3D, 4B, 4D
    -----------------------------------------------------------------------"""
         Start the 16 different reagent combinations.  Order changed from above.----- */

    // The first two don't need mw (nolecular weight) so skip getting it.
    if (starting_solution_is_molar and desired_solution_is_molar)
        skipMW = true;
    if (starting_solution_is_molar and desired_reagent_is_total_by_moles)
        skipMW = true;
    qDebug() << "(Before step 1A) starting_solution_is_molar = " << starting_solution_is_molar;
    qDebug() << "(Before step 1A) desired_solution_is_molar = " << desired_solution_is_molar;
    qDebug() << "(Before step 1A) desired_reagent_is_total_by_moles = "
             << desired_reagent_is_total_by_moles;
    qDebug() << "(Before step 1A) desired_reagent_is_total_by_weight = "
             << desired_reagent_is_total_by_weight;
    qDebug() << "(Before step 1A) skipMW = " << skipMW;
    //Get mw (molecular weight) for other 14 combinations.
    if (skipMW == false)
        mw = read_MW();

    // -- 1A -- no mw needed.
    if (starting_solution_is_molar and desired_solution_is_molar) {
        // Use starting_molarity and desired_molarity.  (NO mw needed)
        qDebug() << "Entered calculate step 1A. ";
        desired_volume_liters = read_desired_volume(); // Get desired volume.
        calculated_desired_moles = desired_molarity * desired_volume_liters;
        ml_needed = ((calculated_desired_moles / starting_molarity) * 1000); // for ml answer.
        format_ml_answer(ml_needed); // Format and post answer to box.
        qDebug() << "desired_volume_liters = " << desired_volume_liters;
        qDebug() << "calculated_desired_moles = " << calculated_desired_moles;
        qDebug() << "ml_needed = " << ml_needed;
    }

    // -- 1C -- No mw needed.
    if (starting_solution_is_molar and desired_reagent_is_total_by_moles) {
        // Use starting_molarity and desired_moles  (NO mw needed).
        qDebug() << "Entered calculate step 1C. ";
        ml_needed = ((desired_moles / starting_molarity) * 1000); // desired_moles from above..
        format_ml_answer(ml_needed);                              // Format and post answer to box.
        qDebug() << "desired_moles = " << desired_moles;
        qDebug() << "ml_needed = " << ml_needed;
    }

    // The last 14 need to use the mw (molecular weight).---------------
    // -- 1B --
    if (starting_solution_is_molar and desired_solution_is_weight) {
        // Use starting_molarity and desired_g_per_L with mw.
        qDebug() << "Entered calculate step 1B. ";
        desired_volume_liters = read_desired_volume(); // Get desired volume.
        calculated_desired_moles = (desired_g_per_L / mw) * desired_volume_liters;
        ml_needed = ((calculated_desired_moles / starting_molarity) * 1000); // for ml answer.
        format_ml_answer(ml_needed); // Format and post answer to box.

        qDebug() << "desired_volume_liters = " << desired_volume_liters;
        qDebug() << "calculated_desired_moles = " << calculated_desired_moles;
        qDebug() << "ml_needed = " << ml_needed;
    }

    // -- 1D --
    if (starting_solution_is_molar and desired_reagent_is_total_by_weight) {
        // Use starting_molarity and desired_grams with mw.
        qDebug() << "Entered calculate step 1D. ";
        calculated_desired_moles = (desired_grams / mw);
        ml_needed = ((calculated_desired_moles / starting_molarity) * 1000); // for ml answer.
        // No error checking needed for starting volume > desired volume
        // because there is no final volume.
        format_ml_answer(ml_needed); // Format and post answer to box.
    }

    //------------------------------------------
    // -- 2A --
    if (starting_solution_is_weight and desired_solution_is_molar) {
        // Use starting_g_per_L and desired_molarity.
        qDebug() << "Entered calculate step 2A. ";
        desired_volume_liters = read_desired_volume(); // Get desired volume.
        calculated_desired_moles = desired_molarity * desired_volume_liters;
        calculated_starting_molarity = (starting_g_per_L / mw);
        ml_needed = ((calculated_desired_moles / calculated_starting_molarity)
                     * 1000);        // for ml answer.
        format_ml_answer(ml_needed); // Format and post answer to box.
        qDebug() << "desired_volume_liters = " << desired_volume_liters;
        qDebug() << "calculated_desired_moles = " << calculated_desired_moles;
        qDebug() << "ml_needed = " << ml_needed;
    }

    // -- 2B -- no mw needed
    if (starting_solution_is_weight and desired_solution_is_weight) {
        // Use starting_g_per_L and desired_g_per_L.
        qDebug() << "Entered calculate step 2B. ";
        desired_volume_liters = read_desired_volume(); // Get desired volume.
        desired_grams = (desired_g_per_L * desired_volume_liters);
        ml_needed = (desired_grams / starting_g_per_L) * 1000;
        format_ml_answer(ml_needed); // Format and post answer to box.
        qDebug() << "desired_volume_liters = " << desired_volume_liters;
        qDebug() << "ml_needed = " << ml_needed;
    }

    // -- 2C --
    if (starting_solution_is_weight and desired_reagent_is_total_by_moles) {
        //Use starting_g_per_L and desired_moles.
        qDebug() << "Entered calculate step 2C. ";
        desired_grams = (desired_moles * mw);
        ml_needed = (desired_grams / starting_g_per_L) * 1000;
        format_ml_answer(ml_needed); // Format and post answer to box.
        qDebug() << "desired_grams = " << desired_grams;
        qDebug() << "ml_needed = " << ml_needed;
    }
    // -- 2D -- no mw needed
    if (starting_solution_is_weight and desired_reagent_is_total_by_weight) {
        // Use starting_g_per_L and desired_grams.
        qDebug() << "Entered calculate step 2D. ";
        ml_needed = (desired_grams / starting_g_per_L) * 1000;
        format_ml_answer(ml_needed); // Format and post answer to box.
        qDebug() << "desired_grams = " << desired_grams;
        qDebug() << "ml_needed = " << ml_needed;
    }

    //---------------------------------------
    // -- 3A --
    if (starting_reagent_is_solid and desired_solution_is_molar) {
        // Use desired_molarity.
        qDebug() << "Entered calculate step 3A. ";
        desired_volume_liters = read_desired_volume(); // Get desired volume.
        calculated_desired_moles = desired_molarity * desired_volume_liters;
        grams_needed = (calculated_desired_moles * mw);
        format_gram_answer(grams_needed); // Format and post answer to box.
        qDebug() << "desired_volume_liters = " << desired_volume_liters;
        qDebug() << "calculated_desired_moles = " << calculated_desired_moles;
        qDebug() << "grams_needed = " << grams_needed;
    }

    // -- 3B -- no mw needed
    if (starting_reagent_is_solid and desired_solution_is_weight) {
        // Use desired_g_per_L.
        qDebug() << "Entered calculate step 3B. ";
        desired_volume_liters = read_desired_volume(); // Get desired volume.
        desired_grams = (desired_g_per_L * desired_volume_liters);
        grams_needed = desired_grams;
        format_gram_answer(grams_needed); // Format and post answer to box.
        qDebug() << "desired_volume_liters = " << desired_volume_liters;
        qDebug() << "grams_needed = " << grams_needed;
    }

    // -- 3C --
    if (starting_reagent_is_solid and desired_reagent_is_total_by_moles) {
        // Use desired_moles
        qDebug() << "Entered calculate step 3C. ";
        grams_needed = (mw * desired_moles);
        format_gram_answer(grams_needed); // Format and post answer to box.
        qDebug() << "grams_needed = " << grams_needed;
    }

    // -- 3D -- no mw needed
    if (starting_reagent_is_solid and desired_reagent_is_total_by_weight) {
        //Use desired_grams.
        qDebug() << "Entered calculate step 3D. ";
        grams_needed = desired_grams;
        format_gram_answer(grams_needed); // Format and post answer to box.
        qDebug() << "grams_needed = " << grams_needed;
    }

    //---------------------------------------
    // -- 4A --
    if (starting_reagent_is_liquid and desired_solution_is_molar) {
        // Use density and desired_molarity.
        qDebug() << "Entered calculate step 4A. ";
        desired_volume_liters = read_desired_volume(); // Get desired volume.
        qDebug() << "desired_volume_liters = " << desired_volume_liters;
        calculated_desired_moles = desired_molarity * desired_volume_liters;
        qDebug() << "calculated_desired_moles = " << calculated_desired_moles;
        grams_needed = (calculated_desired_moles * mw);
        qDebug() << "grams_needed = " << grams_needed;
        ml_needed = (grams_needed / density);
        qDebug() << "ml_needed = " << ml_needed;
        format_ml_answer(ml_needed); // Format and post answer to box.
    }
    // -- 4B -- no mw needed
    if (starting_reagent_is_liquid and desired_solution_is_weight) {
        // Use density and  desired_g_per_L.
        qDebug() << "Entered calculate step 4B. ";
        desired_volume_liters = read_desired_volume(); // Get desired volume.
        desired_grams = desired_g_per_L * desired_volume_liters;
        grams_needed = desired_grams;
        ml_needed = (grams_needed / density) * 1000;
        format_ml_answer(ml_needed); // Format and post answer to box.
        qDebug() << "ml_needed = " << ml_needed;
    }
    // -- 4C --
    if (starting_reagent_is_liquid and desired_reagent_is_total_by_moles) {
        // Use desired_g_per_L and desired_moles.
        qDebug() << "Entered calculate step 4C. ";
        grams_needed = (mw * desired_moles);
        ml_needed = (grams_needed / density) * 1000;
        format_ml_answer(ml_needed); // Format and post answer to box.
        qDebug() << "ml_needed = " << ml_needed;
    }
    // -- 4D -- no mw needed
    if (starting_reagent_is_liquid and desired_reagent_is_total_by_weight) {
        // Use density  and desired_grams.
        qDebug() << "Entered calculate step 4D. ";
        grams_needed = desired_grams;
        ml_needed = (grams_needed / density) * 1000;
        format_ml_answer(ml_needed); // Format and post answer to box.
        qDebug() << "ml_needed = " << ml_needed;
    }
} // This is the curly bracket for the end of "calculate pushbutton clicked".
//---------------------------------------------------------------------------

void MainWindow::on_pushButton_FindMW_clicked()
{
    // Looks up MW in database.

    qDebug() << "MainWindow FindMW clicked - Part 1.";
    // Create new Select_Dialog object, BUT don't display it yet.
    Select_Dialog *mySD = new Select_Dialog(this); // Don't make two copies.

    //QString reagent_fragment = ui->textEdit_StartingReagent->toPlainText();
    QString reagent_fragment = read_starting_reagent();
    qDebug() << "In find_MW... of reagent_fragment = " << reagent_fragment;

    mySD->ui4->label->setText("Pick One Reagent In DataBase (Or Cancel).");
    int rowCount = mySD->chem_db_read(reagent_fragment);
    qDebug() << "MainWindow FindMW clicked - Part 2- ROWCOUNT = " << rowCount;

    if (rowCount <= 0) {
        // No local results, do a PubChem (Enterez) search <-----------FINISH ME-----
        if (g_NetworkAccessPermission == true) {
            on_actionEnterez_Search_triggered(true);
            // Line above returns BEFORE search is fully completed.
            // NOTE: A race condition exists here if code continues! <----------
            // Follow code at end of query (processReturnedText2()) to continue.
            return; // End now to prevent race condition.
        } else {
            // If no internet access is granted to search PubMed.
            cc_msgBox("No results were found in your local database, and your preferences prohibit "
                      "internet access to search on PubMed.",
                      "You can change this under 'Edit-Preferences'",
                      "",
                      this);
            return;
        }
    }
    if (rowCount > 20) {
        // Too many results.
        cc_msgBox("Too many matching reagents were found.",
                  "Please narrow you search by typing a longer reagent name.",
                  "",
                  this);
    }
    if (rowCount > 0 and rowCount <= 20) {
        // Display SELECT dialog box. Returns '1' for OK // '0' for canceled.
        int dialogCode = mySD->exec();
        if (dialogCode == 1) {
            ui->textEdit_StartingReagent->setText(Edit_Dialog::g_reagent);
            ui->lineEdit_MW->setText(Edit_Dialog::g_mw_string);
        }
    }
    qDebug() << "MainWindow FindMW clicked - Part 3-END.";
}

void MainWindow::on_pushButton_SaveMW_clicked()
{
    // Saves MW in the database.
    qDebug() << "MainWindow SaveMW clicked.";

    QString reagent_name = read_starting_reagent();
    qDebug() << "save_MW of " << reagent_name;
    if (reagent_name == "Unknown Reagent") {
        // Error message and DON'T SAVE.
        cc_msgBox("Please enter a reagent name.", "", "", this);
        return;
    }

    // Note: Changed to mw as a string (mw_string).
    // Read, validate, and return mw (molecular weight) as a double.
    // double mw = read_MW();
    // qDebug() << "In SaveMW...MW = " << mw;

    QString mw_string = ui->lineEdit_MW->text();
    // Add error checking here for mw = empty etc..
    qDebug() << "In SaveMW...MW = " << mw_string;

    // db_query my_db_query;
    // int myNum = my_db_query.db_Save(reagent_name, mw_string);

    int myNum = db_Save(reagent_name, mw_string);
    qDebug() << "myNum = " << myNum;
}

void MainWindow::on_actionDatabase_triggered()
{
    // Display and edit the ChemCalc chemical database.
    qDebug() << "MainWindow Menu Edit-Database triggered-STARTED.";
    Edit_Dialog *myED = new Edit_Dialog(this);
    myED->open();
    qDebug() << "MainWindow Menu Edit-Database triggered-ENDED.";
}

void MainWindow::on_textEdit_StartingReagent_cursorPositionChanged()
{
    // On first click of 'Starting Reagent' text box, clear original text.
    qDebug() << "Clearing Starting Reagent defauilt text-2.";
    if (ui->textEdit_StartingReagent->toPlainText() == "Enter reagent name here.") {
        ui->textEdit_StartingReagent->setPlainText("");
    };
}

//-------------TEST AREA-------------------------

//------Start of PubChem Search Code------------------------------------
// The many functions called (in order) are:
// 1) on_actionEnterez_Search_triggered()
// 1B) which includes---> manager->get(QNetworkRequest(QUrl(entrezURL)));
// 2) replyFinished1()
// 3) processReturnedText1()
// -----------------------
// 4) entrezQuery2()
// 4B) which includes---> manager2->get(QNetworkRequest(QUrl(entrezURL2)))
// 5) replyFinished2()
// 6) processReturnedText2()
// 7) askToUseAndSave()
//-------------------------------------------

void MainWindow::on_actionEnterez_Search_triggered(bool strictSearch = true)
{
    // This slot is called when the "entrez search" menu item is clicked,
    // or when 'Find_MW()' calls it due to reagent not in local database.
    // For strictSearch: true (default) = strict search, false = not strict (loose) search.
    //
    // This function queries a PubChem chemical database
    // in order to obtain the molecular Weight (mw).
    // This is the FIRST SEARCH that gets UID's only.
    // Later 'entrezQuery2()' is called to get Docsums of compounds.
    // Ref for e-searches:  https://www.ncbi.nlm.nih.gov/books/NBK25497/
    // Start with "strictSearch" = true (default/optional) but if no results change to false.

    QString myReagentText = read_starting_reagent();
    qDebug() << "Starting on_actionEntrez_Search_triggered. myReagentText = " << myReagentText;

    // g_strictSearch is class scope, declared in header file. (vs local strictSearch).
    g_strictSearch = strictSearch; // Used at end of search if needed to run a loose search.
    qDebug() << "strictSearch = " << strictSearch;

    // From https://doc.qt.io/qt-6/qnetworkaccessmanager.html#details
    // Note there are some new functions in Qt6.7 that may work better (less slots).
    // See https://doc.qt.io/qt-6/qnetworkaccessmanager.html#get-2
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MainWindow::replyFinished1);

    QString pubMedDB
        = "pccompound"; // "PubChem-Compound" There are many other databases, but this one is good.
    QString query = myReagentText;
    // Replace spaces with "+".
    query.replace(" ", "+");

    // assemble the esearch URL
    QString urlA1 = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=";
    QString urlA2 = pubMedDB;
    QString urlA3 = "&term=";
    QString urlA4; // Must be declared outside of if loop.
    // (%22=1 double quote) query search terms has double quotes around it.
    if (strictSearch == true) {
        urlA4 = "%22" + query + "%22";
    }
    if (strictSearch == false) {
        urlA4 = query;
    } // (query search terms do NOT have any quotes around it.
    QString urlA5 = "&usehistory=y";
    // Returns only one value.  Works great with just the best search return.
    QString urlA6 = "&retmax=1";
    QString urlA7 = "&sort=relevance";
    QString urlAComplete = urlA1 + urlA2 + urlA3 + urlA4 + urlA5 + urlA6 + urlA7;
    qDebug() << "urlAComplete = " << urlAComplete;

    QUrl entrezURL(urlAComplete);
    // If no internet connection.
    if (!entrezURL.isValid()) {
        cc_msgBox("Could not establish an internet connection to PubChem databases.", "", "", this);
        qDebug() << "entrezURL is NOT valid. = " << entrezURL;
        entrezURL.clear(); // You need to clear QUrl on failure.
        return;            // End entire function.
    }
    qDebug() << "entrezURL is valid. = " << entrezURL;
    qDebug() << "Encoded entrezURL = " << entrezURL.toEncoded();

    // Leads to (slot) MainWindow::replyFinished1(QNetworkReply *reply1).
    manager->get(QNetworkRequest(QUrl(entrezURL)));
    qDebug() << "End of on_actionEnterez_Search_triggered.";
}

void MainWindow::replyFinished1(QNetworkReply *reply1)
{
    // The web reply for FIRST SEARCH ends up here.
    qDebug() << "replyFinshed1 started.  " << "reply 1 = " << reply1;
    QByteArray myByteArray1 = reply1->readAll();   //WORKS but includesheader etc.
    qDebug() << "myByteArray1 = " << myByteArray1; // LONG! Uncomment to debug.
    entrezData myEntrezDataStruct = processReturnedText1(myByteArray1);
    qDebug() << "myEnterezDataStruct.WebEnv = " << myEntrezDataStruct.WebEnv;
    qDebug() << "myEnterezDataStruct.QueryKey = " << myEntrezDataStruct.QueryKey;
    qDebug() << "myEnterezDataStruct.UIDString = " << myEntrezDataStruct.UIDString;
    // Start SECOND entrez search, for DocSums.
    entrezQuery2(myEntrezDataStruct);
}

// *******************************************************
// Note: The following items are declared at top for CLASS SCOPE.
// 1) QString UIDString();
//
// And in mainwindow.h:
// 2) struct entrezData{
//       QString WebEnv;
//       QString QueryKey;
//       QString UIDString;}
// *******************************************************

MainWindow::entrezData MainWindow::processReturnedText1(QByteArray myByteArray1)
{
    //This returns a 'entrezData' struct of 2 data items and 1 Qlist obtained from the network response myByteArray1.
    qDebug() << "processReturnedText1 called";
    auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
    QString myString = toUtf16(myByteArray1);
    qDebug() << "\nmyString = " << myString;

    QXmlStreamReader xml(myByteArray1);
    QString chemUIDHeader = "Id"; //These need to be declared outside of if/while loops below.
    QString chemUIDNumber = "";
    QString webEnvHeader = "WebEnv";
    QString webEnvNumber = "";
    QString queryKeyHeader = "QueryKey";
    QString queryKeyNumber = "";

    while (!xml.atEnd()) {
        xml.readNext();
        // 1) Look for 1 "<Id>" element and put into UIDString.
        if (xml.name() == chemUIDHeader) {
            chemUIDNumber = xml.readElementText(); // This is the chemical ID number.
            qDebug() << "chemUIDNumber = " << chemUIDNumber;
            // UIDList.append(chemUIDNumber);  //DELETE
            UIDString = chemUIDNumber;
        } //end of FIRST if loop. --------------------------------------

        // 2) Look for (one) "<WebEnv>" element and put into QString 'webEnvNumber'.-----
        if (xml.name() == webEnvHeader) {
            qDebug() << "webEnvHeader = " << xml.name();
            webEnvNumber = xml.readElementText(); // This is the web Envelope number.
            qDebug() << "webEnvNumber = " << webEnvNumber;
        } //end of SECOND if loop. ----------------------------------------------------

        // 3) Look for (one) "<QueryKey>" element and put into QString 'query'.-----
        if (xml.name() == queryKeyHeader) {
            qDebug() << "queryKeyHeader = " << xml.name();
            queryKeyNumber = xml.readElementText(); // This is the web Envelope number.
            qDebug() << "queryKeyNumber = " << queryKeyNumber;
        } //end of THIRD if loop. ----------------------------------------------------
    } //end of while loop.

    // do error handling
    if (xml.hasError()) {
        qDebug() << "Error in processReturnText() function. ";
        // Add more here.  // <-----------------
    } //end of if loop (error processor).
    qDebug() << "UIDString = " << UIDString;
    // return entrezData structure;
    return entrezData{webEnvNumber, queryKeyNumber, UIDString};
}

void MainWindow::entrezQuery2(entrezData myEntrezDataStruct)
{
    // This function queries a PubChem chemical database
    // in order to obtain the molecular Weight (mw).
    // This is the SECOND SEARCH that gets Docsums of compounds.
    // Ref for e-searches:  https://www.ncbi.nlm.nih.gov/books/NBK25497/
    // ESummary Output: XML Document Summaries

    //------Notes from book------------
    // Downloading Data From a Previous Search
    // esearch.fcgi?db=<database>&term=<query>&usehistory=y  // <------entrezQery1-----
    // # esearch produces WebEnv value ($web1) and QueryKey value ($key1)
    // esummary.fcgi?db=<database>&query_key=$key1&WebEnv=$web1  // <-------entrezQuery2-----
    // Input: Web environment (&WebEnv) and query key (&query_key) representing a set
    // of Entrez UIDs on the Entrez history server
    //----------------------------------

    // From https://doc.qt.io/qt-6/qnetworkaccessmanager.html#details
    // Only one QNetworkManager needed for the whole Qt app, but I used 2.

    QString myWebEnv = myEntrezDataStruct.WebEnv;
    QString myQueryKey = myEntrezDataStruct.QueryKey;
    qDebug() << "myWebEnv = " << myWebEnv << " and  " << "myQueryKey = " << myQueryKey;

    QNetworkAccessManager *manager2 = new QNetworkAccessManager(this);
    connect(manager2, &QNetworkAccessManager::finished, this, &MainWindow::replyFinished2);

    // assemble the 2nd URL.
    QString urlB1 = "https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esummary.fcgi?db=";
    QString urlB2
        = "pccompound"; // "PubChem-Compound" There are many other databases, but this one is good.
    QString urlB3 = "&query_key=";
    QString urlB4 = myQueryKey;
    QString urlB5 = "&WebEnv=";
    QString urlB6 = myWebEnv;
    // Returns only one value.  Works great with just the best search return.
    QString urlB7 = "&retmax=1";
    QString urlBComplete = urlB1 + urlB2 + urlB3 + urlB4 + urlB5 + urlB6 + urlB7;
    QUrl entrezURL2(urlBComplete);

    // If no internet connection.
    if (!entrezURL2.isValid()) {
        cc_msgBox("Could not establish an internet connection to PubChem databases.", "", "", this);
        qDebug() << "entrezURL2 is NOT valid. = " << entrezURL2;
        entrezURL2.clear(); // You need to clear QUrl on failure.
        return;             // End entire function.
    }

    qDebug() << "entrezURL2 is valid. = " << entrezURL2;
    qDebug() << "Encoded entrezURL2 = " << entrezURL2.toEncoded();
    // Leads to replyFinished2() via Signals and Slots.
    manager2->get(QNetworkRequest(QUrl(entrezURL2)));
}

void MainWindow::replyFinished2(QNetworkReply *reply2)
{
    // The web reply for SECOND SEARCH ends up here via Signals and Slots.
    qDebug() << "replyFinshed2 started.  " << "reply 2 = " << reply2;
    QByteArray myByteArray2 = reply2->readAll(); //WORKS but includesheader etc.
    myByteArray2 = myByteArray2.simplified();    //Removes excess whitespace (/n,/t, etc).
    qDebug() << "myByteArray2 = " << myByteArray2;
    processReturnedText2(myByteArray2);
}

void MainWindow::processReturnedText2(QByteArray myByteArray2)
{
    // This gets chemName and molecular weight(mw)
    // obtained from the network response mByteArray2.
    // Example: returned text from 'Sodium Chloride': <Item Name=\"MolecularWeight\" Type=\"String\">58.440</Item>
    qDebug() << "processReturnedText2 called \n";

    QDomNodeList myNodeList;
    QDomNode myDomNode;
    QString molecularWeight, chemNameReturned;
    QDomDocument myDomDoc;
    myDomDoc.setContent(myByteArray2);

    // Uncomment this for debugging (lots of output).
    // QString myString = myDomDoc.toString();
    // qDebug() << "\n myString = " << myString;

    // Clear default molecular weight.
    ui->lineEdit_MW->setText("");

    myNodeList = myDomDoc.elementsByTagName("Item");
    for (int i = 1; i <= myNodeList.count(); i++) {
        myDomNode = myNodeList.item(i);
        QDomElement myElement = myDomNode.toElement();
        QDomAttr myAttribute = myElement.attributeNode("Name");
        QString myAttributValue = qPrintable(myAttribute.value());
        QString myElementText = myElement.text();
        // Uncomment for debugging.
        // qDebug() << "Name = " << myAttributValue << "myText = " << myText;

        // Get  Molecular Weight (MW).
        if (myAttributValue == "MolecularWeight") {
            molecularWeight = myElementText;
            qDebug() << "myText (MW) = " << molecularWeight;
        }
        // Get chemical name "MeSHTermList" (chemNameReturned).
        if (myAttributValue == "MeSHHeadingList") { //This is NOT my typo: "MeSHHeadingList";
            chemNameReturned = myElementText;
            qDebug() << "chemNameReturned = " << chemNameReturned;
        }
    }

    if (chemNameReturned.isEmpty() or molecularWeight.isEmpty()) {
        qDebug() << "In processReturnedText2, either chemName or MW are EMPTY!";
        if (g_strictSearch == true) {
            // Start a LOOSE SEARCH.  Watch for infinite loop with no results!
            // NOTE: Insert a time delay to prevent being banned from database. (3 queries/sec max)
            QObject().thread()->msleep(1000); //Sleep for 1000 milliseconds (1 sec).
            on_actionEnterez_Search_triggered(false);
            return; // Nothing is written to GUI (MW and chemical name) on early return.
        }
        if (g_strictSearch == false) {
            // If LOOSE SEARCH failed too.
            qDebug() << "No results were found in both PubChem and Local databases.";
            // If no valid results from entrezQuery, ask to open PubChem website.
            //This creates a modal message box with a message and OK button.
            QMessageBox msgBox2;
            msgBox2.setText("No results were found in both PubChem and Local databases.");
            msgBox2.setInformativeText("Do you want open the PubChem website in your browser?");
            msgBox2.setParent(this);
            msgBox2.setIcon(QMessageBox::Question);
            msgBox2.setWindowModality(Qt::WindowModal);
            msgBox2.setWindowTitle("ChemCalc");
            msgBox2.setStandardButtons(QMessageBox::Open | QMessageBox::No | QMessageBox::Cancel);
            msgBox2.setDefaultButton(QMessageBox::Cancel);
            int ret2 = msgBox2.exec();
            if (ret2 == QMessageBox::Open) {
                // Open PubChem website.
                on_actionOpen_PubChem_Website_triggered();
                return; // Nothing is written to GUI (MW and chemical name) on early return.
            }
            if (ret2 == (QMessageBox::No | QMessageBox::Cancel)) {
                qDebug() << "No or Cancel button selected.";
                return; // Nothing is written to GUI (MW and chemical name) on early return.
            }
        }
    }
    askToUseAndSave(chemNameReturned, molecularWeight);
} // End of processReturnedText2.

void MainWindow::askToUseAndSave(QString chemNameReturned, QString molecularWeight)
{
    // Only valid data continues from here. (Has 'chemNameReturned',
    // and 'molecularWeight', but it might not be the correct chemical you want.)

    // Get typed Reagent Name in GUI.
    QString typedReagentName = ui->textEdit_StartingReagent->toPlainText();

    qDebug() << "MW = " << molecularWeight << " reagent = " << chemNameReturned;
    QString chemNameToGui; // This chem name is the one added to Gui and used for calculations.
    //-------------------------------
    // Ask to use molecular weight in the GUI (for calculation).
    QMessageBox msgBox5;
    msgBox5.setText("PubChem found: MW = " + molecularWeight + " for " + chemNameReturned);
    msgBox5.setInformativeText("Do you want to use this MOLECULAR WEIGHT for the calculation?");
    msgBox5.setParent(this);
    msgBox5.setIcon(QMessageBox::Question);
    msgBox5.setWindowModality(Qt::WindowModal);
    msgBox5.setWindowTitle("ChemCalc");
    msgBox5.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox5.setDefaultButton(QMessageBox::Cancel);
    int ret5 = msgBox5.exec();
    if (ret5 == (QMessageBox::No | QMessageBox::Cancel)) {
        qDebug() << "In ask to use MW, No or Cancel button selected.";
        return; // Nothing is written to GUI or saved (MW and chemical name) on early return.
    }
    if (ret5 == QMessageBox::Yes) {
        qDebug() << "In ask to use MW, YES BUTTON selected.";
        ui->lineEdit_MW->setText(molecularWeight);
    }

    //--------------------------------
    // Ask to save chemical name in the local database.
    QMessageBox msgBox;
    msgBox.setText("PubChem found: (MW = " + molecularWeight + ") " + chemNameReturned);
    msgBox.setInformativeText("Do you want to save this in the database?");
    msgBox.setParent(this);
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setWindowTitle("ChemCalc");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == (QMessageBox::Discard | QMessageBox::Cancel)) {
        qDebug() << "Discard or Cancel button selected.";
        return; // Nothing is written to GUI (MW and chemical name) on early return.
    }
    if (ret == (QMessageBox::Save)) {
        // Save (to local database) and insert data (to GUI).

        // Added code here to select which reagent name (e.g. EDTA vs Edetic Acid)
        // If typed reagent name is not exactly the same as returned name, choose a name to save.
        if (typedReagentName != chemNameReturned) {
            qDebug() << "Choosing a reagent name to save.";
            // ------------
            //This creates a modal message box with a message and OK button.
            QMessageBox msgBox3;
            msgBox3.setText("Returned reagent name is different than your typed reagent name.");
            msgBox3.setInformativeText("Do you want to save under both names");
            msgBox3.setParent(this);
            msgBox3.setIcon(QMessageBox::Question);
            msgBox3.setWindowModality(Qt::WindowModal);
            msgBox3.setWindowTitle("ChemCalc");
            msgBox3.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
            msgBox3.setDefaultButton(QMessageBox::Cancel);
            int ret3 = msgBox3.exec();
            if (ret3 == QMessageBox::Yes) {
                // Save (twice) under both reagent names.
                qDebug() << "Writing chemname and MW to LOCAL database.";
                // Add (returned Reagent Name) data to localdatabase. (skipping confirmation)
                db_Save(chemNameReturned, molecularWeight, true);
                qDebug() << "New reagent and MW added to LOCAL database.";
                chemNameToGui = typedReagentName;
            }
            if (ret3 == QMessageBox::Cancel) {
                qDebug() << "Cancel button selected.";
                return; // Nothing is written to GUI (MW and chemical name) on early return.
            }
            if (ret3 == QMessageBox::No) {
                qDebug() << "No button selected.";
                // Code to choose (saving) either typed or returned reagent name
                //This creates a modal message box.
                QMessageBox msgBox4;
                msgBox4.setText("Which reagent name do you want to save.");
                QString namesText = "1) = " + typedReagentName + ".\n" + "2) = " + chemNameReturned
                                    + ".";
                msgBox4.setInformativeText(namesText);
                msgBox4.setParent(this);
                msgBox4.setIcon(QMessageBox::Question);
                msgBox4.setWindowModality(Qt::WindowModal);
                msgBox4.setWindowTitle("ChemCalc");
                msgBox4.setStandardButtons(QMessageBox::Cancel);
                msgBox4.setDefaultButton(QMessageBox::Cancel);
                QPushButton *use1Button = msgBox4.addButton("Use 1", QMessageBox::ActionRole);
                QPushButton *use2Button = msgBox4.addButton("Use 2", QMessageBox::ActionRole);
                int ret4 = msgBox4.exec();

                if (msgBox4.clickedButton() == use1Button) {
                    // Add (TYPED Reagent Name) data to localdatabase.
                    qDebug() << "use1Button clicked. Saving TYPED reagent to local database.";
                    chemNameToGui = typedReagentName;
                }
                if (msgBox4.clickedButton() == use2Button) {
                    chemNameToGui = chemNameReturned;
                }
                if (ret4 == QMessageBox::Cancel) {
                    qDebug() << "Cancel button selected.";
                    return; // Nothing is saved or written to GUI (MW and chemical name) on early return.
                }
            }
        }

        qDebug() << "Writing chemname to GUI.";
        ui->textEdit_StartingReagent->setPlainText(chemNameToGui);
        // Add data to localdatabase.
        db_Save(chemNameToGui, molecularWeight, true); // Skip confirmation message.
        qDebug() << "chemNameReturned and MW added to LOCAL database.";
    }
}

void MainWindow::on_actionOpen_PubChem_Website_triggered()
{
    // Open PubChem website in users browser.
    bool OK = QDesktopServices::openUrl(QUrl("https://pubchem.ncbi.nlm.nih.gov/"));
    if (!OK) {
        cc_msgBox("Unknown internet or web browser error.", "", "", this);
    }
}

void MainWindow::on_actionPreferences_triggered()
{
    // Called from "Edit / Preferences" menu or first start.
    // Displays preferences_dialog and
    // allows user to set some global variables.
    qDebug() << "on_actionPreferences_triggered()";

    // parseConfigFile("NetworkAccessPermission ", true, true);
    // parseConfigFile("NetworkAccessAsked ", true, true);
    // parseConfigFile("NetworkAccessDontAskAgain ", true, true);

    setConfigGlobals();
    qDebug() << "In on_actionPreferences_triggered: g_NetworkAccessPermission = "
             << g_NetworkAccessPermission;
    qDebug() << "In on_actionPreferences_triggered: g_NetworkAccessAsked = "
             << g_NetworkAccessAsked;
    qDebug() << "In on_actionPreferences_triggered: g_NetworkAccessDontAskAgain = "
             << g_NetworkAccessDontAskAgain;

    Preferences_Dialog *myPreferencesDialog = new Preferences_Dialog(this);
    myPreferencesDialog->open();
}
