# Write thisREST.[c]sh to INSTALL directory

# sh VERSION

install( CODE
"
file( WRITE \${CMAKE_INSTALL_PREFIX}/thisREST.sh 

\"\#!/bin/bash

export REST_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}
export REST_PATH=${CMAKE_INSTALL_PREFIX}
export REST_INPUTDATA=\\\$REST_PATH/data
export REST_GARFIELD_INCLUDE=${Garfield_INCLUDE_DIRS}
export REST_GARFIELD_LIB=${Garfield_LIBRARIES}
export PATH=\\\$REST_PATH/bin:\\\$PATH
export LD_LIBRARY_PATH=\\\$REST_PATH/lib:\\\$LD_LIBRARY_PATH
export LIBRARY_PATH=\\\$LIBRARY_PATH:\\\$REST_PATH/lib

\#alias restRoot=\\\"root -l \\\$REST_PATH/scripts/LoadRESTScripts.C\\\"

if [ \\\$(rest-config --flags | grep \\\"REST_WELCOME=ON\\\") ];then
rest-config --welcome
fi
\"
)
        "
)


foreach(mac ${rest_macros})

string(REPLACE " " "" mac ${mac})
string(REPLACE "rest" "" m ${mac})

install( CODE
"
file( APPEND \${CMAKE_INSTALL_PREFIX}/thisREST.sh 

\"
alias ${mac}=\\\"restManager ${m}\\\"
\"
)
        "
)

endforeach(mac ${rest_macros})

# csh VERSION

install( CODE
"
file( WRITE \${CMAKE_INSTALL_PREFIX}/thisREST.csh 

\"\#!/bin/csh

setenv REST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}
setenv REST_PATH ${CMAKE_INSTALL_PREFIX}
setenv REST_INPUTDATA \\\$REST_PATH/data
setenv REST_GARFIELD_INCLUDE ${Garfield_INCLUDE_DIRS}
setenv REST_GARFIELD_LIB ${Garfield_LIBRARIES}
setenv PATH \\\$REST_PATH/bin:\\\$PATH
setenv LD_LIBRARY_PATH \\\$REST_PATH/lib:\\\$LD_LIBRARY_PATH
if ( \\\$?LIBRARY_PATH == 0 ) setenv LIBRARY_PATH
setenv LIBRARY_PATH \\\${LIBRARY_PATH}:\\\$REST_PATH/lib

if ( `rest-config --flags | grep -c \\\"REST_WELCOME=ON\\\"` ) then
rest-config --welcome
endif
\"
)
        "
)


foreach(mac ${rest_macros})

string(REPLACE " " "" mac ${mac})
string(REPLACE "rest" "" m ${mac})

install( CODE
"
file( APPEND \${CMAKE_INSTALL_PREFIX}/thisREST.csh 

\"
alias ${mac} \\\"restManager ${m}\\\"
\"
)
        "
)

endforeach(mac ${rest_macros})

