#include <stdio.h>
#include <iostream>
#include "arduino-serial/arduino-serial-lib.h"

using namespace std;

int main() {
    /*
    char data[] = {"S3#1#300#1#300E"};
    FILE *file;
    file = fopen("/dev/tty.usbmodem1411", "w"); // Opening usb device file. 
    int i = 0;
    for (i = 0 ; i < 13 ; i++) {
        fprintf(file, "%c", data[i]); // Writing to the file.
        printf("%c\n", (char) data[i]);
    }
    fclose(file);
    */

	const int buf_max = 256;
	char buf[buf_max];

	int fd = serialport_init("/dev/tty.usbmodem1411", 9600);
	cout << "fd: "<< fd << endl;
	if (fd == -1) cout << "couldn't open port" << endl;

	sprintf(buf, "%s", "S3#1#300#1#300E");
	int rc = serialport_write(fd, buf);
	cout << "rc: "<< rc << endl;
	if (rc == -1) cout << "error writing" << endl;
}
