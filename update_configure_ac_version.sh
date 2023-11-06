#!/bin/bash

# Read version
version=$(cat ../version.txt)
maj=$(echo $version | cut -d. -f1)
min=$(echo $version | cut -d. -f2)
mic=$(echo $version | cut -d. -f3)

# Update configure.ac
sed -i "s/m4_define(\[v_maj\], \[[0-9]*\])/m4_define([v_maj], [$maj])/g" ../configure.ac
sed -i "s/m4_define(\[v_min\], \[[0-9]*\])/m4_define([v_min], [$min])/g" ../configure.ac
sed -i "s/m4_define(\[v_mic\], \[[0-9]*\])/m4_define([v_mic], [$mic])/g" ../configure.ac

echo "configure.ac updated with version $version"
