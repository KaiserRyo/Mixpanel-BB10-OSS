APP_NAME = MixpanelExample
BASEDIR = $$quote($$_PRO_FILE_PWD_)

CONFIG += qt warn_on cascades10
CONFIG += precompile_header

PRECOMPILED_HEADER = $$quote($$BASEDIR/precompiled.h)

QT += network
LIBS += -lbbdata -lbbdevice -lbb -lbbplatform

INCLUDEPATH += ../src 
SOURCES += ../src/*.cpp 
HEADERS += ../src/*.h* 

include($$quote($$_PRO_FILE_PWD_)/../Mixpanel/shared.pri)


