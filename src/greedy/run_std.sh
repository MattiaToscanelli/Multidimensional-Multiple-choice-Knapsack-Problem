#!/bin/bash

# List all groups folders
for i in $(ls -d MMKP_GR*);
do
  echo "Running for group " ${i};
  cd ${i};

  # Create out_standard folder if it does not exist
  mkdir -p out_standard;

  for full_name in $(ls ../Instances/standard/mmkp_*.txt);
  do
    instance=$(basename ${full_name})
    # Copy instance file
    cp ${full_name} ${instance}

    # Run algorithm with timeout and store user time
    /usr/bin/time -o ${instance}.time ./mmkp -i ${instance} -t 60

    # Backup instance, output and time files
    mv ${instance} out_standard;
    mv ${instance}.out out_standard;
    mv ${instance}.time out_standard;
  done
  cd ..;
done