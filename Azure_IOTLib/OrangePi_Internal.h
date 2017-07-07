#ifndef _ORANGEPI_INTERNAL_H_
#define _ORANGEPI_INTERNAL_H_
#include <string.h>

/*
 * Get length of int value.
 */
int get_length(int data)
{
    int i = 0;
    
    if(data == 0)
	return i;    

    while(data) {
	i++;
	data /= 10;
    }
    return i;
}
/*
 * int conver to char *
 */
inline void int2string(int data,char *buffer)
{
    int val;
    int length = get_length(data);
    int i,j = 1;

    for(i = 0 ; i < length ; i++) {
	buffer[length - j] = (data % 10) + '0';
	data /= 10;
	j++;	
    }

    buffer[length] = '\0';
}

/*
 *
 */
inline int fact(int num)
{
    int i;
    int total = 1;

    for(i = 0 ; i < num ; i++)
	total = total * 10;
    return total;
}

/*
 * char * conver to int
 */
inline void string2int(char *buffer,int *data)
{
    int len = strlen(buffer);
    int i;

    *data = 0;
    for(i = 0 ; i < len ; i++)  
	*data += (buffer[i] - '0') * fact(len - i - 1);
}
#endif
