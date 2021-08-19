cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17) # Установка требуемого стандарта C++

# Установка имён выходных программ
project(syncpub)
project(syncsub)

# Добавление выполняемых исходных кодов
add_executable(syncpub syncpub.cpp)
add_executable(syncsub syncsub.cpp)

# Поиск cppzmq обертки - библиотеки zmq для последующей линковки с программами
find_package(cppzmq)
target_link_libraries(syncpub cppzmq)
target_link_libraries(syncsub cppzmq)
