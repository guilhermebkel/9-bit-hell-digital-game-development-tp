# FindSDL2_mixer.cmake
# Workaround para sistemas (como Ubuntu) sem SDL2_mixerConfig.cmake

find_path(SDL2_MIXER_INCLUDE_DIR
        SDL_mixer.h
        HINTS
        $ENV{SDL2MIXERDIR}
        /usr/include/SDL2
        /usr/local/include/SDL2
        /usr/include
        /usr/local/include
)

find_library(SDL2_MIXER_LIBRARY
        NAMES SDL2_mixer
        HINTS
        $ENV{SDL2MIXERDIR}
        /usr/lib
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib/x86_64-linux-gnu
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_mixer
        REQUIRED_VARS SDL2_MIXER_LIBRARY SDL2_MIXER_INCLUDE_DIR)

if(SDL2_MIXER_FOUND)
    set(SDL2_MIXER_LIBRARIES ${SDL2_MIXER_LIBRARY})
    set(SDL2_MIXER_INCLUDE_DIRS ${SDL2_MIXER_INCLUDE_DIR})
endif()

mark_as_advanced(SDL2_MIXER_INCLUDE_DIR SDL2_MIXER_LIBRARY)
