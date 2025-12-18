cmake_minimum_required(VERSION 3.15)

project(gamelib)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --- Dependencies ---
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(PNG REQUIRED)
find_package(yaml-cpp REQUIRED)

# --- Pick a specific Python variation ---
find_package(Python 3.10 EXACT REQUIRED COMPONENTS Interpreter Development)

# Path where Python modules will be installed
execute_process(
    COMMAND "${Python_EXECUTABLE}" -c "import sysconfig; print(sysconfig.get_paths()['purelib'])"
    OUTPUT_VARIABLE Python_SITEARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Using Python: ${Python_EXECUTABLE}")
message(STATUS "Python site-packages: ${Python_SITEARCH}")

add_subdirectory(external/pybind11)


# ----------------------------------------------------
# 1️⃣ Build the C++ core library (engine only)
# ----------------------------------------------------
file(GLOB ENGINE_SOURCES CONFIGURE_DEPENDS core/src/*.cpp core/src/shapes/*.cpp core/src/skeletal/*.cpp core/src/agi/*.cpp)

add_library(gamelib_core STATIC ${ENGINE_SOURCES})

target_include_directories(gamelib_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/core/include ${CMAKE_CURRENT_SOURCE_DIR}/external/glm)

target_link_libraries(gamelib_core PUBLIC ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw ${PNG_LIBRARIES} ${YAML_CPP_LIBRARIES})

# ----------------------------------------------------
# 2️⃣ Build the Python bindings module
# ----------------------------------------------------
file(GLOB BINDINGS_SOURCES CONFIGURE_DEPENDS bindings/*.cpp)

pybind11_add_module(gamelib ${BINDINGS_SOURCES})

target_link_libraries(gamelib PRIVATE gamelib_core)

# ----------------------------------------------------
# 3️⃣ Stub generation (optional but recommended)
# ----------------------------------------------------
set(STUBS_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/stubs")

add_custom_target(generate_stubs ALL
        COMMAND "${Python_EXECUTABLE}" -m pybind11_stubgen gamelib --output-dir "${STUBS_OUTPUT_DIR}"
        DEPENDS gamelib
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Generating Python stubs for gamelib..."
)


# ----------------------------------------------------
# 4️⃣ Install module + stubs
# ----------------------------------------------------
install(TARGETS gamelib COMPONENT python LIBRARY DESTINATION "${Python_SITEARCH}")


# If stub generator produces a .pyi file:
install(FILES "${STUBS_OUTPUT_DIR}/gamelib.pyi"
        DESTINATION "${Python_SITEARCH}")
