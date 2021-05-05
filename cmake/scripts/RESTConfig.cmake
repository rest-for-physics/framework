
# This cmake file will enable to find REST from another CMake project via "find_package(REST)"
# It will define useful variables such as "REST_PATH" or "REST_LIBRARIES" which makes linking easier

set(REST_PATH $ENV{REST_PATH})

# TODO: I think there is a better way to do this, we shouldn't call 'rest-config' for this
execute_process(COMMAND rest-config --libs OUTPUT_VARIABLE REST_LIBRARIES)
string(STRIP ${REST_LIBRARIES} REST_LIBRARIES) # It is necessary to strip the whitespaces, or it will give error

execute_process(COMMAND rest-config --version OUTPUT_VARIABLE REST_VERSION)
string(STRIP ${REST_VERSION} REST_VERSION)
