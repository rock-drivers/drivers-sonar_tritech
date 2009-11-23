cmake_minimum_required(VERSION 2.6)

find_package(Qt4 COMPONENTS QtCore REQUIRED)

INCLUDE(${QT_USE_FILE})



INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_INCLUDE_DIR}
	${QT_QT_INCLUDE_DIR}
	${QT_QTCORE_INCLUDE_DIR}
)

qt4_automoc(
	SonarInterface.cpp
	SonarScan.cpp
	MicronConfiguration.cpp
)

QT4_WRAP_CPP(micron_MOC_SRCS 
	SonarInterface.h
)


add_library(micron SHARED
	SonarInterface.cpp
	SonarScan.cpp
	MicronConfiguration.cpp
	${micron_MOC_SRCS}
)

target_link_libraries(micron ${QT_QTCORE_LIBRARIES})

INSTALL(TARGETS micron 
	LIBRARY DESTINATION lib
)

INSTALL(FILES SonarScan.h MicronConfiguration.h SonarInterface.h DESTINATION include)

CONFIGURE_FILE(micron.pc.in micron.pc @ONLY)

INSTALL(FILES 
	${CMAKE_BINARY_DIR}/micron.pc 
	DESTINATION lib/pkgconfig
)
