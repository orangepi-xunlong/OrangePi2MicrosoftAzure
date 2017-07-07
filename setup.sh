# global setting
mkdir /etc/OrangePi/
chown -R www-data:www-data *

# Build the OrangePi to Azure library.
cp PortLib/OrangePi /usr/bin/
cp Azure_IOTLib/OrangePi2Azure.h /usr/include/
cd Azure_IOTLib
gcc OrangePi.c -shared -fPIC -o libOrangePi2Azure.so
mv libOrangePi2Azure.so /usr/lib/
chown www-data:www-data /etc/OrangePi2Azure.conf
cd -

# Build the OrangePi to Sound library.
#cd SoundLib
#cp OrangePi2Sound.h /usr/include/
#gcc OrangePiSoundLib.c -lasound -o OrangePi_Sound
#mv OrangePi_Sound /usr/bin/
#gcc OrangePi2Sound.c -shared -fPIC -o libOrangePi2Sound.so
#mv libOrangePi2Sound.so /usr/lib/
#cd -

cd OrangePiSpeech2Text
./setup.sh
cp -rf base /var/www/cgi-bin/
cp -rf src /var/www/cgi-bin/
cp Speech2Text.sh /var/www/cgi-bin/
cd ..

# Build the OrangePi to video library
cd mjpg-streamer
./setup.sh
cd ..
cp -rf mjpg-streamer/ /var/www/cgi-bin/
cd VideoLib/OrangePi_V4L2
make
mv OrangePi_Capture /usr/bin/
chmod 777 /usr/bin/OrangePi_Capture
chown www-data:www-data /usr/bin/OrangePi_Capture
make lib
mv libOrangePi_video.so /usr/lib/
cd ..
gcc OrangePi_Start_Video.c -o Start_Video
chown www-data:www-data Start_Video
chmod 777 Start_Video
cp Start_Video /var/www/cgi-bin/
cd OrangePi_FaceEmotionDetection
cp OrangePi_CognitiveService.py /var/www/html/
cp OrangePi_2_CognitiveService_Emotion.conf /etc/OrangePi/OrangePi_2_CognitiveService_Emotion.conf
chown www-data:www-data /etc/OrangePi/OrangePi_2_CognitiveService_Emotion.conf
cp OrangePi_2_CognitiveService_Face.conf /etc/OrangePi/OrangePi_2_CognitiveService_Face.conf
chown www-data:www-data /etc/OrangePi/OrangePi_2_CognitiveService_Face.conf
cd ..
gcc OrangePi2CognitiveService.c -lOrangePi_video -shared -fPIC -o libOrangePi2CognitiveService.so
mv libOrangePi2CognitiveService.so /usr/lib/
cp OrangePi2CognitiveService.h /usr/include/
cd ..

# Build websit
gcc Websit/cgi-bin/do_one_capture.c -o Websit/cgi-bin/do_one_capture.cgi
gcc Websit/cgi-bin/update.c -o Websit/cgi-bin/update.cgi
cp -rf Websit/html/* /var/www/html/
mkdir /var/www/cgi-bin
cp -rf Websit/cgi-bin/* /var/www/cgi-bin/
cp -rf Websit/apache2/* /etc/apache2/

# build Sample code
# Build the OrangePi to Azure API
gcc Sample_OrangePi2Azure.c -lOrangePi2Azure -o ToAzure

# Build the OrangePi to get sound of API
gcc Sample_OrangePi2Sound.c -lOrangePi2Sound -o ToSound

# Build the OrangePi to get video of API
gcc Sample_OrangePi2video.c -lOrangePi2CognitiveService -o ToVideo
cp ToVideo /var/www/cgi-bin/
chown -R www-data:www-data /var/www/
