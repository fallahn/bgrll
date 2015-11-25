# Locate NANA
#
# This module defines
# NANA_LIBRARIES - Link to this, by default it includes all NANA components
# NANA_FOUND, if false, do not try to link to NANA
# NANA_INCLUDE_DIRS, where to find the headers
#
#  This module accepts the following variables
#
#  NANA_ROOT - Can be set to NANA install path or Windows build path
#
# Created by Robert Hauck. 

MACRO(_FIND_NANA_LIBRARY _var)
  FIND_LIBRARY(${_var}
     NAMES 
        ${ARGN}
     PATHS
        ${NANA_ROOT}
     PATH_SUFFIXES lib
  )
  MARK_AS_ADVANCED(${_var})
ENDMACRO()



MACRO(_NANA_APPEND_LIBRARIES _list _release)
   SET(_debug ${_release}_DEBUG)
   IF(${_debug})
      SET(${_list} ${${_list}} optimized ${${_release}} debug ${${_debug}})
   ELSE()
      SET(${_list} ${${_list}} ${${_release}})
   ENDIF()
ENDMACRO()

FIND_PATH(NANA_INCLUDE_DIR nana/gui.hpp PATH_SUFFIXES nana)

_FIND_NANA_LIBRARY(NANA_LIBRARY_CORE        nana)
IF(WIN32)
	_FIND_NANA_LIBRARY(NANA_LIBRARY_CORE_DEBUG        nanad)
ENDIF()

# handle the QUIETLY and REQUIRED arguments and set NANA_FOUND to TRUE if all listed variables are TRUE
INCLUDE("FindPackageHandleStandardArgs")
IF(WIN32)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(NANA DEFAULT_MSG NANA_LIBRARY_CORE NANA_INCLUDE_DIR NANA_LIBRARY_CORE_DEBUG)
ELSE(WIN32)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(NANA DEFAULT_MSG NANA_LIBRARY_CORE NANA_INCLUDE_DIR)
ENDIF(WIN32)

IF(NANA_FOUND)
	_NANA_APPEND_LIBRARIES(NANA_LIBRARIES NANA_LIBRARY_CORE)
ENDIF(NANA_FOUND)

SET(NANA_INCLUDE_DIRS ${NANA_INCLUDE_DIR})