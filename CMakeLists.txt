cmake_minimum_required (VERSION 3.24)
project ("asynccurl" VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

message(STATUS "using ${CMAKE_CXX_COMPILER_ID}")
# if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_definitions(-DASIO_HAS_STD_COROUTINE -DASIO_HAS_CO_AWAIT)
# endif()
# add_definitions(-DSPDLOG_ACTIVE_LEVEL=1)


# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

add_library(asynccurl STATIC)

target_sources(asynccurl PRIVATE src/executor.cpp)

target_include_directories(asynccurl PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(asynccurl PUBLIC
nlohmann_json::nlohmann_json
fmt::fmt
spdlog::spdlog
CURL::libcurl
)

if(${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME})
  set(ENABLE_ASYNCCURL_TEST ON)
  set(ENABLE_ASYNCCURL_EXAMPLE ON)
endif()

if(${ENABLE_ASYNCCURL_EXAMPLE})
  add_subdirectory(example)
endif()

include(GNUInstallDirs)
install(TARGETS asynccurl
  EXPORT asynccurlTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set(CPACK_GENERATOR "ZIP;TGZ;TZST")
if(WIN32)
  list(APPEND CPACK_GENERATOR "NSIS")
else()
  list(APPEND CPACK_GENERATOR "DEB")
endif()
set(CPACK_SOURCE_GENERATOR "TGZ;TZST;ZIP;7Z")
set(CPACK_SOURCE_IGNORE_FILES
  "/.git/"
  "/.gitignore$"
  "/.cache/"
  "/.vscode/"
  "/build/"
)
include(CPack)

install(EXPORT asynccurlTargets
    FILE asynccurlTargets.cmake
    NAMESPACE asynccurl::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/asynccurl
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakeFindDependencyMacro)
find_dependency(fmt)
find_dependency(spdlog)
find_dependency(CURL)
find_dependency(nlohmann_json)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/asynccurlConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/asynccurlConfig.cmake
               @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/asynccurlConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/asynccurl)
