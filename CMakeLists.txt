cmake_minimum_required(VERSION 3.20)

project(onion_timer LANGUAGES CXX)

# ---- Library ----
add_library(onion_timer
    "onion/Timer.cpp"
)
add_library(onion::timer ALIAS onion_timer)

target_include_directories(onion_timer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(onion_timer PUBLIC cxx_std_20)

set_target_properties(onion_timer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# ---- Demo ----
option(ONION_BUILD_DEMO "Build Timer demo" OFF)

if (ONION_BUILD_DEMO)
    add_subdirectory(demo)
endif()
