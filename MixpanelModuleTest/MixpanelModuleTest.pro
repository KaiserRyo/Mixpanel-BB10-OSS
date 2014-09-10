APP_NAME = MixpanelModuleTest
BASEDIR = $$quote($$_PRO_FILE_PWD_)

CONFIG += qt warn_on cascades10
CONFIG += precompile_header

PRECOMPILED_HEADER = $$quote($$BASEDIR/precompiled.h)

QT += testlib network
LIBS += -lbbdata -lbbdevice -lbb -lbbplatform

INCLUDEPATH += ../src ../../Mixpanel/include  
SOURCES += ../src/*.cpp ../../Mixpanel/src/*.cpp
HEADERS += ../src/*.h* ../../Mixpanel/include/*.h*

include($$quote($$_PRO_FILE_PWD_)/../Mixpanel/shared.pri)



