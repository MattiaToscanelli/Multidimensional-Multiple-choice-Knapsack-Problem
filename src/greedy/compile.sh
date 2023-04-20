#!/bin/bash

# List all groups folders
for i in $(ls -d MMKP_GR*);
do
  echo "Compiling source for group " ${i};
  cd ${i};
  g++ -O3 -std=c++17 -o mmkp -I . *.cpp -lm;  cd ..;
done