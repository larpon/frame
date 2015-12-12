project(mediaframe)
set(PROJECT_VERSION "1.0")
set(PROJECT_VERSION_MAJOR 1)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.4.0")

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)
include(CheckIncludeFiles)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Quick
    QuickWidgets
)

set(KF5_MIN_VERSION "5.12.0")
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    I18n
    KIO
    Plasma
    PlasmaQuick
    WindowSystem
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

add_definitions(
  -DQT_USE_QSTRINGBUILDER
  -DQT_NO_CAST_TO_ASCII
# -DQT_NO_CAST_FROM_ASCII
  -DQT_STRICT_ITERATORS
  -DQT_NO_URL_CAST_FROM_STRING
  -DQT_NO_CAST_FROM_BYTEARRAY
  -DQT_NO_SIGNALS_SLOTS_KEYWORDS
  -DQT_USE_FAST_OPERATOR_PLUS
  -DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.mediaframe\"
)

plasma_install_package(package org.kde.plasma.mediaframe)

include_directories(plugin/plasmaquick)

set(mediaframeplugin_SRCS
    plugin/mediaframe.cpp
    plugin/mediaframeplugin.cpp
)

install(FILES plugin/qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/mediaframe)

add_library(mediaframeplugin SHARED ${mediaframeplugin_SRCS})

target_link_libraries(mediaframeplugin
                      Qt5::Core
                      Qt5::Qml
                      Qt5::Quick
                      KF5::I18n
                      KF5::KIOCore
                      KF5::KIOWidgets
                      KF5::PlasmaQuick)

install(TARGETS mediaframeplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/plasma/private/mediaframe)
