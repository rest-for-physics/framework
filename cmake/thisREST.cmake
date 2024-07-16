# Write thisREST.[c]sh to INSTALL directory

# Check if the LCG environment was loaded. Then it will be sourced in
# thisREST.sh
set(loadLCG "")
if (DEFINED ENV{LCG_VERSION})
    # Retrieve the value of the environment variable
    set(LCG_VERSION $ENV{LCG_VERSION})

    # Print the value of the environment variable in CMake
    message(STATUS "Found LCG environment! Version: ${LCG_VERSION}")

    set(C_INCLUDE_PATH "$ENV{C_INCLUDE_PATH}")

    string(FIND "${C_INCLUDE_PATH}" ":" COLON_POSITION)

    if (COLON_POSITION GREATER_EQUAL 0)
        # Extract the substring from the beginning up to the position of the
        # first ':'
        string(SUBSTRING "${C_INCLUDE_PATH}" 0 "${COLON_POSITION}" C_CLEAN)
    else ()
        # If ':' is not found, use the entire string
        set(C_CLEAN "${C_INCLUDE_PATH}")
    endif ()

    string(REPLACE "include" "setup.sh" LCG_SETUP "${C_CLEAN}")

    # Print the modified string
    message(STATUS "Original path: ${C_CLEAN}")
    message(STATUS "Modified path: ${LCG_SETUP}")

    set(loadLCG
        "\# We load the LCG_${LCG_VERSION} environment.\necho \\\"Loading\ LCG_${LCG_VERSION}\ environment\\\"\nsource ${LCG_SETUP}\n"
    )

endif ()

# We identify the thisroot.sh script for the corresponding ROOT version
execute_process(
    COMMAND root-config --prefix
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE ROOT_PATH)
string(REGEX REPLACE "\n$" "" ROOT_PATH "${ROOT_PATH}")
set(thisROOT "${ROOT_PATH}/bin/thisroot.sh")

# We identify the geant4.sh script for the corresponding Geant4 version
execute_process(
    COMMAND geant4-config --prefix
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GEANT4_PATH)
string(REGEX REPLACE "\n$" "" GEANT4_PATH "${GEANT4_PATH}")
get_filename_component(GEANT4_BIN_DIR "${GEANT4_PATH}/bin/" REALPATH)

