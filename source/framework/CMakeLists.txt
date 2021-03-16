set(contents external/tinyxml external/expressionEval tools core analysis)

file(GLOB_RECURSE addon_src
	"tiny*cpp"
    "startup.cpp")

if (NOT ${REST_EVE} MATCHES "ON")
	set( excludes TRestEveEventViewer)
endif (NOT ${REST_EVE} MATCHES "ON")

COMPILEDIR(RestFramework)
