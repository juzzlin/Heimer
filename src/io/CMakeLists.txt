# Set sources for the lib
set(LIB_SRC
    alz_file_io.cpp
    alz_file_io_worker.cpp
    file_exception.hpp
    file_io.hpp
    xml_reader.cpp
    xml_writer.cpp
)

# Add the library
add_library(${IO_LIB_NAME} STATIC ${LIB_SRC})
target_link_libraries(${IO_LIB_NAME} Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Xml)
