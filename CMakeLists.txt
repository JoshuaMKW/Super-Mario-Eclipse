cmake_minimum_required(VERSION 3.8)

project(SuperMarioEclipse VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)

file(GLOB SUPER_MARIO_ECLIPSE_SRC
    "src/*.c"
    "src/*.cpp"
    "include/*.h"
    "include/*.hxx"
)

add_executable(SuperMarioEclipse ${SUPER_MARIO_ECLIPSE_SRC} "src/rocketdive.cpp")
add_subdirectory(lib/BetterSunshineEngine)

target_link_libraries(SuperMarioEclipse PUBLIC BetterSunshineEngine)

target_include_directories(SuperMarioEclipse PUBLIC "include")

target_compile_options(SuperMarioEclipse PUBLIC ${SMS_COMPILE_FLAGS})
target_link_options(SuperMarioEclipse PUBLIC ${SMS_LINK_FLAGS})

list(APPEND SUPER_MARIO_ECLIPSE_CONFIG_DEFINES "KURIBO_NO_TYPES" "SUPER_MARIO_ECLIPSE_VERSION=\"v1.0\"")

target_compile_definitions(SuperMarioEclipse PUBLIC ${SUPER_MARIO_ECLIPSE_CONFIG_DEFINES})

add_custom_target(${PROJECT_NAME}.kxe ALL DEPENDS ${PROJECT_NAME})
add_custom_command(
    TARGET ${PROJECT_NAME}.kxe
    COMMAND ${PROJECT_SOURCE_DIR}/lib/BetterSunshineEngine/tools/KuriboConverter.exe ARGS ${PROJECT_NAME} ${PROJECT_NAME}.kxe ${PROJECT_SOURCE_DIR}/lib/BetterSunshineEngine/maps/${SMS_REGION}.map
)