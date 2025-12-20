cmake_minimum_required(VERSION 3.16)

project(XcomPerkEdit
    LANGUAGES CXX
)

# ---- C++ standard ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Qt automatic processing ----
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


# ---- Find Qt ----
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# ---- Sources ----
set(SOURCES
    main.cpp
    MainWindow.cpp
    Soldier.cpp
    Qt/CustomQt.cpp
)

set(HEADERS
    MainWindow.h
    Soldier.h
#    ui_QtDesigner.h
    Qt/CustomQt.h
    xcomsave/xcom.h
)

set(FORMS
    QtDesigner.ui
)

set(RESOURCES
    resources.qrc
)

# ---- Executable ----
add_executable(XcomPerkEdit
    ${SOURCES}
    ${HEADERS}
    ${FORMS}
    ${RESOURCES}
)

# ---- Include paths ----
target_include_directories(XcomPerkEdit PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ---- Qt libraries ----
target_link_libraries(XcomPerkEdit PRIVATE
    Qt6::Widgets
)

# ---- Platform-specific settings ----
if (WIN32)
    target_compile_options(XcomPerkEdit PRIVATE
        /W3
        /Wall
    )

    target_link_directories(XcomPerkEdit PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/xcomsave/out/build/x64-Release
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

