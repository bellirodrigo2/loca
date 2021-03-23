cmake_minimum_required(VERSION 3.10)

# For compiling using C compiler
project(loca_lib LANGUAGES C VERSION 1.0.0)

#Set Standard
set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD 17)

#for GOOGLE TEST
# find_package(GTest REQUIRED)

add_executable(tloca test/tloca.c loca.c lib_maps.c)
# add_executable(gtloca test/gtloca.cpp loca.c)
add_executable(tsstr test/tsstr.c sstring.c)

# run undefined behavior sanitizer
target_link_libraries(tloca -fsanitize=undefined -fno-omit-frame-pointer)
target_link_libraries(tsstr -fsanitize=undefined -fno-omit-frame-pointer)

target_compile_options(tloca PRIVATE -Wall -Wextra -pedantic -Werror -Wno-unused -fsanitize=undefined -fno-omit-frame-pointer)
target_compile_options(tsstr PRIVATE -Wall -Wextra -pedantic -Werror -Wno-unused -fsanitize=undefined -fno-omit-frame-pointer)

#for GOOGLE TEST
# target_link_libraries(gtloca ${GTEST_LIBRARIES} pthread)
