set(contents daq geant4 general readout)
set(libname RestMetadata)

if(${REST_SE} MATCHES "ON")
COMPILEDIR_SE(${libname})
else()
COMPILEDIR(${libname})
endif()

