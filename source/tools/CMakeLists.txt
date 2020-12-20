set(contents external/tinyxml resttools)

file(GLOB_RECURSE addon_src
	"tiny*cpp"
    "startup.cpp")

COMPILEDIR(RestTools)
