
# Regular CMake project setup for non-ESP32
add_executable(${projectName} main.cpp)

# Add required libraries and include directories
target_include_directories(${projectName} PRIVATE "." "include" $ENV{CADMIUM})

# Non-ESP32 specific compile options
target_compile_options(${projectName} PUBLIC -std=gnu++2b)