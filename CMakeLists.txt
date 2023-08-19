cmake_minimum_required (VERSION 3.24)
set(CMAKE_CXX_STANDARD 20)

project ("asynccurl" LANGUAGES CXX)
message(STATUS "using ${CMAKE_CXX_COMPILER_ID}")
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_definitions(-DASIO_HAS_STD_COROUTINE -DASIO_HAS_CO_AWAIT)
endif()


add_library(asynccurl)

target_sources(asynccurl PRIVATE executor.cpp)

find_package(asio CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)







find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(asynccurl PUBLIC
asio::asio
nlohmann_json::nlohmann_json
fmt::fmt
)



include (GNUInstallDirs)
install(TARGETS asynccurl
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

include(CPack)
