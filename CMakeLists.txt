PROJECT(REST)
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)


#minimum compiler version
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # require at least gcc 4.8
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
        message(FATAL_ERROR "GCC version must be at least 4.8!")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # require at least clang 3.2
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.2)
        message(FATAL_ERROR "Clang version must be at least 3.2!")
    endif ()
endif ()

#some cmake settings
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(ROOT_FIND_REQUIRED true)
set(CMAKE_MACOSX_RPATH 1)
IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-stdlib=libc++")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")

#install path
IF (NOT DEFINED INSTALL_PREFIX)
    IF (DEFINED ENV{REST_PATH})
        set(INSTALL_PREFIX $ENV{REST_PATH})
    else ()
        set(INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install)
    endif ()
ENDIF ()
get_filename_component(INSTALL_PREFIX_ABSOLUTE ${INSTALL_PREFIX} ABSOLUTE BASE_DIR "${CMAKE_BINARY_DIR}")
set(REST_PATH ${INSTALL_PREFIX_ABSOLUTE})

set(CMAKE_INSTALL_PREFIX ${INSTALL_PREFIX_ABSOLUTE})

#welcome message
IF (NOT DEFINED REST_WELCOME)
    set(REST_WELCOME ON)
ENDIF ()

#start compile
include(MacroRootDict)

set(rest_include_dirs)
set(rest_libraries)
set(rest_libraries_regular)
set(rest_exes)
set(rest_macros)
set(rest_macros_str)

add_subdirectory(framework)
add_subdirectory(libraries)

set(rest_libraries_regular "-L${CMAKE_INSTALL_PREFIX}/lib ")
foreach(lib ${rest_libraries})
	set(rest_libraries_regular "${rest_libraries_regular}-l${lib} ")
endforeach(lib)

#REST macros string(separate with blank)
set(rest_macros_str)
foreach(mac ${rest_macros})
	set(rest_macros_str "${rest_macros_str}${mac} ")
endforeach(mac)

message("")
message(STATUS "rest Libraries : " ${rest_libraries_regular})
message(STATUS "rest executables: " ${rest_exes})
message(STATUS "rest macros: " ${rest_macros_str})
message("")


#begin installation
message("")
message("=============================")
message("REST will be installed in : ${CMAKE_INSTALL_PREFIX}")
message("Version: ${GIT_TAG}")
message("System is : ${CMAKE_SYSTEM_NAME}")
message("=============================")
message("")

set(Headers)
foreach(dir ${rest_include_dirs})
	file(GLOB hdr "${dir}/*.h*")
	set(Headers ${hdr} ${Headers})
endforeach()

message("Headers: ${Headers}")

INSTALL(FILES ${Headers} DESTINATION include)

#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG DESTINATION .)

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/LICENCE DESTINATION .)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/scripts/restv2_1ToXml.py
        DESTINATION scripts
        COMPONENT install
        )

#adding shell executables
include(thisREST)

#install(CODE "chmod 744 ${CMAKE_INSTALL_PREFIX}/bin/rest-config")
install(CODE "execute_process(COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX}/bin/rest-config)")
install(CODE "execute_process(COMMAND chmod 755 ${CMAKE_INSTALL_PREFIX}/scripts/restv2_1ToXml.py)")
