set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG HINTS ${LLVM_DIR})
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIR})
add_definitions(${LLVM_DEFINITIONS})

add_executable(laser laser.cpp)

llvm_map_components_to_libnames(LLVM_LIBS core ipo native)

target_link_libraries(laser ${LLVM_LIBS})
