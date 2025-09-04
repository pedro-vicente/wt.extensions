#!/bin/bash

path_wt="$(pwd)/ext/wt-4.12.0"
path_wt_build="$(pwd)/build/wt-4.12.0"
echo "Wt at: $path_wt"
echo "Wt build at: $path_wt_build"

mkdir -p build
pushd build

if [[ "$OSTYPE" == "linux-gnu"* ]]; then

path_boost="$(pwd)/../ext/boost_1_88_0"
echo "At: $(pwd)"
echo "Boost at: $path_boost"

cmake .. --fresh \
    -DWT_INCLUDE="$path_wt/src" \
    -DWT_CONFIG_H="$path_wt_build" \
    -DBOOST_INCLUDE_DIR="$path_boost" \
    -DBOOST_INCLUDE_ASIO_DIR="$path_boost/libs/asio/include/" \
    -DBOOST_LIB_DIRS="$path_boost/stage/lib/"
cmake --build . 

elif [[ "$OSTYPE" == "darwin"* ]]; then

path_boost="$(pwd)/../ext/boost_1_88_0"
echo "At: $(pwd)"
echo "Boost at: $path_boost"

cmake .. --fresh \
    -DWT_INCLUDE="$path_wt/src" \
    -DWT_CONFIG_H="$path_wt_build" \
    -DBOOST_INCLUDE_DIR="$path_boost" \
    -DBOOST_INCLUDE_ASIO_DIR="$path_boost/libs/asio/include/" \
    -DBOOST_LIB_DIRS="$path_boost/stage/lib/"
cmake --build . 

elif [[ "$OSTYPE" == "msys" ]]; then

path_boost="$(pwd)/boost_1_88_0"
echo "Boost at: $path_boost"

mkdir -p wt.extensions
pushd wt.extensions

cmake ../.. --fresh \
    -DWT_INCLUDE="$path_wt/src" \
    -DWT_CONFIG_H="$path_wt_build" \
    -DBOOST_INCLUDE_DIR="$path_boost/include/boost-1_88" \
    -DBOOST_LIB_DIRS="$path_boost/lib"
cmake --build .  --config Debug --parallel 

popd

fi

popd


if [[ "$OSTYPE" != "msys" ]]; then
pushd build
export LD_LIBRARY_PATH=$path_boost/stage/lib:$LD_LIBRARY_PATH
echo "open browser http://localhost:8081"
./test_extensions --http-address=0.0.0.0 --http-port=8081  --docroot=. -t 2 -d dc_311-2016.csv.s0311.csv -g ward-2012.geojson
popd
fi
