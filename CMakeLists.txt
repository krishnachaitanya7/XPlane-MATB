project(shine-lab-plugin)
set(CMAKE_CXX_STANDARD 14)
cmake_minimum_required(VERSION 3.14)
## Setting Up Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
# End
# Cmake Code for XPlane
include_directories("/usr/local/include")
set(CMAKE_CXX_FLAGS "-shared -rdynamic -nodefaultlibs -undefined_warning -fPIC -Wall -Wextra -std=c++0x -pthread")
add_definitions(-DAPL=0 -DIBM=0 -DLIN=1 -DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1)
# End
# CMake for Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(project_sources XPlane_matb.cpp)
#set(project_headers mainwindow.h)
#set(project_ui mainwindow.ui)
#qt5_wrap_ui(ui_wrap ${project_ui})
#qt5_wrap_cpp(moc_sources ${project_headers})
add_executable(qt_xplane_test
        ${project_sources}
        )
target_link_libraries(qt_xplane_test)