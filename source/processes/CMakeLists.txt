set(contents analysis general raw signal)
set(libname RestProcesses)

if(${REST_SE} MATCHES "ON")
COMPILEDIR_SE(${libname})
else()
COMPILEDIR(${libname})
endif()
