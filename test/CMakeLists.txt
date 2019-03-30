add_library(Catch INTERFACE)
set(CATCH_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/catch2)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

add_executable(tests ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(tests Catch)