set(contents analysis general util)

if (NOT DEFINED REST_RAW_LIB OR NOT ${REST_RAW_LIB} MATCHES "ON")
	set(excludes TRestSignalToRawSignalProcess)
endif (NOT DEFINED REST_RAW_LIB OR NOT ${REST_RAW_LIB} MATCHES "ON")

COMPILEDIR(RestProcesses)
