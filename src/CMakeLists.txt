rock_library(sonar_tritech
  SOURCES Micron.cpp Profiling.cpp SeaNet.cpp 
  HEADERS Micron.hpp Profiling.hpp SeaNet.hpp SeaNetTypes.hpp
  DEPS base-types iodrivers-base )

rock_executable(sonar_tritech_bin Main.cpp
    DEPS sonar_tritech)

