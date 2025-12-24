cmake_minimum_required(VERSION 3.16)

project(XcomPerkEdit
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_subdirectory(xcomsave)

set(SOURCES
    main.cpp
    MainWindow.cpp
    Soldier.cpp
    Qt/CustomQt.cpp
)

set(HEADERS
    MainWindow.h
    Soldier.h
    Qt/CustomQt.h
    xcomsave/xcom.h
)

set(FORMS
    QtDesigner.ui
)

set(RESOURCES
    resources.qrc
)

add_executable(XcomPerkEdit
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
    #set_target_properties(XcomPerkEdit PROPERTIES
    #WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
    #)

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

    target_link_directories(XcomPerkEdit PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/xcomsave/build
    )

    target_link_libraries(XcomPerkEdit PRIVATE
        xcomsave
        zlib
    )
endif()

