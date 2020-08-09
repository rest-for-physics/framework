if (${REST_EVE} MATCHES "ON")
	set(contents general analysis geant4 addons viewers)
else()
	set(contents general analysis geant4 addons)
endif (${REST_EVE} MATCHES "ON")

COMPILEDIR(RestEvents)