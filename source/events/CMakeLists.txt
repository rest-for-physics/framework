set(contents general analysis geant4 addons viewers)
set(libname RestEvents)


#if(${REST_SE} MATCHES "ON")
#COMPILEDIR_SE(${libname})
#else()
COMPILEDIR(${libname})
#endif()
