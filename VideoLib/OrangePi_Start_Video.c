#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
	char buffer[200];

	memset(buffer,0,200);

	sprintf(buffer,"mjpg_streamer -i \"/var/www/cgi-bin/mjpg-streamer/input_orangepi.so\" -o \"/var/www/cgi-bin/mjpg-streamer/output_http.so -w /var/www/cgi-bin/mjpg-streamer/www\"");
	
	system(buffer);

	return 0;
}
