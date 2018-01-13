# Qt project file to cross-compile the editor for Windows with MXE

TEMPLATE = app
TARGET   = dementia-editor

DEFINES += DATA_PATH=\\\"./data\\\" VERSION=\\\"0.0.0\\\"
QMAKE_CXXFLAGS += -std=gnu++11

# Qt version check
contains(QT_VERSION, ^5\\..*) {
    message("Building for Qt version $${QT_VERSION}.")
    QT += widgets xml
} else {
    error("Qt5 is required!")
}

INCLUDEPATH += . contrib

# Input
HEADERS +=  \
    aboutdlg.hpp \
    application.hpp \
    config.hpp \
    draganddropstore.hpp \
    graph.hpp \
    edge.hpp \
    editordata.hpp \
    editorview.hpp \
    mainwindow.hpp \
    mediator.hpp \
    mindmapdata.hpp \
    mindmapdatabase.hpp \
    node.hpp \
    nodebase.hpp \
    nodehandle.hpp \
    undostack.hpp \
    contrib/mclogger.hh \

SOURCES += \
    aboutdlg.cpp \
    application.cpp \
    config.cpp \
    draganddropstore.cpp \
    graph.cpp \
    edge.cpp \
    editordata.cpp \
    editorview.cpp \
    main.cpp \
    mainwindow.cpp \
    mediator.cpp \
    mindmapdata.cpp \
    mindmapdatabase.cpp \
    node.cpp \
    nodebase.cpp \
    nodehandle.cpp \
    undostack.cpp \
    contrib/mclogger.cc \


RESOURCES += ../../data/icons/icons.qrc ../../data/images/images.qrc
RC_FILE = ../../data/icons/WindowsDementia.rc
//TRANSLATIONS += \
//    translations/dementia_fi.ts

target.path = $$OUT_PWD/../..
INSTALLS += target

