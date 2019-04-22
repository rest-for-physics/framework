#set(contents)
#set(libname RestTasks)

#file(GLOB_RECURSE addon_CINT "*.hh")

#COMPILEDIR(${libname})


set(rest_macros)



set(rest_macros ${rest_macros} "restFit")
set(rest_macros ${rest_macros} "restCreateHisto")
set(rest_macros ${rest_macros} "restIntegrate")
set(rest_macros ${rest_macros} "restIntegrateSmeearing")
set(rest_macros ${rest_macros} "restViewEvents")
set(rest_macros ${rest_macros} "restViewG4Event")
#set(rest_macros ${rest_macros} "restViewHitsEvent")
set(rest_macros ${rest_macros} "restViewReadout")
#set(rest_macros ${rest_macros} "restViewGasCurve")
set(rest_macros ${rest_macros} "restViewGeometry")
set(rest_macros ${rest_macros} "restMergeFiles")
set(rest_macros ${rest_macros} "restPrintMetadata")
set(rest_macros ${rest_macros} "restPrintFileContents")
set(rest_macros ${rest_macros} "restPrintDataBase")



set(rest_macros ${rest_macros} PARENT_SCOPE)

#REST macros string(separate with blank)
set(rest_macros_str)
foreach(mac ${rest_macros})
set(rest_macros_str "${rest_macros_str}${mac} ")
endforeach(mac ${rest_macros})
set(rest_macros_str ${rest_macros_str} PARENT_SCOPE)
