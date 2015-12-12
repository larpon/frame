# (Media) frame
A (work in progress) KF5 plasmoid port of the KDE (picture) frame widget.

This port does not aim to be 100% true to the original.

Please note that the sections below are just keywords for me to remember. Thus not intended for the general public :)

# Building with cmake
`sudo apt-get cmake`

## OPTIONAL Use a prefix
`cmake -DCMAKE_INSTALL_PREFIX=/home/user/Environments/KDE/`

## Help system find cmake QML plugins
cmake installs per default to /usr/local/[...] so add
`export QML2_IMPORT_PATH=${QML2_IMPORT_PATH}:/usr/local/lib/x86_64-linux-gnu/qml`
to ~/.profile

## Building
From project dir (where CMakeLists.txt are located):
```
mkdir build
cd build
cmake ..
make
sudo make install
```

# Kubuntu 15.04, 15.10 notes

## KDE Extra CMake Modules:
...
Could not find a package configuration file provided by "ECM" (requested
  version 0.0.11) with any of the following names:

    ECMConfig.cmake
    ecm-config.cmake
...

Fix:
`sudo apt-get install extra-cmake-modules`


## KDE kf5 dev libs:
...
Could not find a package configuration file provided by "KF5I18n"
  (requested version 5.12.0) with any of the following names:

    KF5I18nConfig.cmake
    kf5i18n-config.cmake
...
  Could not find a package configuration file provided by "KF5KIO" (requested
  version 5.12.0) with any of the following names:

    KF5KIOConfig.cmake
    kf5kio-config.cmake
...
Could not find a package configuration file provided by "KF5Plasma"
  (requested version 5.12.0) with any of the following names:

    KF5PlasmaConfig.cmake
    kf5plasma-config.cmake
...
Could not find a package configuration file provided by "KF5PlasmaQuick"
  (requested version 5.12.0) with any of the following names:

    KF5PlasmaQuickConfig.cmake
    kf5plasmaquick-config.cmake
...
Could not find a package configuration file provided by "KF5WindowSystem"
  (requested version 5.12.0) with any of the following names:

    KF5WindowSystemConfig.cmake
    kf5windowsystem-config.cmake
...
Could NOT find KF5 (missing: I18n KIO Plasma PlasmaQuick WindowSystem)

Fix:
```
sudo apt-get install libkf5i18n-dev
sudo apt-get install kio-dev
sudo apt-get install plasma-workspace-dev
```

# Random notes
plasmashell output:
```Failed to open BO for returned DRI2 buffer (1920x1080, dri2 back buffer, named 7).
This is likely a bug in the X Server that will lead to a crash soon.```

## Help

### Old applet src
https://quickgit.kde.org/?p=kdeplasma-addons.git&a=tree&h=83190106788557085acaaa1fa03d432ce2a875ef&hb=78cd7a67251166dc4b41251b6a6fcc7f982acee7&f=applets%2Fframe

### C++ code in plasmoids
https://quickgit.kde.org/?p=plasma-desktop.git&a=tree&h=65794860b48912689c8c6f6fc94af1ff3c964db7&hb=d1191d784060e359cc4e06b44e4ce47463798c05&f=applets%2Fkicker
https://quickgit.kde.org/?p=plasma-desktop.git&a=tree&h=6358e531726be4cd40cef18e13f04fb691f061bc&hb=d1191d784060e359cc4e06b44e4ce47463798c05&f=applets%2Ftaskmanager
https://quickgit.kde.org/?p=plasma-desktop.git&a=tree&h=d28724cb342eda268a87d848013114d9a4a7a7c8&hb=d1191d784060e359cc4e06b44e4ce47463798c05&f=containments%2Fdesktop

### Doc bundle format used by Qt Assistant
http://api.kde.org/qch/frameworks5-frameworks.qch

