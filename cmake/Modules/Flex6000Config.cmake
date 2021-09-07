INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_FLEX6000 Flex6000)

FIND_PATH(
    FLEX6000_INCLUDE_DIRS
    NAMES Flex6000/api.h
    HINTS $ENV{FLEX6000_DIR}/include
        ${PC_FLEX6000_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    FLEX6000_LIBRARIES
    NAMES gnuradio-Flex6000
    HINTS $ENV{FLEX6000_DIR}/lib
        ${PC_FLEX6000_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/Flex6000Target.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FLEX6000 DEFAULT_MSG FLEX6000_LIBRARIES FLEX6000_INCLUDE_DIRS)
MARK_AS_ADVANCED(FLEX6000_LIBRARIES FLEX6000_INCLUDE_DIRS)
