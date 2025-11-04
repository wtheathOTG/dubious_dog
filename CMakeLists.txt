cmake_minimum_required(VERSION 4.0)
project(dubious_dog C)

set(CMAKE_C_STANDARD 11)

set(SDL2_ROOT "C:/Libraries/SDL2/SDL2-devel-2.32.8-mingw/SDL2-2.32.8/x86_64-w64-mingw32")

add_executable(dubious_dog main.c)

# Headers & libs
target_include_directories(dubious_dog PRIVATE "${SDL2_ROOT}/include/SDL2")
target_link_directories(dubious_dog PRIVATE "${SDL2_ROOT}/lib")

# Link order matters on MinGW: mingw32, SDL2main, then SDL2
target_link_libraries(dubious_dog PRIVATE mingw32 SDL2main SDL2)

# Copy the runtime DLL next to the exe so Run/Debug works in CLion
add_custom_command(TARGET dubious_dog POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_ROOT}/bin/SDL2.dll"
        $<TARGET_FILE_DIR:dubious_dog>)