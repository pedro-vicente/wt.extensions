cmake_minimum_required(VERSION 3.20)
set(CMAKE_BUILD_TYPE Debug)
project (wt.extensions)

message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "WT_INCLUDE: " ${WT_INCLUDE})
message(STATUS "WT_CONFIG_H: " ${WT_CONFIG_H})
message(STATUS "BOOST_INCLUDE_DIR: " ${BOOST_INCLUDE_DIR})
message(STATUS "BOOST_LIB_DIRS: " ${BOOST_LIB_DIRS})
message(STATUS "BOOST_INCLUDE_ASIO_DIR: " ${BOOST_INCLUDE_ASIO_DIR})

set (CMAKE_CXX_STANDARD 14)

include_directories(${BOOST_INCLUDE_DIR})
include_directories(${BOOST_INCLUDE_ASIO_DIR})
include_directories(${WT_INCLUDE})
include_directories(${WT_CONFIG_H})
include_directories(${CMAKE_SOURCE_DIR}/gason/src)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/ext/wt-4.12.0/src)

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

if (MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT test_extensions)
  set_property(TARGET test_extensions PROPERTY 
    VS_DEBUGGER_COMMAND_ARGUMENTS "--http-address=0.0.0.0 --http-port=8080  --docroot=. -t 2 -d dc_311-2016.csv.s0311.csv -g ward-2012.geojson")
  set_property(TARGET test_extensions PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()


#//////////////////////////
# link with libraries
# lib_dep contains a cascade definition of all the libraries needed to link
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
endif()

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wno-deprecated-copy -Wno-deprecated-copy-dtor")
endif()

message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")

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
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/build/wt-4.12.0/src/Debug/wtd.lib)
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/build/wt-4.12.0/src/http/Debug/wthttpd.lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_filesystem-vc143-mt-gd-x64-1_88.lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_thread-vc143-mt-gd-x64-1_88.lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_program_options-vc143-mt-gd-x64-1_88.lib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_chrono-vc143-mt-gd-x64-1_88.lib)
endif()

if(APPLE)
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/install/wt/lib/libwt.dylib)
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/install/wt/lib/libwthttp.dylib)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_filesystem.a)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_thread.a)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_program_options.a)
endif()

if (LINUX)
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/install/wt/lib/libwt.so)
  set(lib_dep ${lib_dep} ${CMAKE_CURRENT_SOURCE_DIR}/install/wt/lib/libwthttp.so)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_filesystem.so)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_thread.so)
  set(lib_dep ${lib_dep} ${BOOST_LIB_DIRS}/libboost_program_options.so)
endif()

#//////////////////////////
#  link
#//////////////////////////

message(STATUS "lib_dep: " ${lib_dep})
target_link_libraries (test_chart ${lib_dep})
target_link_libraries (test_extensions ${lib_dep})

#//////////////////////////
# data files 
#//////////////////////////

message(STATUS "Copying Leaflet source to: ${CMAKE_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/src/extensions/leaflet-src.js" DESTINATION ${CMAKE_BINARY_DIR})

message(STATUS "Copying data files to: ${CMAKE_BINARY_DIR}")
set(DATA_FILES
  "data/dc_311-2016.csv.s0311.csv"
  "data/ward-2012.geojson"
  "data/montgomery_county_schools.csv"
  "data/montgomery_county_boundary.json"
  "data/wmata_stations.json"
  "data/md_maryland_zip_codes_geo.min.json"
  "data/TATMS_npp_d20141130_t1817273_e1817589_b16023_c20141201005810987954_noaa_ops.h5.star.json"
  "data/gz_2010_us_040_00_20m.json"
  "data/us.topojson"
  "data/us_presidential_election_results_by_county.csv"
)

message(STATUS "Copying data files to: ${CMAKE_BINARY_DIR}")
foreach(DATA_FILE ${DATA_FILES})
  file(COPY "${CMAKE_SOURCE_DIR}/${DATA_FILE}" DESTINATION ${CMAKE_BINARY_DIR})
endforeach()