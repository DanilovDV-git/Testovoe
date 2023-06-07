#!/usr/bin/bash

cd ./libpqxx
./configure --disable-shared
make
sudo make install

cd ../
cmake ./
make
