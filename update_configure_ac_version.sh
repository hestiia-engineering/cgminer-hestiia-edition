#!/bin/bash

PROJECT_ROOT=.

if [ -f $PROJECT_ROOT/version.txt ]; then
    echo "version.txt found"
else
    echo "looking in parent folder"
    PROJECT_ROOT=..
    if [ -f $PROJECT_ROOT/version.txt ]; then
        echo "version.txt found"
    else
        echo "version.txt not found"
        pwd
        exit 1
    fi
fi
version=$(cat $PROJECT_ROOT/version.txt)
maj=$(echo $version | cut -d. -f1)
min=$(echo $version | cut -d. -f2)
mic=$(echo $version | cut -d. -f3)

# Update configure.ac
sed -i "s/m4_define(\[v_maj\], \[[0-9]*\])/m4_define([v_maj], [$maj])/g" ${PROJECT_ROOT}/configure.ac
sed -i "s/m4_define(\[v_min\], \[[0-9]*\])/m4_define([v_min], [$min])/g" ${PROJECT_ROOT}/configure.ac
sed -i "s/m4_define(\[v_mic\], \[[0-9]*\])/m4_define([v_mic], [$mic])/g" ${PROJECT_ROOT}/configure.ac

echo "configure.ac updated with version $version"
