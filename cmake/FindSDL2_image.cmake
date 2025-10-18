# This workaround is necessary because of Ubuntu 20.04

find_path(SDL2_IMAGE_INCLUDE_DIR
        SDL_image.h
        HINTS
        $ENV{SDL2IMAGEDIR}
        /usr/include/SDL2
        /usr/local/include/SDL2
        /usr/include
        /usr/local/include
)

find_library(SDL2_IMAGE_LIBRARY
        NAMES SDL2_image
        HINTS
        $ENV{SDL2IMAGEDIR}
        /usr/lib
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
        /usr/local/lib/x86_64-linux-gnu
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_image REQUIRED_VARS SDL2_IMAGE_LIBRARY SDL2_IMAGE_INCLUDE_DIR)

if(SDL2_IMAGE_FOUND)
    set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARY})
    set(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIR})
endif()

mark_as_advanced(SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)
