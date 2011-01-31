cmake_minimum_required(VERSION 2.6)

INCLUDE(FindPkgConfig)

pkg_check_modules(IO REQUIRED "iodrivers_base")
INCLUDE_DIRECTORIES(${IO_INCLUDE_DIRS})
LINK_DIRECTORIES(${IO_LIBRARY_DIRS})

find_package(Boost COMPONENTS thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

ADD_DEFINITIONS(-O0 -Wall)

add_library(micron SHARED
	SeaNet.cpp
	Micron.cpp
	Profiling.cpp
	SonarScan.cpp
	MicronConfiguration.cpp
)

add_executable(micronTest
	SeaNet.cpp
	Micron.cpp
	SonarScan.cpp
	Profiling.cpp
	MicronConfiguration.cpp
	main.cpp
)

add_executable(profilingTest
	SeaNet.cpp
	Micron.cpp
	SonarScan.cpp
	Profiling.cpp
	MicronConfiguration.cpp
	main-profiling.cpp
)

TARGET_LINK_LIBRARIES(micron
	${IO_LIBRARIES}
	${Boost_LIBRARIES}
)

TARGET_LINK_LIBRARIES(micronTest
	${IO_LIBRARIES}
	${Boost_LIBRARIES}
)

TARGET_LINK_LIBRARIES(profilingTest
	${IO_LIBRARIES}
	${Boost_LIBRARIES}
)


INSTALL(TARGETS micron 
	LIBRARY DESTINATION lib
)

INSTALL(FILES SonarScan.h MicronConfiguration.h SeaNet.h Micron.h Profiling.h DESTINATION include)

CONFIGURE_FILE(micron.pc.in micron.pc @ONLY)

INSTALL(FILES 
	${CMAKE_BINARY_DIR}/micron.pc 
	DESTINATION lib/pkgconfig
)
