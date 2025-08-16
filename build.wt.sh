#!/bin/bash
# --http-address=0.0.0.0 --http-port=8080 --deploy-path=/hello --docroot=.
# --http-address=0.0.0.0 --http-port=80 --docroot=.

remote=$(git config --get remote.origin.url)
echo "remote repository: $remote"
if [ ! -d "ext/wt-4.12.0" ]; then
    git clone -b 4.12-release https://github.com/emweb/wt.git ext/wt-4.12.0
    rm -rf build/wt-4.12.0
else
    echo "ext/wt-4.12.0 already exists, skipping clone"
fi

sleep 2
mkdir -p build/wt-4.12.0
pushd build
pushd wt-4.12.0

if [[ "$OSTYPE" == "msys" ]]; then

path_boost="../boost_1_88_0"
echo "At: $(pwd)"
echo "Boost at: $path_boost"
sleep 3

cmake ../../ext/wt-4.12.0 --fresh -DCMAKE_INSTALL_PREFIX=C:/wt_install -DBOOST_PREFIX="$path_boost" -DINSTALL_EXAMPLES=OFF \
-DENABLE_QT5=OFF -DENABLE_QT6=OFF 

pwd
echo "Press any key to continue..."
read -n 1 -s

cmake --build .  --config Debug --parallel 
cmake --install . --config Debug

elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake .. -DCMAKE_INSTALL_PREFIX=/Users/pvn/wt_install -DBOOST_PREFIX=/Users/pvn/git/wt.extensions/ext/boost_1_88_0 -DINSTALL_EXAMPLES=ON
cmake --build . --parallel 9
make install

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake .. -DCMAKE_INSTALL_PREFIX=/home/pvn/wt_install -DBOOST_PREFIX=/home/pvn/git/wt.extensions/ext/boost_1_88_0 -DINSTALL_EXAMPLES=ON
cmake --build . --parallel 9
make install
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/pvn/git/wt.extensions/ext/boost_1_88_0/stage/lib:/home/pvn/wt_install/lib
echo "open browser http://localhost:8081"
/home/pvn/wt_install/lib/Wt/examples/hello/hello.wt --http-address=0.0.0.0 --http-port=8081 --docroot=.
 
fi

popd 
popd 



