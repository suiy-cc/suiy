#!/bin/bash

if [ ! -d "./build" ]; then
  mkdir ./build
fi

cd ./build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DIS_GENERATE_CPP_SOURCE_FILE_BY_FIDL=OFF -DWITH_IAS_BACKEND=ON -DWITH_ROOTFS_AIO=ON $1
make -j4
cd ../
