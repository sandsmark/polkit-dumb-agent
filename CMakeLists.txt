cmake_minimum_required(VERSION 3.6)

project(polkit-dumb-notifier LANGUAGES CXX C)

find_package(Qt5 COMPONENTS Widgets DBus REQUIRED)
find_package(KF5Su REQUIRED)

# systemd for responder dbus
find_package(PkgConfig REQUIRED)
pkg_check_modules(systemd REQUIRED IMPORTED_TARGET libsystemd)

include(GNUInstallDirs)
set(LIBEXEC_PATH "${CMAKE_INSTALL_FULL_LIBDIR}/polkit-dumb-agent/")
add_definitions(-DLIBEXEC_DIR=\"${LIBEXEC_PATH}\")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(polkit-dumb-notifier agent.cpp)

target_link_libraries(polkit-dumb-notifier PRIVATE
    Qt5::DBus
    Qt5::Widgets
    KF5::Su
    )

add_executable(polkit-dumb-notifier-responder sendreply.c)
target_link_libraries(polkit-dumb-notifier-responder PRIVATE PkgConfig::systemd)

install(TARGETS polkit-dumb-notifier DESTINATION bin)
install(TARGETS polkit-dumb-notifier-responder
    DESTINATION "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/polkit-dumb-agent/"
    )
