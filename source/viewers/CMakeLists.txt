add_subdirectory(general)
set(VIEWERS_GENERAL_INC ${VIEWERS_GENERAL_INC} PARENT_SCOPE)

add_subdirectory(geant4)
set(VIEWERS_GEANT4_INC ${VIEWERS_GEANT4_INC} PARENT_SCOPE)

add_library(RestViewers ${ViewersGeneral} ${ViewersGeant4} )
