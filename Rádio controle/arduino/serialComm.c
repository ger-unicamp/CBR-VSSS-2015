#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>

int main(int argc, char ** argv) {

struct termios options;

int fd = open(                               
         // the name of the serial port                     
         // as a c-string (char *)
         // eg. /dev/ttys0
         "/dev/ttyACM0",
         // configuration options
         // O_RDWR - we need read
         //     and write access
         // O_CTTY - prevent other
         //     input (like keyboard)      
         //     from affecting what we read
         // O_NDELAY - We don't care if     
         //     the other side is           
         //     connected (some devices
         //     don't explicitly connect)     
         O_RDWR | O_NOCTTY | O_NDELAY         
    );
if(fd == -1) {
    // error code goes here
}

tcgetattr(fd, &options);

cfsetispeed(&options, B9600);
cfsetospeed(&options, B9600);

// PARENB is enable parity bit
// so this disables the parity bit
options.c_cflag &= ~PARENB;

// CSTOPB means 2 stop bits
// otherwise (in this case)
// only one stop bit
options.c_cflag &= ~CSTOPB;

// CSIZE is a mask for all the
// data size bits, so anding
// with the negation clears out
// the current data size setting
options.c_cflag &= ~CSIZE;

// Set the timeouts
// VMIN is the minimum amount
// of characters to read.
options.c_cc[VMIN] = 0;

// The amount of time to wait
// for the amount of data
// specified by VMIN in tenths
// of a second.
options.c_cc[VTIME] = 1;

// CLOCAL means don't allow
// control of the port to be changed
// CREAD says to enable the receiver
options.c_cflag |= (CLOCAL | CREAD);

// apply the settings to the serial port
// TCSNOW means apply the changes now
// other valid options include:
//    TCSADRAIN - wait until every
//        thing has been transmitted
//    TCSAFLUSH - flush buffers
//        and apply changes
if(tcsetattr(fd, TCSANOW, &options)!= 0) {

   // error code goes here

}

// CS8 means 8-bits per work
options.c_cflag |= CS8;

char buf[100];
sprintf(buf, "S1#1#200#1#200E\n");
printf("%s", buf);

/**********************************

      Writing to the Serial Port

***********************************/
// write data to the serial port
// fd is the file descriptor of
//      the serial port
// buf is a pointer to the data that
//      we want to write to the serial
//      port
// bufSize is the amount of data that we
//      want to write
int wordsWritten = write(fd, buf, 16);

int wordsRead = read(fd, buf, 100);
printf("%s", buf);

// close the serial port
if(close(fd) == -1) {
   // error code goes here
}

}
