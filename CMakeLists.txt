cmake_minimum_required(VERSION 2.8)

project(opencvhdfs)

find_package( OpenCV REQUIRED )
find_package(HDF5 COMPONENTS CXX)

include_directories(
	${PROJECT_SOURCE_DIR}/include
	${OPENCV_INCLUDE_DIRS}
	${HDF5_INCLUDE_DIRS}
)

file(GLOB opencvhdfs_SRC
    "src/*.cpp"
)

add_library(opencvhdfs_lib SHARED ${opencvhdfs_SRC})
target_link_libraries(opencvhdfs_lib
	${OpenCV_LIBS}
	${HDF5_CXX_LIBRARIES}
	${HDF5_LIBRARIES}
)

option(enable_example "enable_example" OFF)

if(enable_example)
	add_executable(example example.cpp)
	target_link_libraries(example opencvhdfs_lib ${OpenCV_LIBS})
endif()