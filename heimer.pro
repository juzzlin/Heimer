# Qt project file to cross-compile the editor for Windows with MXE
# (there are problems with static Qt and CMake)

TEMPLATE = app
TARGET   = heimer

DEFINES += VERSION=\\\"1.5.0\\\"
QMAKE_CXXFLAGS += -std=gnu++11

# Qt version check
contains(QT_VERSION, ^5\\..*) {
    message("Building for Qt version $${QT_VERSION}.")
    QT += widgets xml
} else {
    error("Qt5 is required!")
}

SRC = src

INCLUDEPATH += . $$SRC/contrib/SimpleLogger/src

# Input
HEADERS +=  \
    $$SRC/aboutdlg.hpp \
    $$SRC/application.hpp \
    $$SRC/draganddropstore.hpp \
    $$SRC/graph.hpp \
    $$SRC/graphicsfactory.hpp \
    $$SRC/edge.hpp \
    $$SRC/edgebase.hpp \
    $$SRC/edgedot.hpp \
    $$SRC/edgetextedit.hpp \
    $$SRC/editordata.hpp \
    $$SRC/editorscene.hpp \
    $$SRC/editorview.hpp \
    $$SRC/fileexception.hpp \
    $$SRC/hashseed.hpp \
    $$SRC/pngexportdialog.hpp \
    $$SRC/layers.hpp \
    $$SRC/mainwindow.hpp \
    $$SRC/mediator.hpp \
    $$SRC/mindmapdata.hpp \
    $$SRC/mindmapdatabase.hpp \
    $$SRC/node.hpp \
    $$SRC/nodebase.hpp \
    $$SRC/nodehandle.hpp \
    $$SRC/reader.hpp \
    $$SRC/serializer.hpp \
    $$SRC/statemachine.hpp \
    $$SRC/textedit.hpp \
    $$SRC/undostack.hpp \
    $$SRC/writer.hpp \
    $$SRC/contrib/SimpleLogger/src/simple_logger.hpp \

SOURCES += \
    $$SRC/aboutdlg.cpp \
    $$SRC/application.cpp \
    $$SRC/draganddropstore.cpp \
    $$SRC/graph.cpp \
    $$SRC/graphicsfactory.cpp \
    $$SRC/edge.cpp \
    $$SRC/edgebase.cpp \
    $$SRC/edgedot.cpp \
    $$SRC/edgetextedit.cpp \
    $$SRC/editordata.cpp \
    $$SRC/editorscene.cpp \
    $$SRC/editorview.cpp \
    $$SRC/hashseed.cpp \
    $$SRC/pngexportdialog.cpp \
    $$SRC/main.cpp \
    $$SRC/mainwindow.cpp \
    $$SRC/mediator.cpp \
    $$SRC/mindmapdata.cpp \
    $$SRC/mindmapdatabase.cpp \
    $$SRC/node.cpp \
    $$SRC/nodebase.cpp \
    $$SRC/nodehandle.cpp \
    $$SRC/reader.cpp \
    $$SRC/serializer.cpp \
    $$SRC/statemachine.cpp \
    $$SRC/textedit.cpp \
    $$SRC/undostack.cpp \
    $$SRC/writer.cpp \
    $$SRC/contrib/SimpleLogger/src/simple_logger.cpp \


RESOURCES += data/icons/icons.qrc data/images/images.qrc data/translations/translations.qrc
RC_FILE = data/icons/WindowsHeimer.rc

data.files = AUTHORS CHANGELOG COPYING README

target.path = $$OUT_PWD/../..
INSTALLS += target

