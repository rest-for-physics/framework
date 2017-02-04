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

add_executable(restCreateHisto restCreateHisto.cxx)
install(TARGETS restCreateHisto RUNTIME DESTINATION bin)
target_link_libraries(restCreateHisto ${target_libraries})

add_executable(restPrintAnaTreeVariables restPrintAnaTreeVariables.cxx)
install(TARGETS restPrintAnaTreeVariables RUNTIME DESTINATION bin)
target_link_libraries(restPrintAnaTreeVariables ${target_libraries})

add_executable(restFit restFit.cxx)
install(TARGETS restFit RUNTIME DESTINATION bin)
target_link_libraries(restFit ${target_libraries})

add_executable(restPrintRunInfo restPrintRunInfo.cxx)
install(TARGETS restPrintRunInfo RUNTIME DESTINATION bin)
target_link_libraries(restPrintRunInfo ${target_libraries})

add_executable(restPrintAllMetadata restPrintAllMetadata.cxx)
install(TARGETS restPrintAllMetadata RUNTIME DESTINATION bin)
target_link_libraries(restPrintAllMetadata ${target_libraries})

add_executable(restPrintFileContents restPrintFileContents.cxx)
install(TARGETS restPrintFileContents RUNTIME DESTINATION bin)
target_link_libraries(restPrintFileContents ${target_libraries})
