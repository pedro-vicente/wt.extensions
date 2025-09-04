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

path_boost="$(pwd)/build/boost_1_88_0"
echo "Boost at: $path_boost"

cmake .. --fresh \
    -DWT_INCLUDE="$path_wt/src" \
    -DWT_CONFIG_H="$path_wt_build" \
    -DBOOST_INCLUDE_DIR="$path_boost/include/boost-1_88" \
    -DBOOST_LIB_DIRS="$path_boost/stage/lib/"
cmake --build .  --config Debug --parallel 

fi

popd

