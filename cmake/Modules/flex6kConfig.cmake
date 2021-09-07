INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_FLEX6K flex6k)

FIND_PATH(
    FLEX6K_INCLUDE_DIRS
    NAMES flex6k/api.h
    HINTS $ENV{FLEX6K_DIR}/include
        ${PC_FLEX6K_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    FLEX6K_LIBRARIES
    NAMES gnuradio-flex6k
    HINTS $ENV{FLEX6K_DIR}/lib
        ${PC_FLEX6K_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/flex6kTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FLEX6K DEFAULT_MSG FLEX6K_LIBRARIES FLEX6K_INCLUDE_DIRS)
MARK_AS_ADVANCED(FLEX6K_LIBRARIES FLEX6K_INCLUDE_DIRS)
