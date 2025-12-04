cmake_minimum_required(VERSION 3.12)

project(molehill VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Widgets PrintSupport Sql Gui Network Xml DBus)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets PrintSupport Sql Gui Network Xml DBus)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(chemcalc
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        mprintdialog.h
        mprintdialog.cpp
        mprintdialog.ui
        edit_dialog.h
        edit_dialog.cpp
        edit_dialog.ui
        resources.qrc
        select_dialog.h
        select_dialog.cpp
        select_dialog.ui
        about_dialog.h
        about_dialog.cpp
        about_dialog.ui
        COPYING.txt
        chemcalc.desktop
        chemcalc_icon_120.svg
        preferences_dialog.h
        preferences_dialog.cpp
        preferences_dialog.ui
    )

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET chemcalc APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(chemcalc SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(chemcalc
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(chemcalc PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(chemcalc PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)
target_link_libraries(chemcalc PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(chemcalc PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(chemcalc PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(chemcalc PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(chemcalc PRIVATE Qt${QT_VERSION_MAJOR}::Xml)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.github.chris_baba.molehill)
endif()
set_target_properties(chemcalc PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE FALSE
    WIN32_EXECUTABLE FALSE
)

# See https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
include(GNUInstallDirs)  # Provides prefix ('/usr' for my linux) for below lines.
install(TARGETS chemcalc
    BUNDLE DESTINATION .  # Used only on macOS.
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}  # /usr/lib for me (prefix+/lib)
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # /usr/bin for me (prefix+/bin)
)
# See https://cmake.org/cmake/help/latest/command/install.html#files
install(FILES chemcalc.desktop
    # Defaults to /usr/share + /applications on my linux.
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES chemcalc_icon_120.svg
    # Defaults to /usr/share + /icons/hicolor/scalable/apps on my linux.
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(chemcalc)
endif()

## Delay for 2 seconds to let deployment work correctly.
# file(LOCK C:/Users/chris/Documents/molehill-win11/build/MinGW-Release/chemcalc.exe TIMEOUT 2)

## Note: The next steps are DEPLOYMENT (windeployqt.exe),
## and INSTALLER (~/Qt/Tools/QtInstallerFramework/4.10/bin/binarycreator).
## See https://doc.qt.io/qtinstallerframework/ifw-tutorial.html
## Deployment executable is at "C:\Qt\6.10.1\mingw_64\bin\windeployqt.exe",
## but this may need a first time setup, see docs.
##
##---DEPLOYMENT SECTION-----
# 1) CD "C:\Users\chris\Documents\molehill-win11\molehill-win-installer\packages\com.github.chris_baba.molehill_chemical_calculator\data"
# 2) C:\Qt\6.10.1\mingw_64\bin\windeployqt.exe C:\Users\chris\Documents\molehill-win11\build\Desktop_Qt_6_10_1_MinGW_64_bit-Release\chemcalc.exe
## I got this Warning: Cannot find any version of the dxcompiler.dll and dxil.dll.
##
##---INSTALLER SECTION------
# 3) CD C:\Users\chris\Documents\molehill-win11\molehill-win-installer
## ------Run following 5 lines as one command:-----
# 4) C:\Qt\Tools\QtInstallerFramework\4.10\bin\binarycreator.exe --offline-only
# -t C:\Qt\Tools\QtInstallerFramework\4.10\bin\installerbase.exe
# -p C:\Users\chris\Documents\molehill-win11\molehill-win-installer\packages
# -c C:\Users\chris\Documents\molehill-win11\molehill-win-installer\config\config.xml
# Molehill_Installer.exe
##--------------------
