project(bgrll)
cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
SET(bgrll_STATIC_SFML FALSE CACHE BOOL "Choose whether SFML is linked statically or not.")
SET(bgrll_STATIC_RUNTIME FALSE CACHE BOOL "Use statically linked standard/runtime libraries? This option must match the one used for SFML.")

if(CMAKE_COMPILER_IS_GNUCXX)
  if(bgrll_STATIC_RUNTIME)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y -static")
  else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y")
  endif()
endif()

#removes the console window when building with MinGW
if(MINGW AND ${CMAKE_BUILD_TYPE} EQUAL Release)
  add_definitions(SUBSYSTEM_WINDOWS)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y -Wl,--subsystem,windows")
endif()

if(bgrll_STATIC_SFML)
  SET(SFML_STATIC_LIBRARIES TRUE)
endif()


find_package(SFML 2 REQUIRED graphics window system)
find_package(Nana REQUIRED)

if(UNIX)
  find_package(X11 REQUIRED)
  find_package(Threads REQUIRED)
  add_definitions(-DNANA_UNICODE)
endif()


include_directories(
  ${SFML_INCLUDE_DIR}
  ${NANA_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include)

if(X11_FOUND)
  include_directories(${X11_INCLUDE_DIRS})
endif()

#if using mingw on windows compile the icon / resource files
#set(RES_FILES "")
#if(MINGW)
#  set(RES_FILES "icon.rc")
#  set(CMAKE_RC_COMPILER_INIT windres)
#  enable_language(RC)
#  set(CMAKE_RC_COMPILE_OBJECT
#  "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")
#endif()

SET(bgrll_DIR ${CMAKE_SOURCE_DIR}/src)
include(${bgrll_DIR}/CMakeLists.txt)

#  ${RES_FILES}
  
if(WIN32)
  add_executable(bgrll WIN32 ${bgrll_SRC})
else()
  add_executable(bgrll ${bgrll_SRC})
endif()

target_link_libraries(bgrll
  ${SFML_LIBRARIES}
  ${NANA_LIBRARIES}
  ${SFML_DEPENDENCIES})

if(UNIX)
  target_link_libraries(bgrll
    ${X11_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    -lXft)
endif()

#install executable
install(TARGETS bgrll
  RUNTIME DESTINATION .)

#install asset data
install(DIRECTORY assets
  DESTINATION .)

