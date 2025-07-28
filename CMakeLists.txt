cmake_minimum_required(VERSION 3.10)
project(ShamirSecretSharing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GMP library
find_library(GMP_LIBRARIES NAMES gmp)
if(NOT GMP_LIBRARIES)
    message(FATAL_ERROR "GMP library not found")
endif()

# Add nlohmann/json as a dependency
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Add executable
add_executable(shamir_secret_sharing
    main.cpp
    ShamirSecretSharing.cpp
)

# Link libraries
target_link_libraries(shamir_secret_sharing PRIVATE
    ${GMP_LIBRARIES}
    nlohmann_json::nlohmann_json
)

# Include directories
target_include_directories(shamir_secret_sharing PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)