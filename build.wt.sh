#!/bin/bash
# --http-address=0.0.0.0 --http-port=8080 --deploy-path=/hello --docroot=.
# --http-address=0.0.0.0 --http-port=80 --docroot=.

remote=$(git config --get remote.origin.url)
echo "remote repository: $remote"
if [ ! -d "ext/wt-4.12.0" ]; then
    git -c advice.detachedHead=false clone -b 4.12-release https://github.com/emweb/wt.git ext/wt-4.12.0
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

cmake ../../ext/wt-4.12.0 --fresh -DCMAKE_INSTALL_PREFIX=C:/wt_install -DBOOST_PREFIX="$path_boost" -DINSTALL_EXAMPLES=OFF \
-DENABLE_QT5=OFF -DENABLE_QT6=OFF 
cmake --build .  --config Debug --parallel 
cmake --install . --config Debug

elif [[ "$OSTYPE" == "darwin"* ]]; then

path_boost="$(pwd)/../../ext/boost_1_88_0"
echo "At: $(pwd)"
echo "Boost at: $path_boost"

cmake ../../ext/wt-4.12.0 \
 -DCMAKE_INSTALL_PREFIX="$(pwd)/../../install/wt" \
 -DBOOST_PREFIX="$path_boost" \
 -DINSTALL_EXAMPLES=ON \
 -DCMAKE_CXX_FLAGS="-Wno-deprecated -Wno-deprecated-declarations -Wno-deprecated-copy" \
 -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release --parallel 
cmake --install . --config Release

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then

path_boost="$(pwd)/../../ext/boost_1_88_0"
echo "At: $(pwd)"
echo "Boost at: $path_boost"

cmake ../../ext/wt-4.12.0 \
 -DCMAKE_INSTALL_PREFIX="$(pwd)/../../install/wt" \
 -DBOOST_PREFIX="$path_boost" \
 -DINSTALL_EXAMPLES=ON \
 -DCMAKE_BUILD_TYPE=Release

cmake --build . --config Release 
cmake --install . --config Release

fi

popd 
popd 



