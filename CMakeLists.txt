cmake_minimum_required (VERSION 3.20)
project(Eugene)

# c++20を使うのでセット
set(CMAKE_CXX_STANDARD 23)

option(USE_IMGUI "Imguiを使用するか?" OFF)

# visual stdio用にunicodeを使うように
add_definitions(-D_UNICODE -DUNICODE)

set(LIBRARY_OUTPUT_PATH ./Game/Engine)



add_library(Eugene SHARED 
	Source/Eugene.cpp
	Source/ThreadPool.cpp
	Source/Engine.cpp
	Source/ResourceManager.cpp
	Source/GameObject/GameObject.cpp
	Source/GameObject/Scene.cpp
	Source/GameObject/Component/Component.cpp
	)
	
target_include_directories(Eugene PUBLIC "EugeneLib/Include")
target_link_libraries(Eugene ../EugeneLib/Debug/EugeneLib)

