cmake_minimum_required(VERSION 3.15)
project(MyProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Собираем все .cpp файлы в текущей папке
file(GLOB SOURCES "*.cpp")

# Создаём исполняемый файл
add_executable(MyExecutable ${SOURCES})

# Запуск после сборки
add_custom_target(Run
    COMMAND ${CMAKE_BINARY_DIR}/Debug/MyExecutable.exe
    DEPENDS MyExecutable
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

