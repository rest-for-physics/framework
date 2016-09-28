set(target_libraries RestViewers RestTools RestEvents RestMetadata RestProcesses RestExternal RestCore ${target_libraries} -lGui ${ROOT_LIBRARIES} -lEve -lGeom)

add_executable(restManager restManager.cxx)
install(TARGETS restManager RUNTIME DESTINATION bin)
target_link_libraries(restManager ${target_libraries})

add_executable(restPlots restPlots.cxx)
install(TARGETS restPlots RUNTIME DESTINATION bin)
target_link_libraries(restPlots ${target_libraries})

add_executable(restIntegrate restIntegrate.cxx)
install(TARGETS restIntegrate RUNTIME DESTINATION bin)
target_link_libraries(restIntegrate ${target_libraries})

add_executable(restIntegrateSmearing restIntegrateSmearing.cxx)
install(TARGETS restIntegrateSmearing RUNTIME DESTINATION bin)
target_link_libraries(restIntegrateSmearing ${target_libraries})
