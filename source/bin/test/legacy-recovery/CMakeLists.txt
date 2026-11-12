# The base legacy-signal fixtures are supplied by detectorlib's recovery tests.
# The changed unrelated class lives here so this regression needs no tracklib.
root_generate_dictionary(G__RecoveryOldTrack TrackFixture.h LINKDEF LinkDef.h)
add_executable(
    RecoveryTrackFixtureIO TrackFixtureIO.cxx TrackFixture.cxx
                           ${CMAKE_CURRENT_BINARY_DIR}/G__RecoveryOldTrack.cxx)
target_include_directories(RecoveryTrackFixtureIO
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RecoveryTrackFixtureIO PRIVATE ${ROOT_LIBRARIES})

root_generate_dictionary(G__RecoveryNewTrack TrackFixture.h LINKDEF LinkDef.h
                         OPTIONS -DRECOVERY_NEW_TRACK_SCHEMA)
add_library(
    RecoveryNewTrack SHARED TrackFixture.cxx
                            ${CMAKE_CURRENT_BINARY_DIR}/G__RecoveryNewTrack.cxx)
target_compile_definitions(RecoveryNewTrack PRIVATE RECOVERY_NEW_TRACK_SCHEMA)
target_include_directories(RecoveryNewTrack PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RecoveryNewTrack PRIVATE ${ROOT_LIBRARIES})

add_test(
    NAME LegacySignalRecovery.PreservesBranchWithRemovedBase
    COMMAND
        ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/check_recovery.py
        $<TARGET_FILE:restRoot> $<TARGET_FILE:RestLegacyRecoveryFixtureWriter>
        $<TARGET_FILE:RestLegacyRecoveryFixtureReader>
        $<TARGET_FILE:RecoveryTrackFixtureIO> $<TARGET_FILE:RecoveryNewTrack>
        ${CMAKE_SOURCE_DIR}/macros/legacy/recoverLegacySignalData.C)
set_tests_properties(LegacySignalRecovery.PreservesBranchWithRemovedBase
                     PROPERTIES TIMEOUT 180)
