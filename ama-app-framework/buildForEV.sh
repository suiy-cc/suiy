#!/bin/bash

if [ ! -d "./build" ]; then
  mkdir ./build
fi

cd ./build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DIS_GENERATE_CPP_SOURCE_FILE_BY_FIDL=OFF -DWITH_MODULE_EV=ON -DWITH_IAS_BACKEND=ON
make -j4
cd ../
