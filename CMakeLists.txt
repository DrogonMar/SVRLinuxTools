cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(SVRLinuxTools)

set(CMAKE_CXX_STANDARD 17)

set(QT_MIN_VERSION "5.3.0")
set(KF5_MIN_VERSION "5.2.0")

if(CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DRELEASE=1)
endif()
message(${CMAKE_BUILD_TYPE})

find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

include(ThirdParty/ThirdParty.cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
        Core    # QCommandLineParser, QStringLiteral
        Widgets # QApplication
        Gui
        OpenGL
        WebSockets
        )

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
        CoreAddons      # KAboutData
        I18n            # KLocalizedString
        WidgetsAddons   # KMessageBox
        )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL WebSockets REQUIRED)

set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")

set(DU_sources src/main.cpp
        src/Utils.cpp
        src/MainOverlay/MainOverlay.cpp
        src/GPMDP/GPMDP.cpp
        src/GPMDP/ImageCache.cpp)

set(DU_headers src/SVRLT.h
        src/Utils.h
        src/MainOverlay/MainOverlay.h
        src/GPMDP/GPlaylist.h
        src/GPMDP/GTrack.h
        src/GPMDP/GPMDP.h
        src/GPMDP/ImageCache.h)

set(DU_ui src/MainOverlay/MainOverlay.ui
        src/GPMDP/GPMDP.ui
        )

qt5_wrap_ui(ui_wrap ${DU_ui})
qt5_wrap_cpp(moc_sources ${DU_headers})


add_executable(SVRLinuxTools
        ${TP_SOURCES}
        ${TP_HEADERS}
        ${DU_sources}
        ${DU_headers}
        ${ui_wrap}
        ${moc_sources})

target_link_libraries(SVRLinuxTools
        openvr_api
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::OpenGL
        Qt5::WebSockets
        KF5::CoreAddons
        KF5::I18n
        KF5::WidgetsAddons)

install(TARGETS SVRLinuxTools  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
