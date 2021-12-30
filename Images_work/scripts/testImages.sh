#!/bin/bash
rm -rf ../GolombCodeFiles ../Images_Out ../Histograms 
mkdir  ../GolombCodeFiles ../Images_Out ../Histograms 
cd ../build


make
start_time=$SECONDS

cd ../
cd Images
images=$(ls *.ppm)
cd ../build
for imageIn in $images
do
    for j in {0,2}
    do
        for i in {0..2}
        do
            imageOut="../Images_Out/${imageIn%".ppm"}_${i}_${j}.ppm"
            imageCoded="../GolombCodeFiles/${imageIn%".ppm"}_${i}_${j}.txt"
            # echo "../Images/"$imageIn
            # echo $imageOut 
            # echo $imageCoded
            ./opencvTest "../Images/"$imageIn $imageOut $imageCoded $i $j
        done
    done
done
cd ../
cd Images
images=$(ls *.jpg)
cd ../build
for imageIn in $images
do
    for j in {0,2}
    do
        for i in {0..2}
        do
        imageOut="../Images_Out/${imageIn%".jpg"}_${i}_${j}.jpg"
        imageCoded="../GolombCodeFiles/${imageIn%".jpg"}_${i}_${j}.txt"
        # echo "../Images/"$imageIn
        # echo $imageOut 
        # echo $imageCoded
        ./opencvTest "../Images/"$imageIn $imageOut $imageCoded $i $j
        done
    done
done
cd ..
cd Images
images=$(ls *.png)
cd ../build
for imageIn in $images
do
    for j in {0,2}
    do
        for i in {0..2}
        do
        imageOut="../Images_Out/${imageIn%".png"}_${i}_${j}.png"
        imageCoded="../GolombCodeFiles/${imageIn%".png"}_${i}_${j}.txt"
        # echo "../Images/"$imageIn
        # echo $imageOut 
        # echo $imageCoded
        ./opencvTest "../Images/"$imageIn $imageOut $imageCoded $i $j
        done
    done
done


# echo "Images:"
# cd Images_Out
# imagesOut=$(ls *.jpg)
# cd ..
# for image in $images
# do
#     filesize=$(stat -c %s $image)
#     echo "${image}    ${filesize} bytes"
# done

# cd Images_Out
# imagesOut=$(ls *.png)
# cd ..
# for image in $images
# do
#     filesize=”$(stat -c %s $image)
#     echo "${image}    ${filesize} bytes"
# done
cd ..
echo "Original files:"

imagesCoded=$(ls ./Images/*.jpg)


for coded in $imagesCoded
do
    filesize=$(stat -c %s $coded)
    echo "${coded}    ${filesize} bytes"
done

imagesCoded=$(ls ./Images/*.png)

for coded in $imagesCoded
do
    filesize=$(stat -c %s $coded)
    echo "${coded}    ${filesize} bytes"
done
imagesCoded=$(ls ./Images/*.ppm)

for coded in $imagesCoded
do
    filesize=$(stat -c %s $coded)
    echo "${coded}    ${filesize} bytes"
done
echo "Coded files:"
imagesCoded=$(ls ./GolombCodeFiles/*.txt)

for coded in $imagesCoded
do
    filesize=$(stat -c %s $coded)
    echo "${coded}    ${filesize} bytes"
done
cd scripts
elapsed=$(( SECONDS - start_time ))
echo $elapsed" seconds"