cmake_minimum_required(VERSION 3.20)
project(PGEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)
find_package(SDL3_mixer REQUIRED)

find_package(Eigen3 CONFIG REQUIRED)
find_package(cereal CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)

file(GLOB_RECURSE ENGINE_SOURCES
  CONFIGURE_DEPENDS
  "src/*.cpp"
)

add_library(PGEngine STATIC ${ENGINE_SOURCES})

target_link_libraries(PGEngine PUBLIC
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
    Eigen3::Eigen
    cereal::cereal
    CLI11::CLI11
)

target_include_directories(PGEngine PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(GNUInstallDirs)

install(TARGETS PGEngine
    EXPORT PGEngineTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pandorica_graphics)

install(EXPORT PGEngineTargets
    FILE PGEngineTargets.cmake
    NAMESPACE PGEngine::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PGEngine
)
