rock_library(sonar_tritech
  SOURCES SeaNetMicron.cpp SeaNet.cpp SeaNetTypesInternal.cpp 
  HEADERS SeaNetMicron.hpp SeaNet.hpp SeaNetTypes.hpp SeaNetTypesInternal.hpp
  DEPS_PKGCONFIG base-lib iodrivers_base )

rock_executable(sonar_tritech_bin Main.cpp
    DEPS sonar_tritech)

