set(contents)

if (NOT DEFINED RESTLIB_RAW OR NOT ${RESTLIB_RAW} MATCHES "ON")
	set(excludes TRestDetectorSignalToRawSignalProcess)
endif (NOT DEFINED RESTLIB_RAW OR NOT ${RESTLIB_RAW} MATCHES "ON")

COMPILEDIR(RestDetector)
