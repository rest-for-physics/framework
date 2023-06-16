function (set_library_version TAG_VARIABLE)
    execute_process(
        COMMAND git describe --abbrev=0 --tags
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_TAG
        OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

    if (NOT GIT_TAG)
        message(
            WARNING
                "Failed to retrieve Git tag. Probably the '.git' directory is missing. This may happen in remove development environments. To fix this configure the syncing of the '.git' directory."
        )
    endif ()

    string(REGEX REPLACE "^v(.*)" "\\1" LIB_VERSION ${GIT_TAG})

    set(${TAG_VARIABLE}
        ${LIB_VERSION}
        PARENT_SCOPE)
endfunction ()
