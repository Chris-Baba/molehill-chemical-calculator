#include <QApplication>
#include <QGuiApplication> //?
#include <QDebug>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QLoggingCategory>

#include "mainwindow.h"

//--------------------------------------------------------
// Declare database connection outside of any function. It
//  also needs "include QCoreApplication" environment.
QSqlDatabase MainWindow::g_db;
//--------------------------------------------------------

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //--- Read commandline options.
    bool verbose = false;  // Defaults to no qDebug output.
    QCoreApplication::setApplicationName("ChemCalc");
    QCommandLineParser parser;
    parser.setApplicationDescription("A Chemical Calculator");
    parser.addHelpOption();
    QCommandLineOption verboseOption(QStringList() << "v" << "verbose", "Verbose debugging output");
    parser.addOption(verboseOption);
    // Process the actual command line arguments given by the user
    parser.process(app);
    verbose = parser.isSet(verboseOption);
    if (!verbose){
        // This turns off qDebug messages in all chemcalc code.
        // Comment out next line (and revert when done) for IDE debugging.
        QLoggingCategory::setFilterRules("*.debug=false");  //<-- UNCOMMENT <<--<<--
    };
    //----

    MainWindow w;
    w.show();
    return app.exec();
}
