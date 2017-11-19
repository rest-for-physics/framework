set(contents)
set(libname RestTasks)

file(GLOB_RECURSE addon_CINT "*.hh")

COMPILEDIR(${libname})


set(rest_macros)
foreach(mac ${addon_CINT})
	string(REGEX MATCH "[^/\\]*hh" filename ${mac})
	string(REPLACE ".hh" "" filename_noext ${filename})
	string(REPLACE "REST_" "" macname ${filename_noext})

set(rest_macros ${rest_macros} "rest${macname} ")
endforeach(mac ${addon_CINT})
set(rest_macros ${rest_macros} PARENT_SCOPE)