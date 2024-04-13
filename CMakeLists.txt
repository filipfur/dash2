cmake_minimum_required(VERSION 3.22) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(APPLI_NAME app)

project(${APPLI_NAME})

option(LIX_BUILD_TESTS "Build the engine's test suite" OFF)
add_subdirectory(lix)
enable_testing()
add_subdirectory(hex)

file(GLOB APPLI_SOURCES
    *.cpp
    gen/*/*.cpp
)

add_executable(${APPLI_NAME} ${APPLI_SOURCES})

target_include_directories(${APPLI_NAME} PRIVATE
    include
    .
    ${HEX_INCLUDE}
)

target_link_libraries(${APPLI_NAME} PRIVATE engine)

if(NOT DEFINED ASPROC_HOME)
  message(FATAL_ERROR "ASPROC_HOME is not set.")
endif()

add_custom_target(
    GenerateAssets ALL
    COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR} -f ${ASPROC_HOME}/gen_assets.mk gen_assets ASPROC_HOME=${ASPROC_HOME}
)
add_dependencies(GenerateAssets asproc)

set_target_properties(${APPLI_NAME} PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin
)
install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)