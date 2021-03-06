//QT
#include <QApplication>
#include <QCommandLineParser>
#include <QMainWindow>

//KDE
#include <KAboutData>
#include <KLocalizedString>
#include <KMessageBox>

//ThirdParty
#include "ThirdParty/OpenVRQT/openvroverlaycontroller.h"

//Local
#include "SVRLT.h"
#include "MainOverlay/MainOverlay.h"
#include "Utils.h"

int main(int argc, char *argv[]){
    QApplication app(argc, argv);

    //Setup KDE stuff
    KLocalizedString::setApplicationDomain(InternalName);
    SetupAboutData();

    //Setup parser
    QCommandLineParser parser;
    parser.setApplicationDescription(ShortDescription);
    parser.addHelpOption();

    QCommandLineOption novrOption("novr",
            QCoreApplication::translate("main", "Run without VR."));
    parser.addOption(novrOption);

    parser.process(app);

    //Check the dirs to make sure they exist
    qDebug() << Utils::GetHomeDir();
    qDebug() << Utils::GetSettingsDir();
    qDebug() << Utils::GetCacheDir();

    if(!QDir(Utils::GetSettingsDir()).exists()){
        QDir().mkdir(Utils::GetSettingsDir());
        qDebug() << "Settings dir created";
    }

    if(!QDir(Utils::GetCacheDir()).exists()){
        QDir().mkdir(Utils::GetCacheDir());
        qDebug() << "Cache dir created";
    }

    auto view = new MainOverlay;

    bool novr = parser.isSet(novrOption);

    if(novr){
        view->show();
    }else{
        COpenVROverlayController::SharedInstance()->Init();
        COpenVROverlayController::SharedInstance()->SetWidget( view );
    }

    int result = app.exec();

    //Shut it down like a nice lad :)
    if(!novr) COpenVROverlayController::SharedInstance()->Shutdown();

    return result;
}