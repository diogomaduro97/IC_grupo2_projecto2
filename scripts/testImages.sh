#!/bin/bash

cd ../

make

#images="ls ./Images/*.jpg"
cd Images
images=$(ls *.jpg)
cd ..
for imageIn in $images
do
    imageOut="./Images_Out/$imageIn"
    echo "./Images/"$imageIn
    echo $imageOut 
    ./opencvTest "./Images/"$imageIn $imageOut
done

cd Images
images=$(ls *.png)
cd ..
for imageIn in $images
do
    imageOut="./Images_Out/$imageIn"
    echo "./Images/"$imageIn
    echo $imageOut 
    ./opencvTest "./Images/"$imageIn $imageOut
done

cd scripts