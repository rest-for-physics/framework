add_subdirectory(general)
set(VIEWERS_GENERAL_INC ${VIEWERS_GENERAL_INC} PARENT_SCOPE)

add_subdirectory(geant4)
set(VIEWERS_GEANT4_INC ${VIEWERS_GEANT4_INC} PARENT_SCOPE)

add_library(RestViewers SHARED ${ViewersGeneral} ${ViewersGeant4} )
#add_library(RestViewersStatic STATIC ${ViewersGeneral} ${ViewersGeant4} )

install(TARGETS RestViewers
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)