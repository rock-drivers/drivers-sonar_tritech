
qt4_automoc(
	SonarInterface.cpp
	SonarScan.cpp
	MicronConfiguration.cpp
)

QT4_WRAP_CPP(micron_MOC_SRCS 
	SonarInterface.h
	SonarScan.h
	MicronConfiguration.h
)


add_library(micron SHARED
	SonarInterface.cpp
	SonarScan.cpp
	MicronConfiguration.cpp
	${micron_MOC_SRCS}
)

INSTALL(TARGETS micron 
	LIBRARY DESTINATION lib
)

INSTALL(FILES SonarScan.h MicronConfiguration.h DESTINATION include)

CONFIGURE_FILE(micron.pc.in micron.pc @ONLY)

INSTALL(FILES 
	${CMAKE_BINARY_DIR}/micron.pc 
	DESTINATION lib/pkgconfig
)
