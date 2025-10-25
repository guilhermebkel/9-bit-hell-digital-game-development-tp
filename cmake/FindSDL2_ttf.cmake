# FindSDL2_ttf.cmake
# Workaround for systems (like Ubuntu) without SDL2_ttfConfig.cmake

find_path(SDL2_TTF_INCLUDE_DIR
        SDL_ttf.h
        HINTS
        $ENV{SDL2TTFDIR}
        /usr/include/SDL2
        /usr/local/include/SDL2
        /usr/include
        /usr/local/include
)

find_library(SDL2_TTF_LIBRARY
        NAMES SDL2_ttf
        HINTS
        $ENV{SDL2TTFDIR}
        /usr/lib
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib/x86_64-linux-gnu
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_ttf
        REQUIRED_VARS SDL2_TTF_LIBRARY SDL2_TTF_INCLUDE_DIR)

if(SDL2_TTF_FOUND)
    set(SDL2_TTF_LIBRARIES ${SDL2_TTF_LIBRARY})
    set(SDL2_TTF_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIR})
endif()

mark_as_advanced(SDL2_TTF_INCLUDE_DIR SDL2_TTF_LIBRARY)
