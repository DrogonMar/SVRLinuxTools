//
// Created by drogonmar on 11/19/19.
//
#pragma once

#include <KAboutData>

/*
 * The reason I have all the KDE About here is because its a lot faster to find this file
 * and edit it ;)
 */
#define InternalName "SVRLinuxTools"
#define DisplayName "SVR Linux Tools"
#define Version "1.0"
#define ShortDescription "A SteamVR Overlay"
#define LicenseType KAboutLicense::Custom
#define CopyrightStatement ""
#define OtherText "" //no f'in idea tbh
#define HomePageAddress "https://github.com/DrogonMar/"
#define BugAddress "https://github.com/DrogonMar/SVRLinuxTools/issues"

#define QPROPGETSET(type, qmlName, cppName) type get##qmlName() const { \
    return cppName;  \
    }

static void SetupAboutData() {
    KAboutData aboutData(InternalName, DisplayName, Version, ShortDescription, LicenseType, CopyrightStatement,
                         OtherText, HomePageAddress, BugAddress);
    aboutData.addAuthor("Reece Hagan", "Programmer", "drogonmar@gmail.com");


    KAboutData::setApplicationData(aboutData);
}

