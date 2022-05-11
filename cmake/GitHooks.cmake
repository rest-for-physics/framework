
find_program(clang-format QUIET)

if (clang-format_NOTFOUND)
    message(WARNING "clang-format not found, skipping formatting in pre-commit hook")
    return()
endif ()

# Installing clang-format pre-commit hook if prerequisites are met and it doesn't exist yet.

if (NOT EXISTS ${PROJECT_SOURCE_DIR}/.git)
    message(WARNING ".git directory not found at the root of the repository (${PROJECT_SOURCE_DIR}), skipping formatting in pre-commit hook")
    return()
endif ()

if (EXISTS ${PROJECT_SOURCE_DIR}/.git/hooks/pre-commit)
    message(DEBUG "git hook found at ${PROJECT_SOURCE_DIR}/.git/hooks/pre-commit")
    return()
endif ()

# We cannot write the file from here because we need exec permissions
configure_file(${CMAKE_SOURCE_DIR}/cmake/utils/git_pre-commit.in ${PROJECT_SOURCE_DIR}/.git/hooks/pre-commit)

