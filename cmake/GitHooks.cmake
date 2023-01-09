find_program(clang-format QUIET)

if(clang-format_NOTFOUND)
    message(
        WARNING "clang-format not found, skipping formatting in pre-commit hook"
    )
    return()
endif()

# Installing clang-format pre-commit hook if prerequisites are met and it
# doesn't exist yet.

if(NOT EXISTS ${PROJECT_SOURCE_DIR}/.git)
    message(
        WARNING
            ".git directory not found at the root of the repository (${PROJECT_SOURCE_DIR}), skipping formatting in pre-commit hook. Perhaps you are compiling in a remote host?"
    )
    return()
endif()

if(NOT EXISTS ${PROJECT_SOURCE_DIR}/.git/hooks/pre-commit)
    # We cannot write the file from here because we need exec permissions
    configure_file(${CMAKE_SOURCE_DIR}/cmake/utils/git_pre-commit.in
                   ${PROJECT_SOURCE_DIR}/.git/hooks/pre-commit)
else()
    message(DEBUG
            "git hook found at ${PROJECT_SOURCE_DIR}/.git/hooks/pre-commit")
endif()

find_package(Git)

if(Git_FOUND)
    message(
        STATUS
            "${GIT_EXECUTABLE} submodule foreach git rev-parse --git-path hooks"
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule --quiet foreach
                "git rev-parse --git-path hooks"
        OUTPUT_VARIABLE SUBMODULE_OUTPUT)
    string(REPLACE "\n" ";" "SUBMODULE_OUTPUT" "${SUBMODULE_OUTPUT}")
    set(SUBMODULE_HOOK ${SUBMODULE_OUTPUT})
    foreach(HOOK_DIR ${SUBMODULE_HOOK})
        if(NOT EXISTS ${HOOK_DIR}/pre-commit)
            configure_file(${CMAKE_SOURCE_DIR}/cmake/utils/git_pre-commit.in
                           ${HOOK_DIR}/pre-commit)
            message(
                STATUS
                    "Installing ${CMAKE_SOURCE_DIR}/cmake/utils/git_pre-commit.in in ${HOOK_DIR}/pre-commit"
            )
        endif()
    endforeach()
else()
    message(WARNING "Git not found")
endif()
