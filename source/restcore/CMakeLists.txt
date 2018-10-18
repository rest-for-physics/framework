set(contents)
set(libname RestCore)


#if(${REST_SE} MATCHES "ON")
#COMPILEDIR_SE(${libname})
#else()
COMPILEDIR(${libname})
#endif()
