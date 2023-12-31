#!/bin/bash
# --http-address=0.0.0.0 --http-port=8080 --deploy-path=/hello --docroot=.
# --http-address=0.0.0.0 --http-port=80 --docroot=.

remote=$(git config --get remote.origin.url)
echo "remote repository: $remote"
git clone -b 4.10-release https://github.com/emweb/wt.git ext/wt-4.10.0
sleep 4
pushd ext
pushd wt-4.10.0
mkdir -p build
pushd build

if [[ "$OSTYPE" == "msys" ]]; then

cmake .. --fresh -DCMAKE_INSTALL_PREFIX=C:/wt_install -DBOOST_PREFIX=M:/wt.extensions/build/boost_1_82_0
sleep 6
cmake --build . --parallel 9
cmake --install . --config Debug

elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake .. -DCMAKE_INSTALL_PREFIX=/Users/pvn/wt_install -DBOOST_PREFIX=/Users/pvn/github/wt.extensions/ext/boost_1_82_0 -DINSTALL_EXAMPLES=ON
cmake --build . --parallel 9
make install

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake .. -DCMAKE_INSTALL_PREFIX=/home/pvn/wt_install -DBOOST_PREFIX=/home/pvn/github/wt.extensions/ext/boost_1_82_0 -DINSTALL_EXAMPLES=ON
cmake --build . --parallel 9
make install
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/pvn/github/wt.extensions/ext/boost_1_82_0/stage/lib:/home/pvn/wt_install/lib
/home/pvn/wt_install/lib/Wt/examples/hello/hello.wt --http-address=0.0.0.0 --http-port=8081 --docroot=.

fi

popd 
popd 



