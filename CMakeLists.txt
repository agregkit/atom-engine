cmake_minimum_required(VERSION 3.1)

project(swap_server)

include(FindPackageHandleStandardArgs)

# find libevent

if (EVENT_INCLUDE_DIR AND EVENT_LIBRARY)
  # Already in cache, be silent
  set(EVENT_FIND_QUIETLY TRUE)
endif (EVENT_INCLUDE_DIR AND EVENT_LIBRARY)

find_path(EVENT_INCLUDE_DIR event.h
  PATHS /usr/include
  PATH_SUFFIXES event
)

find_library(EVENT_LIBRARY
  NAMES event
  PATHS /usr/lib /usr/local/lib
)

set(EVENT_LIBRARIES ${EVENT_LIBRARY} )

add_definitions(-DLIBNET_LIL_ENDIAN)

find_package_handle_standard_args(EVENT
  DEFAULT_MSG
  EVENT_INCLUDE_DIR
  EVENT_LIBRARIES
)

mark_as_advanced(EVENT_INCLUDE_DIR EVENT_LIBRARY)

# find hiredis

find_path(
  HIREDIS_INCLUDE_DIR
  NAMES hiredis/hiredis.h
  HINTS ${HIREDIS_ROOT_DIR}
  PATH_SUFFIXES include)

find_library(
  HIREDIS_LIBRARY
  NAMES hiredis
  HINTS ${HIREDIS_ROOT_DIR}
  PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR})

set(HIREDIS_INCLUDE_DIRS ${HIREDIS_INCLUDE_DIR})
set(HIREDIS_LIBRARIES ${HIREDIS_LIBRARY})

find_package_handle_standard_args(hiredis 
  DEFAULT_MSG 
  HIREDIS_LIBRARY 
  HIREDIS_INCLUDE_DIR
)

mark_as_advanced(HIREDIS_LIBRARY HIREDIS_INCLUDE_DIR)

# set project

include_directories(include)
include_directories(include/requests)
include_directories(libs/rapidjson)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(CPP_FILES 
	src/main.cpp
	src/swapserver.cpp
  src/requestsprocessor.cpp
  src/requestsprocessormt.cpp
  src/requestsworker.cpp
  src/requestsworkermt.cpp
  src/requests/request.cpp
  src/requestfactory.cpp
  src/requests/requestneworder.cpp
  src/requests/requestnewtrade.cpp
  src/requests/requestorders.cpp
  src/requests/requesttrades.cpp
)

add_executable(swap_server ${CPP_FILES})

target_link_libraries(swap_server ${EVENT_LIBRARY} ${HIREDIS_LIBRARIES})