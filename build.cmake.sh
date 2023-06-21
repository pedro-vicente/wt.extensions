#!/bin/bash

mkdir -p build
pushd build
if [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake .. -DWT_INCLUDE=/home/pvn/wt_install/include -DWT_CONFIG_H=/home/pvn/wt_install/include

elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake .. -DWT_INCLUDE=/Users/pvn/wt_install/include -DWT_CONFIG_H=/Users/pvn/wt_install/include

elif [[ "$OSTYPE" == "msys" ]]; then

cmake .. --fresh -DWT_INCLUDE=M:/wt.extensions/ext/wt-4.10.0/src -DWT_CONFIG_H=M:/wt.extensions/ext/wt-4.10.0/build
cmake --build .

fi
cmake --build . --parallel 9
popd
pwd

if [[ "$OSTYPE" != "msys" ]]; then
pushd build
export LD_LIBRARY_PATH=/home/pvn/github/wt.extensions/ext/boost_1_82_0/stage/lib:$LD_LIBRARY_PATH
./test_extensions --http-address=0.0.0.0 --http-port=8081  --docroot=. -t 2 -d dc_311-2016.csv.s0311.csv -g ward-2012.geojson
popd
fi

exit

