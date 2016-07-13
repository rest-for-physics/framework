include_directories(restcore/inc)

include_directories(external/tsp/inc)

include_directories(events/geant4/inc)
include_directories(events/analysis/inc)
include_directories(events/general/inc)

include_directories(metadata/daq/inc)
include_directories(metadata/general/inc)
include_directories(metadata/geant4/inc)
include_directories(metadata/readout/inc)

include_directories(tools/inc)

include_directories(processes/general/inc)
include_directories(processes/analysis/inc)
include_directories(processes/raw/inc)
include_directories(processes/signal/inc)

include_directories(viewers/general/inc)
include_directories(viewers/geant4/inc)


add_subdirectory(restcore)
add_subdirectory(external)
add_subdirectory(events)
add_subdirectory(metadata)
add_subdirectory(tools)
add_subdirectory(processes)
add_subdirectory(viewers)
