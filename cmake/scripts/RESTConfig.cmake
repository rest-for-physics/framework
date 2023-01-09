# This cmake file will enable to find REST from another CMake project via
# "find_package(REST)" It will define useful variables such as "REST_PATH" or
# "REST_LIBRARIES" which makes linking easier

execute_process(COMMAND rest-config --prefix OUTPUT_VARIABLE REST_PATH)

set(REST_PATH_ENV $ENV{REST_PATH})
string(STRIP "${REST_PATH_ENV}" REST_PATH_ENV)
string(STRIP "${REST_PATH}" REST_PATH)

if(NOT "${REST_PATH_ENV}" STREQUAL "${REST_PATH}")
    message(
        SEND_ERROR
            "REST installation found at ${REST_PATH} but 'REST_PATH' env variable points to $ENV{REST_PATH}"
    )
    set(REST_FOUND False)
endif()

execute_process(COMMAND rest-config --libs OUTPUT_VARIABLE REST_LIBRARIES)
string(STRIP ${REST_LIBRARIES} REST_LIBRARIES) # It is necessary to strip the
                                               # whitespaces, or it will give
                                               # error

execute_process(COMMAND rest-config --incdir OUTPUT_VARIABLE REST_INCLUDE_DIRS)
string(STRIP ${REST_INCLUDE_DIRS} REST_INCLUDE_DIRS) # It is necessary to strip
                                                     # the whitespaces, or it
                                                     # will give error

execute_process(COMMAND rest-config --version OUTPUT_VARIABLE REST_VERSION)
string(STRIP ${REST_VERSION} REST_VERSION) # It is necessary to strip the
                                           # whitespaces, or it will give error

message(STATUS "REST installation found at: ${REST_PATH}")
