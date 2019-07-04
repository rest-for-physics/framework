set(GIT_VALID FALSE)
set(GIT_TAG)
set(GIT_DATE)
set(GIT_COMMIT " N/A ")
set(GIT_BRANCH)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
  return()
endif()

execute_process(COMMAND git describe --tags 
WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} 
OUTPUT_VARIABLE gitdiscribe ERROR_VARIABLE err)

if(err MATCHES ".*fatal.*")
  message("Warning! not a git repository")
  return()
endif()

set(GIT_VALID TRUE)

#get git tag
string(REGEX REPLACE "v(.*)-(.*)-(.*)"
       "v\\1" version
       ${gitdiscribe})
string(REPLACE "\n" "" version ${version})
set(GIT_TAG ${version})

#get git commit date
execute_process(COMMAND git log -1 --format=%ai
WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} 
OUTPUT_VARIABLE git_date)
string(REPLACE "\n" "" git_date ${git_date})
set(GIT_DATE ${git_date})

#get git number	
execute_process(COMMAND git rev-parse --verify HEAD 
WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} 
OUTPUT_VARIABLE git_ver)
string(SUBSTRING ${git_ver} 0 8 git_commit )
set(GIT_COMMIT ${git_commit})

#get git branch
execute_process(COMMAND git branch
WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
OUTPUT_FILE /tmp/temp.txt)
execute_process(COMMAND grep -e "^*"
WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
INPUT_FILE /tmp/temp.txt 
OUTPUT_VARIABLE git_branch)
execute_process(COMMAND rm /tmp/temp.txt)
string(REPLACE "\n" "" git_branch ${git_branch})
string(SUBSTRING ${git_branch} 2 -1 git_branch)
set(GIT_BRANCH ${git_branch})

#get build time
string(TIMESTAMP date "%Y-%m-%d %H:%M")
set(DATE ${date})

message(STATUS "REST release : ${GIT_TAG}")
message(STATUS "REST date    : ${GIT_DATE}")
message(STATUS "REST commit  : ${GIT_COMMIT}")
message(STATUS "REST branch  : ${GIT_BRANCH}")
message(STATUS "Build time   : ${date}")
message("")