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


set(rest_macros_str)
foreach(mac ${rest_macros})
set(rest_macros_str "${rest_macros_str}${mac} ")
endforeach(mac ${rest_macros})
set(rest_macros_str ${rest_macros_str} PARENT_SCOPE)
