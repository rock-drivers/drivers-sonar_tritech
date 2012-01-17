rock_library(sonar_tritech
  SOURCES Micron.cpp SeaNet.cpp SeaNetTypesInternal.cpp 
  HEADERS Micron.hpp SeaNet.hpp SeaNetTypes.hpp
  DEPS_PKGCONFIG base-lib iodrivers_base )

rock_executable(sonar_tritech_bin Main.cpp
    DEPS sonar_tritech)

