# set(contents) set(libname RestTasks)

# file(GLOB_RECURSE addon_CINT "*.hh")

# COMPILEDIR(${libname})

set(rest_macros)

set(rest_macros ${rest_macros} "restFit")
set(rest_macros ${rest_macros} "restViewEvents")
# set(rest_macros ${rest_macros} "restViewHitsEvent") set(rest_macros
# ${rest_macros} "restViewReadout") set(rest_macros ${rest_macros}
# "restViewGasCurve")
set(rest_macros ${rest_macros} "restViewGeometry")
set(rest_macros ${rest_macros} "restMergeFiles")
set(rest_macros ${rest_macros} "restMakeProcess")
set(rest_macros ${rest_macros} "restPrintMetadata")
set(rest_macros ${rest_macros} "restPrintFileContents")
set(rest_macros ${rest_macros} "restSendMessage")

set(rest_macros
    ${rest_macros}
    PARENT_SCOPE)
