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
    graphicsfactory.hpp \
    edge.hpp \
    edgedot.hpp \
    editordata.hpp \
    editorscene.hpp \
    editorview.hpp \
    hashseed.hpp \
    layers.hpp \
    mainwindow.hpp \
    mediator.hpp \
    mindmapdata.hpp \
    mindmapdatabase.hpp \
    node.hpp \
    nodebase.hpp \
    nodehandle.hpp \
    nodetextedit.hpp \
    serializer.hpp \
    undostack.hpp \
    writer.hpp \
    contrib/mclogger.hh \

SOURCES += \
    aboutdlg.cpp \
    application.cpp \
    draganddropstore.cpp \
    graph.cpp \
    graphicsfactory.cpp \
    edge.cpp \
    edgedot.cpp \
    editordata.cpp \
    editorscene.cpp \
    editorview.cpp \
    hashseed.cpp \
    main.cpp \
    mainwindow.cpp \
    mediator.cpp \
    mindmapdata.cpp \
    mindmapdatabase.cpp \
    node.cpp \
    nodebase.cpp \
    nodehandle.cpp \
    nodetextedit.cpp \
    serializer.cpp \
    undostack.cpp \
    writer.cpp \
    contrib/mclogger.cc \


RESOURCES += ../../data/icons/icons.qrc ../../data/images/images.qrc
RC_FILE = ../../data/icons/WindowsDementia.rc
//TRANSLATIONS += \
//    translations/dementia_fi.ts

target.path = $$OUT_PWD/../..
INSTALLS += target

