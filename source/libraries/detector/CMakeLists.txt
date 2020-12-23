set(contents)

set(excludes)

# We remove from compilation those processs that connect to other libraries
if (NOT DEFINED RESTLIB_RAW OR NOT ${RESTLIB_RAW} MATCHES "ON")
	set(excludes ${excludes} TRestDetectorSignalToRawSignalProcess )
endif (NOT DEFINED RESTLIB_RAW OR NOT ${RESTLIB_RAW} MATCHES "ON")

if (NOT DEFINED RESTLIB_TRACK OR NOT ${RESTLIB_TRACK} MATCHES "ON")
	set(excludes ${excludes} TRestDetectorHitsToTrackProcess TRestDetectorHitsToTrackFastProcess)
endif (NOT DEFINED RESTLIB_TRACK OR NOT ${RESTLIB_TRACK} MATCHES "ON")

COMPILEDIR(RestDetector)
