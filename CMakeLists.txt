cmake_minimum_required(VERSION 3.5)

project(polkit-dumb-notifier LANGUAGES CXX)

find_package(Qt5 COMPONENTS Widgets DBus REQUIRED)
find_package(KF5Su REQUIRED)
find_package(KF5WidgetsAddons REQUIRED)

include(GNUInstallDirs)
add_definitions(-DLIBEXEC_DIR=\"${CMAKE_INSTALL_FULL_LIBEXECDIR}\")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(polkit-dumb-notifier agent.cpp sudlg.cpp)

target_link_libraries(polkit-dumb-notifier PRIVATE KF5::Su Qt5::Widgets Qt5::DBus KF5::WidgetsAddons)
install(TARGETS polkit-dumb-notifier DESTINATION bin)
