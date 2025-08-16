#!/bin/bash

path_wt="$(pwd)/ext/wt-4.12.0"
path_wt_build="$(pwd)/build/wt-4.12.0"
path_boost="$(pwd)/build/boost_1_88_0"
echo "Wt at: $path_wt"
echo "Wt build at: $path_wt_build"
echo "Boost at: $path_boost"

mkdir -p build
pushd build

if [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake .. -DWT_INCLUDE=/home/pvn/wt_install/include -DWT_CONFIG_H=/home/pvn/wt_install/include

elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake .. -DWT_INCLUDE=/Users/pvn/wt_install/include -DWT_CONFIG_H=/Users/pvn/wt_install/include

elif [[ "$OSTYPE" == "msys" ]]; then

cmake .. --fresh \
    -DWT_INCLUDE="$path_wt/src" \
    -DWT_CONFIG_H="$path_wt_build" \
    -DBOOST_INCLUDE_DIR="$path_boost/include/boost-1_88" \
    -DBOOST_LIB_DIRS="$path_boost/lib"

echo "Press any key to continue..."
read -n 1 -s

cmake --build .  --config Debug --parallel 


fi
cmake --build . --parallel 9
popd
pwd

if [[ "$OSTYPE" != "msys" ]]; then
pushd build
export LD_LIBRARY_PATH=/home/pvn/git/wt.extensions/ext/boost_1_88_0/stage/lib:$LD_LIBRARY_PATH
echo "open browser http://localhost:8081"
./test_extensions --http-address=0.0.0.0 --http-port=8081  --docroot=. -t 2 -d dc_311-2016.csv.s0311.csv -g ward-2012.geojson
popd
fi

exit