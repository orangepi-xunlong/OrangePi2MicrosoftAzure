#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <OrangePi2Azure.h>

const char *connect = "HostName=OrangePiPlus.azure-devices.net;DeviceId=OrangePiPlus;SharedAccessKey=+342A6mNVuabgJOjUYzizjFe5j5H2+EIGmsm4L4mIB4=";

int main()
{
    OrangePi_Update_Azure_Key(connect);
    OrangePi2Azure("rgbled","255");
    return 0;
}
