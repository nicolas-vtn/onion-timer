add_executable(onion_timer_demo
    "timer_demo.cpp"
)

target_link_libraries(onion_timer_demo
    PRIVATE
        onion::timer
)

target_compile_features(onion_timer_demo PRIVATE cxx_std_20)

set_target_properties(onion_timer_demo PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
