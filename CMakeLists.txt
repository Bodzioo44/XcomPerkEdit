cmake_minimum_required(VERSION 3.16)

project(XcomPerkEdit
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "ui")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_subdirectory(lib/xcomsave)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/Soldier.cpp
    src/widgets/CustomQt.cpp
)

set(HEADERS
    include/MainWindow.h
    include/Soldier.h
    include/widgets/CustomQt.h
    lib/xcomsave/xcom.h
)

# set(AUTOUIC_SEARCH_PATHS "ui")

set(FORMS
    ui/QtDesigner.ui
)

set(RESOURCES
    res/resources.qrc
)



add_executable(XcomPerkEdit
    WIN32
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${RESOURCES}
)

target_include_directories(XcomPerkEdit PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(XcomPerkEdit PRIVATE
    Qt6::Widgets
)

if (WIN32)
    target_compile_options(XcomPerkEdit PRIVATE
        /W3
        /Wall
    )

    target_link_libraries(XcomPerkEdit PRIVATE
        xcomsave
        zlib
    )
endif()

if (UNIX AND NOT APPLE)
    target_compile_options(XcomPerkEdit PRIVATE
        -Wall
        -Wextra
        -Wno-missing-field-initializers
        -Wno-attributes
    )

    target_link_libraries(XcomPerkEdit PRIVATE
        xcomsave
        zlib
    )
endif()

