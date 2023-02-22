cmake_minimum_required (VERSION 3.20)
project(Eugene)

# c++20を使うのでセット
set(CMAKE_CXX_STANDARD 23)

add_subdirectory("EugeneLib")

option(USE_IMGUI "Imguiを使用するか?" OFF)

# visual stdio用にunicodeを使うように
add_definitions(-D_UNICODE -DUNICODE)

set(LIBRARY_OUTPUT_PATH ../)

add_library(Eugene SHARED 


	)
	
target_include_directories(Eugene PUBLIC "EugeneLib/Include")

