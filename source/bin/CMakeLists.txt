set(target_libraries RestViewers RestTools RestEvents RestMetadata RestProcesses RestExternal RestCore -lGui ${ROOT_LIBRARIES} -lEve -lGeom Garfield)

add_executable(restManager restManager.cxx)
install(TARGETS restManager RUNTIME DESTINATION bin)
target_link_libraries(restManager ${target_libraries})

add_executable(restPlots restPlots.cxx)
install(TARGETS restPlots RUNTIME DESTINATION bin)
target_link_libraries(restPlots ${target_libraries})