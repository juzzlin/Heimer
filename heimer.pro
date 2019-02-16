# Qt project file to cross-compile the editor for Windows with MXE
# (there are problems with static Qt and CMake)

TEMPLATE = app
TARGET   = heimer

DEFINES += VERSION=\\\"1.7.0\\\"
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
    $$SRC/about_dlg.hpp \
    $$SRC/application.hpp \
    $$SRC/drag_and_drop_store.hpp \
    $$SRC/graph.hpp \
    $$SRC/graphics_factory.hpp \
    $$SRC/edge.hpp \
    $$SRC/edge_base.hpp \
    $$SRC/edge_dot.hpp \
    $$SRC/edge_text_edit.hpp \
    $$SRC/editor_data.hpp \
    $$SRC/editor_scene.hpp \
    $$SRC/editor_view.hpp \
    $$SRC/file_exception.hpp \
    $$SRC/hash_seed.hpp \
    $$SRC/png_export_dialog.hpp \
    $$SRC/layers.hpp \
    $$SRC/magic_zoom.hpp \
    $$SRC/main_window.hpp \
    $$SRC/mediator.hpp \
    $$SRC/mind_map_data.hpp \
    $$SRC/mind_map_data_base.hpp \
    $$SRC/node.hpp \
    $$SRC/node_base.hpp \
    $$SRC/node_handle.hpp \
    $$SRC/reader.hpp \
    $$SRC/serializer.hpp \
    $$SRC/state_machine.hpp \
    $$SRC/text_edit.hpp \
    $$SRC/undo_stack.hpp \
    $$SRC/writer.hpp \
    $$SRC/contrib/SimpleLogger/src/simple_logger.hpp \

SOURCES += \
    $$SRC/about_dlg.cpp \
    $$SRC/application.cpp \
    $$SRC/drag_and_drop_store.cpp \
    $$SRC/graph.cpp \
    $$SRC/graphics_factory.cpp \
    $$SRC/edge.cpp \
    $$SRC/edge_base.cpp \
    $$SRC/edge_dot.cpp \
    $$SRC/edge_text_edit.cpp \
    $$SRC/editor_data.cpp \
    $$SRC/editor_scene.cpp \
    $$SRC/editor_view.cpp \
    $$SRC/hash_seed.cpp \
    $$SRC/png_export_dialog.cpp \
    $$SRC/magic_zoom.cpp \
    $$SRC/main.cpp \
    $$SRC/main_window.cpp \
    $$SRC/mediator.cpp \
    $$SRC/mind_map_data.cpp \
    $$SRC/mind_map_data_base.cpp \
    $$SRC/node.cpp \
    $$SRC/node_base.cpp \
    $$SRC/node_handle.cpp \
    $$SRC/reader.cpp \
    $$SRC/serializer.cpp \
    $$SRC/state_machine.cpp \
    $$SRC/text_edit.cpp \
    $$SRC/undo_stack.cpp \
    $$SRC/writer.cpp \
    $$SRC/contrib/SimpleLogger/src/simple_logger.cpp \


RESOURCES += data/icons/icons.qrc data/images/images.qrc data/translations/translations.qrc
RC_FILE = data/icons/WindowsHeimer.rc

data.files = AUTHORS CHANGELOG COPYING README

target.path = $$OUT_PWD/../..
INSTALLS += target

