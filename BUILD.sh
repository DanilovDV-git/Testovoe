#!/usr/bin/bash

# Building project\
cd ./server
chmod +x ./build.sh
./build.sh
chmod +x ./server/launch.sh
cd ../

cd ./client
chmod +x ./build.sh
./build.sh
chmod +x ./launch.sh
cd ../


cd ./auto_client
chmod +x ./build.sh
./build.sh
chmod +x ./launch.sh
cd ../

chmod +x ./LAUNCH.sh
