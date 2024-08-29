cmake_minimum_required(VERSION 3.29)

project(multi350)
set(LIB_NAME multi350)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

add_library(${LIB_NAME} STATIC
src/controller.cpp
src/dlpc350.cpp
src/status.cpp
src/usb.cpp
)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

include(GetHidapi)
target_link_libraries(${LIB_NAME} PUBLIC hidapi::hidapi)

# ### old code to include hidapi headers
# find_path(hidapi_INCLUDE_DIR NAMES hidapi.h PATH_SUFFIXES hidapi)
# target_include_directories(${LIB_NAME} PUBLIC ${hidapi_INCLUDE_DIR})

# ### old code to include & link hidapi from binary distribution
# target_include_directories(${LIB_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/hidapi/include")
# target_link_libraries(${LIB_NAME} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/hidapi/x64/hidapi.lib")

set_target_properties(${LIB_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)