cmake_minimum_required(VERSION 2.6)

INCLUDE(FindPkgConfig)

pkg_check_modules(IO REQUIRED "iodrivers_base")
INCLUDE_DIRECTORIES(${IO_INCLUDE_DIRS})
LINK_DIRECTORIES(${IO_LIBRARY_DIRS})

find_package(Boost COMPONENTS thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_library(micron SHARED
	SonarInterface.cpp
	SonarScan.cpp
	MicronConfiguration.cpp
)

add_executable(micronTest
	SonarInterface.cpp
	SonarScan.cpp
	MicronConfiguration.cpp
	main.cpp
)

TARGET_LINK_LIBRARIES(micron
	${IO_LIBRARIES}
	${Boost_LIBRARIES}
)

TARGET_LINK_LIBRARIES(micronTest
	${IO_LIBRARIES}
	${Boost_LIBRARIES}
)


INSTALL(TARGETS micron 
	LIBRARY DESTINATION lib
)

INSTALL(FILES SonarScan.h MicronConfiguration.h SonarInterface.h DESTINATION include)

CONFIGURE_FILE(micron.pc.in micron.pc @ONLY)

INSTALL(FILES 
	${CMAKE_BINARY_DIR}/micron.pc 
	DESTINATION lib/pkgconfig
)
