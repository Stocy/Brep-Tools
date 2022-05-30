cmake_minimum_required(VERSION 3.22)
project(occ_test)
# si open cascade n'est pas trouvé indiquer le chemin ci-dessous (dossier contanant OpenCASCADEConfig.cmake)
# LIST(APPEND OpenCASCADE_DIR  "/usr/lib/cmake/opencascade" ...)
find_package(OpenCASCADE REQUIRED)
find_package(OpenGL REQUIRED)
find_package(VTK)

include_directories(SYSTEM ${OpenCASCADE_INCLUDE_DIR})

set(SRC_DIR ${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SRCDIR})

set(CMAKE_CXX_STANDARD 20)

add_executable(occ_test main.cpp Utils.h)

target_compile_definitions(occ_test PRIVATE "-DSRCDIR=\"${SRC_DIR}\"")
target_link_libraries(occ_test PRIVATE "${OpenCASCADE_LIBRARIES}"
        ${OPENGL_LIBRARIES})
