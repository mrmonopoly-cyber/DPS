cmake_minimum_required(VERSION 3.16)
project(ControlUnitLogicOperator VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Gather source files recursively
file(GLOB_RECURSE PROJECT_SOURCES
    "${CMAKE_SOURCE_DIR}/test_utility/*c"
    "${CMAKE_SOURCE_DIR}/main.c"
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

file(GLOB_RECURSE PROJECT_LIBS
    "${CMAKE_SOURCE_DIR}/lib/c_vector/*c"
)

# Add executable
add_executable(main
    ${PROJECT_SOURCES}
)

# Create a library for the `lib` sources
add_library(lib_sources STATIC
    ${PROJECT_LIBS}
)

# Include directories for the library
target_include_directories(lib_sources PRIVATE
    "${CMAKE_SOURCE_DIR}/lib"
)

target_compile_options(lib_sources PRIVATE
    $<$<CONFIG:Debug>:-m32 -O0 -fsanitize=address,undefined -g>
    $<$<CONFIG:Release>:-m32 -O2>
)

# Link the `lib_sources` library to the main executable
target_link_libraries(main PRIVATE lib_sources)

# Set compile options based on build type
target_compile_options(main PRIVATE
    $<$<CONFIG:Debug>:-m32 -DDEBUG -Wall -Wextra -Werror -O0 -fsanitize=address,undefined -g>
    $<$<CONFIG:Release>:-m32 -DDEBUG -Wall -Wextra -Werror -O2>
)

target_link_options(main PRIVATE
    $<$<CONFIG:Debug>:-m32 -Wall -Wextra -Werror -O0 -fsanitize=address,undefined -g>
    $<$<CONFIG:Release>:-m32 -Wall -Wextra -Werror -O2>
)

# Optional: Create custom targets for convenience
add_custom_target(build_debug
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug -Bdebug -S${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build debug
)

add_custom_target(build_release
    COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release -Brelease -S${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build release
)
