cmake_minimum_required(VERSION 3.15)

project(gamelib)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(PNG REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(Python 3.10 REQUIRED COMPONENTS Interpreter Development)
execute_process(
    COMMAND "${Python_EXECUTABLE}" -c "import sysconfig; print(sysconfig.get_paths()['purelib'])"
    OUTPUT_VARIABLE Python_SITEARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_subdirectory(pybind11)

file(GLOB SOURCE CONFIGURE_DEPENDS src/*.cpp)

pybind11_add_module(gamelib ${SOURCE})

target_include_directories(gamelib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(gamelib PUBLIC ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw ${PNG_LIBRARIES} ${YAML_CPP_LIBRARIES})

install(TARGETS gamelib COMPONENT python LIBRARY DESTINATION "${Python_SITEARCH}")

install(SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/generate_stubs.cmake")

# Install the generated stub after stubgen runs
install(
        DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/stubs/monkey2"
        DESTINATION "${Python_SITEARCH}"
)
