if (MSVC)
 cmake_minimum_required(VERSION 3.26)
else()
 cmake_minimum_required(VERSION 3.15)
endif()
set(CMAKE_BUILD_TYPE Debug)
project (wt.extensions)

message(STATUS "WT_INCLUDE: " ${WT_INCLUDE})
message(STATUS "WT_CONFIG_H: " ${WT_CONFIG_H})
message(STATUS "WT_ROOT: " ${WT_ROOT})
message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_CURRENT_BINARY_DIR})

set (CMAKE_CXX_STANDARD 14)
if (MSVC)
  include_directories(${CMAKE_SOURCE_DIR}/build/boost_1_82_0/include/boost-1_82)
else()
  include_directories(${CMAKE_SOURCE_DIR}/ext/boost_1_82_0)
endif()
include_directories(${WT_INCLUDE})
include_directories(${WT_CONFIG_H})
include_directories(${CMAKE_SOURCE_DIR}/gason/src)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/ext/wt-4.10.0/src)

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

#//////////////////////////
# source files
#//////////////////////////

set(src ${src})
set(src ${src} src/WCesium.cc)
set(src ${src} src/WCesium.hh)
set(src ${src} src/WLeaflet.cc)
set(src ${src} src/WLeaflet.hh)
set(src ${src} src/WPlotly.cc)
set(src ${src} src/WPlotly.hh)
set(src ${src} src/csv.cc)
set(src ${src} src/csv.hh)
set(src ${src} src/star_dataset.cc)
set(src ${src} src/star_dataset.hh)
set(src ${src} src/star_json.cc)
set(src ${src} src/topojson.cc)
set(src ${src} src/topojson.hh)
set(src ${src} src/geojson.cc)
set(src ${src} src/geojson.hh)
set(src ${src} ${CMAKE_SOURCE_DIR}/gason/src/gason.cpp)

add_executable(test_chart src/chart/test_chart.cc src/chart/chart_model.cc src/chart/chart_model.hh ${src})
add_executable(test_extensions src/extensions/test_extensions.cc src/extensions/test_geojson.cc src/extensions/test_atms.cc ${src})

#//////////////////////////
#link with libraries
#lib_dep contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(lib_dep ${lib_dep})

#//////////////////////////
# Linux/Mac
#//////////////////////////

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-deprecated-declarations")
  message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")
endif()

#//////////////////////////
# LINUX
# order of the link libraries matters; pthread dl
#//////////////////////////

if(LINUX)
  set(lib_dep ${lib_dep} pthread dl)
  set(lib_dep ${lib_dep} stdc++fs)
  find_program(LSB_RELEASE_EXEC lsb_release)
  execute_process(COMMAND ${LSB_RELEASE_EXEC} -is OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
  message(STATUS "Building in " ${LSB_RELEASE_ID_SHORT})
endif()

if (MSVC)
  set(lib_dep ${lib_dep} M:/wt.extensions/ext/wt-4.10.0/build/src/Debug/wtd.lib)
  set(lib_dep ${lib_dep} M:/wt.extensions/ext/wt-4.10.0/build/src/http/Debug/wthttpd.lib)
  set(BOOST_LIB_DIRS ${CMAKE_SOURCE_DIR}/build/boost_1_82_0/lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_filesystem-vc143-mt-gd-x64-1_82.lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_thread-vc143-mt-gd-x64-1_82.lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_program_options-vc143-mt-gd-x64-1_82.lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_chrono-vc143-mt-gd-x64-1_82.lib)
endif()

if(APPLE)
  set(lib_dep ${lib_dep} /Users/pvn/wt_install/lib/libwt.dylib)
  set(lib_dep ${lib_dep} /Users/pvn/wt_install/lib/libwthttp.dylib)
  set(BOOST_LIB_DIRS /Users/pvn/github/wt.extensions/ext/boost_1_82_0/stage/lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_filesystem.a)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_thread.a)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_program_options.a)
endif()

if (LINUX)
  set(lib_dep ${lib_dep} /home/pvn/wt_install/lib/libwt.so)
  set(lib_dep ${lib_dep} /home/pvn/wt_install/lib/libwthttp.so)
  set(BOOST_LIB_DIRS /home/pvn/github/wt.extensions/ext/boost_1_82_0/stage/lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_filesystem.so)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_thread.so)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_program_options.so)
endif()

message(STATUS "lib_dep: " ${lib_dep})
target_link_libraries (test_chart ${lib_dep})
target_link_libraries (test_extensions ${lib_dep})

message(STATUS "Copying data files to: ${CMAKE_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/src/extensions/leaflet.js" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/src/extensions/data/dc_311-2016.csv.s0311.csv" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/src/extensions/data/ward-2012.geojson" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/src/extensions/data/montgomery_county_schools.csv" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/src/extensions/data/montgomery_county_boundary.json" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/src/extensions/data/wmata_stations.json" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/src/extensions/data/md_maryland_zip_codes_geo.min.json" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/src/extensions/data/TATMS_npp_d20141130_t1817273_e1817589_b16023_c20141201005810987954_noaa_ops.h5.star.json" DESTINATION ${CMAKE_BINARY_DIR})
