# File: macros.cmake
# Authors: Igor N. Bongartz
# Erstellt: 2015-06-11
# Version: 2015-06-11
#
# This file contains several macros which are used in this project. Notice that several are copied straight from web ressources.

macro(add_imported_library_interface name include)
	add_library(${name} INTERFACE IMPORTED)
	set_target_properties(${name} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${include}")
endmacro(add_imported_library_interface)

macro(add_imported_library name type lib include)
	# Workaround from https://cmake.org/Bug/view.php?id=15052
	file(MAKE_DIRECTORY "${include}")
	if("${lib}" STREQUAL "")
		if("${type}" STREQUAL "SHARED")
			add_library(${name} INTERFACE IMPORTED)
			set_target_properties(${name} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${include}")
		endif()
	else()
		add_library(${name}_${type} ${type} IMPORTED)
		set_target_properties(${name}_${type} PROPERTIES IMPORTED_LOCATION "${lib}")
		set_target_properties(${name}_${type} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${include}")
	endif()
endmacro(add_imported_library)

macro(export_option name)
	list(APPEND EXPORTED_OPTIONS "${name}")
endmacro(export_option)

macro(export_target output TARGET)
	get_target_property(TYPE ${TARGET} TYPE)
	if(TYPE STREQUAL "SHARED_LIBRARY")
		get_target_property(LOCATION ${TARGET} IMPORTED_LOCATION)
		get_target_property(INCLUDE ${TARGET} INTERFACE_INCLUDE_DIRECTORIES)
		set(${output} "${${output}}
add_library(${TARGET} SHARED IMPORTED)
set_target_properties(${TARGET} PROPERTIES IMPORTED_LOCATION \"${LOCATION}\")
set_target_properties(${TARGET} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES \"${INCLUDE}\")
")
	elseif(TYPE STREQUAL "STATIC_LIBRARY")
		get_target_property(LOCATION ${TARGET} IMPORTED_LOCATION)
		get_target_property(INCLUDE ${TARGET} INTERFACE_INCLUDE_DIRECTORIES)
		set(${output} "${${output}}
add_library(${TARGET} STATIC IMPORTED)
set_target_properties(${TARGET} PROPERTIES IMPORTED_LOCATION \"${LOCATION}\")
set_target_properties(${TARGET} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES \"${INCLUDE}\")
")
		if(NOT "${ARGN}" STREQUAL "")
			set(${output} "${${output}}set_target_properties(${TARGET} PROPERTIES IMPORTED_LINK_INTERFACE_LIBRARIES \"${ARGN}\")
")
		endif()
	elseif(TYPE STREQUAL "INTERFACE_LIBRARY")
		get_target_property(INCLUDE ${TARGET} INTERFACE_INCLUDE_DIRECTORIES)
		set(${output} "${${output}}
add_library(${TARGET} INTERFACE IMPORTED)
set_target_properties(${TARGET} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES \"${INCLUDE}\")
")
	else()
		message(STATUS "Unknown type ${TYPE}")
	endif()
endmacro(export_target)

macro(export_dependency output TARGET DEP)
	set(${output} "${${output}}set_target_properties(${TARGET} PROPERTIES IMPORTED_LINK_INTERFACE_LIBRARIES \"${DEP}\")
")
endmacro(export_dependency)

macro(load_library group name version)
    string(TOUPPER ${name} LIBNAME)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ${DYNAMIC_EXT})
    set(Boost_USE_STATIC_LIBS OFF)
    find_package(${name} ${version} ${ARGN} QUIET)
    if(${name}_FOUND OR ${LIBNAME}_FOUND)
        if (${name}_FOUND)
            set(LIBNAME ${name})
        endif()
		
		add_imported_library(${LIBNAME} SHARED "${${LIBNAME}_LIBRARY}" "${${LIBNAME}_INCLUDE_DIR}")

        unset(${LIBNAME}_FOUND CACHE)
        unset(${LIBNAME}_INCLUDE_DIR CACHE)
        unset(${LIBNAME}_LIBRARY CACHE)

        set(CMAKE_FIND_LIBRARY_SUFFIXES ${STATIC_EXT})
        set(Boost_USE_STATIC_LIBS ON)
        if (ARGN)
            list(REMOVE_ITEM ARGN "REQUIRED")
        endif()
        find_package(${name} ${version} ${ARGN} QUIET)
        if(${LIBNAME}_FOUND)
			add_imported_library(${LIBNAME} STATIC "${${LIBNAME}_LIBRARY}" "${${LIBNAME}_INCLUDE_DIR}")
        endif()

        unset(${LIBNAME}_LIBRARY CACHE)
    else()
        message("-- Library ${name} was not found.")
    endif()
endmacro(load_library)

macro(get_include_dir var name)
	get_target_property(INCLUDE_DIR ${name} INTERFACE_INCLUDE_DIRECTORIES)
	set(${var} ${${var}}:${INCLUDE_DIR})
endmacro(get_include_dir)
