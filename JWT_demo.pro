QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/login_dialog.cpp \
    src/main.cpp \
    src/main_wgt.cpp \
    thirdparty/NetworkManager/network_manager.cpp \
    thirdparty/NetworkManager/api_rest.cpp

HEADERS += \
  src/login_dialog.h \
  src/main_wgt.h \
  thirdparty/NetworkManager/network_manager.h \
  thirdparty/NetworkManager/api_rest.h

TRANSLATIONS += \
    src/JWT_demo_ru_RU.ts

#INCLUDEPATH += thirdparty/NetworkManager

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
