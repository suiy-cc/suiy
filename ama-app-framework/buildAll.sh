#!/bin/bash

if [ ! -d "./build" ]; then
  mkdir ./build
fi

cd ./build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make -j4
cd ../
