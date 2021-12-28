#!/bin/bash

cd ../

make

cd Images
images=$(ls *.jpg)
cd ..
for imageIn in $images
do
    for i in {0..2}
    do
        imageOut="./Images_Out/$imageIn"
        imageCoded="${imageOut}Coded${i}"
        echo "./Images/"$imageIn
        echo $imageOut 
        echo $imageCoded
        ./opencvTest "./Images/"$imageIn $imageOut $imageCoded $i
    done
done

cd Images
images=$(ls *.png)
cd ..
for imageIn in $images
do
    for i in {0..2}
    do
        imageOut="./Images_Out/$imageIn"
        imageCoded="${imageOut}Coded${i}"
        echo "./Images/"$imageIn
        echo $imageOut
        echo $imageCoded
        ./opencvTest "./Images/"$imageIn $imageOut $imageCoded $i
    done
done


cd Images_Out
imagesOut=$(ls *.jpg)
cd ..
for image in $images
do
    filesize=$(stat -c %s $image)
    echo "${image}    ${filesize} bytes"
done

cd Images_Out
imagesOut=$(ls *.png)
cd ..
for image in $images
do
    filesize=”$(stat -c %s $image)
    echo "${image}    ${filesize} bytes"
done

cd scripts