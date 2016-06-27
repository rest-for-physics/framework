include_directories(inc)


file(GLOB RestCoreSrc
        "src/*.cxx"
        )

add_library(RestCore ${RestCoreSrc})