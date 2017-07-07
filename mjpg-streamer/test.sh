#make DEBUG=true clean all
export LD_LIBRARY_PATH="$(pwd)"
./mjpg_streamer -i "./input_orangepi.so" -o "./output_http.so -w ./www"
