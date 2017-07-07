# Start recording
cd base/
rm julius.wav
rm julius.out
arecord -d 2 -f cd -c 1 -r 16000 -t wav julius.wav
./autoSpeech2Text.sh
cd ../src/
./update
