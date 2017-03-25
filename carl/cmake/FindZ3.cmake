# - Try to find libz3
# Once done this will define
#  LIBZ3_FOUND - System has libz3
#  LIBZ3_INCLUDE_DIRS - The libz3 include directories
#  LIBZ3_LIBRARIES - The libraries needed to use libz3

# dependencies
# -- TODO -- needed?

# find include dir by searching for a concrete file, which definetely must be in it
find_path(Z3_INCLUDE_DIR 
            NAMES src/util/z3_exception.h #exemplary file, should only be available in z3
            PATHS ENV PATH INCLUDE
            PATH_SUFFIXES z3
         )

# find library
find_library(Z3_LIBRARY 
		NAMES z3
                PATHS /usr/local/include/z3/build ENV PATH INCLUDE
            )

# set up the final variables
set(Z3_LIBRARIES ${Z3_LIBRARY})
set(Z3_INCLUDE_DIRS ${Z3_INCLUDE_DIR}/src/util)

# set the LIBZ3_FOUND variable by utilizing the following macro
# (which also handles the REQUIRED and QUIET arguments)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(z3 DEFAULT_MSG
                                  Z3_LIBRARY Z3_INCLUDE_DIR)

IF (NOT Z3_FIND_QUIETLY)
      MESSAGE(STATUS "Found Z3: ${Z3_LIBRARY}")
ENDIF (NOT Z3_FIND_QUIETLY)

# debug output to see if everything went well
#message(${Z3_INCLUDE_DIR})
#message(${Z3_LIBRARY})

# make the set variables only visible in advanced mode
mark_as_advanced(Z3_LIBRARY Z3_INCLUDE_DIR )