set(g4LibPath "")
set(loadG4 "")
if (${REST_G4} MATCHES "ON")
    # https://github.com/rest-for-physics/framework/issues/331
    set(g4LibPath ":${GEANT4_PATH}/lib/")
    set(loadG4
        "\# if geant4.sh script is found we load the same Geant4 version as used in compilation\nif [[ -f \\\"${GEANT4_BIN_DIR}/geant4.sh\\\" ]]; then
    [[ -n \\\"\\\${ZSH_VERSION}\\\" ]] && pushd ${GEANT4_BIN_DIR} > /dev/null\n    source ${GEANT4_BIN_DIR}/geant4.sh\n    [[ -n \\\"\\\${ZSH_VERSION}\\\" ]] && popd > /dev/null\nfi\n"
    )
endif ()

set(loadMPFR "")
if (DEFINED MPFR_PATH)
    set(loadMPFR "export LD_LIBRARY_PATH=${MPFR_PATH}/lib:\$LD_LIBRARY_PATH")
endif ()

set(loadCRY "")
if (DEFINED REST_CRY_PATH)
    set(loadCRY "export LD_LIBRARY_PATH=${REST_CRY_PATH}/lib:\$LD_LIBRARY_PATH")
endif ()

set(loadGarfield "")
set(Garfield_INCLUDE_ENV "")
if (${REST_GARFIELD} MATCHES "ON")
    if (DEFINED ENV{GARFIELD_INSTALL})
        # this is the recommended way to source newer Garfield installations
        set(loadGarfield
            "
# if GARFIELD is enabled we load the same Garfield environment used in compilation
source $ENV{GARFIELD_INSTALL}/share/Garfield/setupGarfield.sh
")
        set(Garfield_INSTALL "$ENV{GARFIELD_INSTALL}")
    else ()
        set(loadGarfield
            "
# if GARFIELD is enabled we load the same Garfield environment used in compilation
export GARFIELD_HOME=$ENV{GARFIELD_HOME}
export HEED_DATABASE=\$GARFIELD_HOME/Heed/heed++/database
export LD_LIBRARY_PATH=\$GARFIELD_HOME/lib:\$LD_LIBRARY_PATH
")
        set(Garfield_INSTALL "$ENV{GARFIELD_HOME}")
    endif ()
    set(Garfield_INCLUDE_ENV ":$ENV{GARFIELD_INSTALL}/include")
endif ()

file(STRINGS
     "${CMAKE_CURRENT_SOURCE_DIR}/source/framework/core/inc/TRestVersion.h"
     lines)

message(STATUS "########## Latest release info #############")

string(FIND "${lines}" "REST_RELEASE " FOUND_RELEASE)
string(SUBSTRING "${lines}" ${FOUND_RELEASE} 50 release)
string(SUBSTRING "${release}" 14 30 release)
string(FIND "${release}" "\"" FOUND_RELEASE)
string(SUBSTRING "${release}" 0 ${FOUND_RELEASE} release)
message(STATUS "Release version : ${release}")

string(FIND "${lines}" "REST_RELEASE_NAME" FOUND_RELEASE_NAME)
string(SUBSTRING "${lines}" ${FOUND_RELEASE_NAME} 50 releaseName)
string(SUBSTRING "${releaseName}" 19 30 releaseName)
string(FIND "${releaseName}" "\"" FOUND_RELEASE_NAME)
string(SUBSTRING "${releaseName}" 0 ${FOUND_RELEASE_NAME} releaseName)
message(STATUS "Release name : ${releaseName}")

string(FIND "${lines}" "REST_RELEASE_DATE" FOUND_RELEASE_DATE)
string(SUBSTRING "${lines}" ${FOUND_RELEASE_DATE} 50 releaseDate)
string(SUBSTRING "${releaseDate}" 19 30 releaseDate)
string(FIND "${releaseDate}" "\"" FOUND_RELEASE_DATE)
string(SUBSTRING "${releaseDate}" 0 ${FOUND_RELEASE_DATE} releaseDate)
message(STATUS "Release date : ${releaseDate}")
message(STATUS "########## Latest release info #############")
message("")

# install thisREST script, sh VERSION
install(
    CODE "
file( WRITE \${CMAKE_INSTALL_PREFIX}/thisREST.sh

\"\#!/bin/bash

${loadLCG}

\# check active shell by checking for existence of _VERSION variable
if [[ -n \\\"\\\${BASH_VERSION}\\\" ]]; then
    thisdir=\\\$(cd \\\$(dirname \\\${BASH_ARGV[0]}); pwd)
elif [[ -n \\\"\\\${ZSH_VERSION}\\\" ]]; then
    thisdir=\\\$(cd \\\$(dirname \\\$0); pwd)
else
    echo \\\"Invalid shell! Either source with bash or zsh!\\\"
    return 1
fi

\# if thisroot.sh script is found we load the same ROOT version as used in compilation
if [[ -f \\\"${thisROOT}\\\" ]]; then
    source ${thisROOT}
fi

${loadG4}
${loadMPFR}
${loadCRY}
${loadGarfield}

if [ \\\$REST_PATH ] ; then
    echo switching to REST installed in \\\${thisdir}
    _PATH=`echo \\\$PATH | sed -e \\\"s\#\\\${REST_PATH}/bin:\#\#g\\\"`
    _LD_LIBRARY_PATH=`echo \\\$LD_LIBRARY_PATH | sed -e \\\"s\#\\\${REST_PATH}/lib:\#\#g\\\"`
else
    _PATH=\\\$PATH
    _LD_LIBRARY_PATH=\\\$LD_LIBRARY_PATH
fi

export REST_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}
export REST_PATH=\\\${thisdir}
# REST_HOME is where temporary files are stored
export REST_HOME=\\\${HOME}
export ROOT_INCLUDE_PATH=\\\$REST_PATH/include${Garfield_INCLUDE_ENV}:\\\$ROOT_INCLUDE_PATH
export REST_INPUTDATA=\\\$REST_PATH/data
export REST_GARFIELD_INCLUDE=${Garfield_INCLUDE_DIRS}
export REST_GARFIELD_LIB=${Garfield_LIBRARIES}
export PATH=\\\$REST_PATH/bin:\\\$_PATH
export LD_LIBRARY_PATH=\\\$REST_PATH/lib:\\\$_LD_LIBRARY_PATH
# DYLD_LIBRARY_PATH is required by MacOs
export DYLD_LIBRARY_PATH=\\\$REST_PATH/lib:\\\$LD_LIBRARY_PATH${g4LibPath}
export LIBRARY_PATH=\\\$REST_PATH/lib:\\\$LIBRARY_PATH
export PYTHONPATH=${PYTHON_BINDINGS_INSTALL_DIR}:\\\$PYTHONPATH

alias restRoot=\\\"restRoot -l\\\"
alias restRootMacros=\\\"restRoot -l --m\\\"
alias restListMacros=\\\"restManager ListMacros\\\"

if [ \\\$(rest-config --flags | grep \\\"REST_WELCOME=ON\\\") ]; then
    rest-config --welcome
fi

# REST aliases
\"
)
        ")

foreach (mac ${rest_macros})

    string(REPLACE " " "" mac ${mac})
    string(REPLACE "rest" "" m ${mac})

    install(
        CODE "
file( APPEND \${CMAKE_INSTALL_PREFIX}/thisREST.sh
\"alias ${mac}=\\\"restManager ${m}\\\"
\"
)
        ")

endforeach (mac ${rest_macros})

# install thisREST script, csh VERSION
install(
    CODE "
file( WRITE \${CMAKE_INSTALL_PREFIX}/thisREST.csh

\"\#!/bin/csh

setenv REST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}
setenv REST_PATH ${CMAKE_INSTALL_PREFIX}
setenv REST_INPUTDATA \\\$REST_PATH/data
setenv ROOT_INCLUDE_DIR \\\$REST_PATH/include
if ( \\\$?ROOT_INCLUDE_PATH == 0 ) setenv ROOT_INCLUDE_PATH
setenv ROOT_INCLUDE_PATH \\\${ROOT_INCLUDE_PATH}:\\\$REST_PATH/include
setenv REST_GARFIELD_INCLUDE ${Garfield_INCLUDE_DIRS}
setenv REST_GARFIELD_LIB ${Garfield_LIBRARIES}
setenv PATH \\\$REST_PATH/bin:\\\$PATH
setenv LD_LIBRARY_PATH \\\$REST_PATH/lib:\\\$LD_LIBRARY_PATH
if ( \\\$?LIBRARY_PATH == 0 ) setenv LIBRARY_PATH
setenv LIBRARY_PATH \\\${LIBRARY_PATH}:\\\$REST_PATH/lib
setenv PYTHONPATH \\\${PYTHONPATH}:${PYTHON_BINDINGS_INSTALL_DIR}

if ( `rest-config --flags | grep -c \\\"REST_WELCOME=ON\\\"` ) then
rest-config --welcome
endif
\"
)
        ")

foreach (mac ${rest_macros})

    string(REPLACE " " "" mac ${mac})
    string(REPLACE "rest" "" m ${mac})

    install(
        CODE "
file( APPEND \${CMAKE_INSTALL_PREFIX}/thisREST.csh
\"alias ${mac} \\\"restManager ${m}\\\"
\"
)
        ")

endforeach (mac ${rest_macros})

# install rest-config
install(
    CODE "
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CollectGitInfo.cmake)

message(STATUS \"Installing: \${CMAKE_INSTALL_PREFIX}/bin/rest-config\")

file( WRITE \${CMAKE_INSTALL_PREFIX}/bin/rest-config

\"

if [ $# -ne 1 ] ; then

echo \\\"  Use restRoot command to load REST libraries and scripts inside ROOT          \\\"
echo \\\"  Use restManager command to manage the configurations and start REST          \\\"
echo \\\"  Type \\\\\\\"rest-config --help\\\\\\\" for more info                        \\\"

else

option=$1

if [ $option = \\\"--incdir\\\" ] ; then
echo ${CMAKE_INSTALL_PREFIX}/include

fi

if [ $option = \\\"--libdir\\\" ] ; then
echo ${CMAKE_INSTALL_PREFIX}/lib

fi

if [ $option = \\\"--libs\\\" ] ; then
echo ${rest_libraries_regular}

fi

if [ $option = \\\"--exes\\\" ] ; then
echo ${rest_exes}
echo -------------------------------------------
echo ${rest_macros_str}

fi

if [ $option = \\\"--version\\\" ] ; then
echo \${GIT_TAG}

fi

if [ $option = \\\"--prefix\\\" ] ; then
echo ${CMAKE_INSTALL_PREFIX}

fi

if [ $option = \\\"--commit\\\" ] ; then
echo \${GIT_COMMIT}

fi

if [ $option = \\\"--clean\\\" ] ; then
echo \${GIT_CLEANSTATE}

fi

if [ $option = \\\"--release\\\" ] ; then
echo \${REST_OFFICIAL_RELEASE}

fi

if [ $option = \\\"--flags\\\" ] ; then
echo REST_WELCOME=${REST_WELCOME}
echo REST_GARFIELD=${REST_GARFIELD}
echo REST_G4=${REST_G4}
echo REST_DATABASE=${REST_DATABASE}
echo CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
echo CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}

fi

if [ $option = \\\"--welcome\\\" ] ; then

if [ -t 0 ] ; then

echo \\\"  *****************************************************************************\\\"
echo \\\"  W E L C O M E   to  R E S T  \\\"
echo \\\"  \\\"
echo \\\"  Commit  : \${GIT_COMMIT} (\${GIT_DATE})  \\\"
echo \\\"  Branch/Version : \${GIT_BRANCH}/\${GIT_TAG}  \\\"
echo \\\"  Compilation date : ${date}  \\\"
echo \\\"  \\\"
echo \\\"  Latest release: : v${release} - ${releaseName} - ${releaseDate}  \\\"
echo \\\"  \\\"
echo \\\"  Official release : \${REST_OFFICIAL_RELEASE} \\\"
echo \\\"  Clean state : \${GIT_CLEANSTATE} \\\"
echo \\\"  \\\"
echo \\\"  Installed at : \${CMAKE_INSTALL_PREFIX}  \\\"
echo \\\"  \\\"
echo \\\"  REST-for-Physics site : rest-for-physics.github.io  \\\"
echo \\\"  \\\"
echo \\\"  Remember that REST is made by physicists for physicists, \\\"
echo \\\"  who are supposed to toil and suffer till they become experts. \\\"
echo \\\"  *****************************************************************************\\\"
echo \\\"  \\\"

fi

fi

if [ $option = \\\"--help\\\" ] ; then
echo \\\"  Usage :                                                                                              \\\"
echo \\\"  rest-config [--version] : Prints the version of REST                                                 \\\"
echo \\\"  rest-config [--prefix]  : Prints REST installation directory                                         \\\"
echo \\\"  rest-config [--incdir]  : Shows the directory of headers                                             \\\"
echo \\\"  rest-config [--libdir]  : Shows the directory of library                                             \\\"
echo \\\"  rest-config [--libs]    : Prints regular REST libraries                                              \\\"
echo \\\"  rest-config [--exes]    : Prints a list of REST executables with alias                               \\\"
echo \\\"  rest-config [--welcome] : Prints the welcome message                                                 \\\"
echo \\\"  rest-config [--flags]   : Prints cmake flags defined when installing                                 \\\"
echo \\\"  rest-config [--release] : Prints 'Yes' if the compilation corresponds with an official git tag.      \\\"
echo \\\"  rest-config [--clean]   : Prints 'Yes' if no local modifications were found during compilation       \\\"
fi

fi


\"
)
        ")
