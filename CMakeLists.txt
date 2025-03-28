cmake_minimum_required(VERSION 3.14)
project(MyProject CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Указываем пути
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/inc)
set(BUILD_DIR ${CMAKE_BINARY_DIR}/build)
set(BIN_DIR ${BUILD_DIR}/bin)
set(OBJ_DIR ${BUILD_DIR}/obj)

# Файлы исходного кода
file(GLOB SRCS "${SRC_DIR}/*.cpp")

# Создаем исполняемый файл
add_executable(main ${SRCS})

# Устанавливаем директорию для бинарников
set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# Указываем пути к заголовочным файлам
include_directories(${INC_DIR})

# Опции компиляции
target_compile_options(main PRIVATE -g)

# Команда для запуска
add_custom_target(run COMMAND ${BIN_DIR}/main DEPENDS main)

# Команда для отладки
add_custom_target(debug COMMAND gdb ${BIN_DIR}/main DEPENDS main)
