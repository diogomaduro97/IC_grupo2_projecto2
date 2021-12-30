#!/bin/bash
rm -rf ../build
mkdir ../build
cd ../build
cmake ..
make

rm -rf ../GolombCodeFiles ../Sounds_Out ../Histograms
mkdir ../GolombCodeFiles ../Sounds_Out ../Histograms

start_time=$SECONDS

cd ../Sounds
sounds=$(ls *.wav)
cd ../build
echo "--------------------"
for sound in $sounds
do
    soundIn="../Sounds/$sound"
    losslessSoundOut="../Sounds_Out/${sound%".wav"}_lossless.wav"
    losslessOut="../GolombCodeFiles/${sound%".wav"}_lossless.txt"
    lossySoundOut="../Sounds_Out/${sound%".wav"}_lossy.wav"
    lossyOut="../GolombCodeFiles/${sound%".wav"}_lossy.txt"
    echo $sound
    # echo $soundIn
    # echo $losslessSoundOut
    # echo $losslessOut
    # echo $lossySoundOut
    # echo $lossyOut
    ./audioTest $soundIn $losslessSoundOut $losslessOut $lossySoundOut $lossyOut
    echo "--------------------"
done


cd ..
echo "Original files:"

for sound in $sounds
do
    filesize=$(stat -c %s ./Sounds/$sound)
    echo "${sound}    ${filesize} bytes"
done


echo "Coded files:"
soundsCoded=$(ls ./GolombCodeFiles/*.txt)

for coded in $soundsCoded
do
    filesize=$(stat -c %s $coded)
    echo "${coded}    ${filesize} bytes"
done


cd scripts
elapsed=$(( SECONDS - start_time ))
echo $elapsed" seconds"