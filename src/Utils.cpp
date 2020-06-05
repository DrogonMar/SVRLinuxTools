//
// Created by drogonmar on 6/4/20.
//
#include <QtCore/QString>

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "SVRLT.h"
#include "Utils.h"

QString Utils::GetHomeDir() {
    QString HomeDir;
    if((HomeDir = QString(getenv("HOME"))) == NULL){
        HomeDir = QString(getpwuid(getuid())->pw_dir);
    }
    return HomeDir + '/';
}

QString Utils::GetSettingsDir(){
    QString SettingsDir;
#if RELEASE
    SettingsDir = GetHomeDir() + ".config/"+ InternalName + '/';
#else
    SettingsDir = "Settings/";
#endif

    return SettingsDir;
}

QString Utils::GetCacheDir() {
    QString CacheDir;
#if RELEASE
    CacheDir = GetHomeDir() + ".cache/" + QString(InternalName) + "/";
#else
    CacheDir = "Cache/";
#endif
    return CacheDir;
}