#include <stdio.h>
#include <unistd.h>             /* Used for UART */
#include <fcntl.h>              /* Used for UART */
#include <termios.h>            /* Used for UART */
#include <pthread.h>            /* Posix threads */

/* Delay between each communication group (ms) */
#define DELAY 1000

static int ft12_toggle = 0;
static pthread_t thread;        /* BAOS listening thread */


void send_data(int f, unsigned char *buffer, int length)
{
    int i;

    if(f != -1)
    {
        // Filestream, bytes to write, number of bytes to write
        int count = write(f, buffer, length);

        if(count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            // Bytes sent
            printf("<-- %2i bytes sent: ", length);

            for(i = 0; i < length; i++)
            {
                printf("%02x ", buffer[i]);
            }

            printf("\n");
        }

    }

}

void *receive_data(void *data)
{
    int *f = ((int *)data);
    unsigned char buffer[256];
    int i;
    int length = 0;

    while(*f != -1)
    {

        // Filestream, buffer to store in, number of bytes to read (max)
        length = read(*f, (void *)buffer, 255);

        if(length < 0)
        {
            // An error occured (occurs if there are no bytes)
            usleep(10);
        }
        else if(length == 0)
        {
            // No data waiting
            usleep(10);
        }
        else
        {
            // Bytes received
            printf("--> %2i bytes received: ", length);

            for(i = 0; i < length; i++)
            {
                printf("%02x ", buffer[i]);
            }

            printf("\n");

            if(buffer[0] == 0x68)
            {
                buffer[0] = 0xe5;
                send_data(*f, buffer, 1);
                printf("\n");
            }

        }

    }

    printf("Thread ended.\n");
    return NULL;
}


void ft12_reset(int f)
{
    unsigned char buffer[4];
    unsigned char *p;

    p = &buffer[0];
    *p++ = 0x10;
    *p++ = 0x40;
    *p++ = 0x40;
    *p++ = 0x16;

    send_data(f, buffer, p - &buffer[0]);
    ft12_toggle = 1;
}


void ft12_send(int f, unsigned char *buffer, int length)
{
    unsigned char tx_buffer[5];
    unsigned char *p;
    unsigned char sum;
    int i;

    p = &tx_buffer[0];
    *p++ = 0x68;
    *p++ = length + 1;
    *p++ = length + 1;
    *p++ = 0x68;
    sum = *p++ = ft12_toggle ? 0x73 : 0x53;
    ft12_toggle = 1 - ft12_toggle;

    for(i = 0; i < length; i++)
    {
        sum += buffer[i];
    }

    send_data(f, tx_buffer, p - &tx_buffer[0]);
    send_data(f, buffer, length);

    p = &tx_buffer[0];
    *p++ = sum;
    *p++ = 0x16;

    send_data(f, tx_buffer, p - &tx_buffer[0]);
}


int main(void)
{
    int uart0_f = -1;
    struct termios options;
    unsigned char tx_buffer[20];
    unsigned char *p;

    // Open in non blocking read/write mode
    uart0_f = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);

    if(uart0_f == -1)
    {
        // ERROR - CAN'T OPEN SERIAL PORT
        printf("Error - Unable to open UART.\n");
        return 1;
    }

    tcgetattr(uart0_f, &options);
    options.c_cflag = B19200 | CS8 | CLOCAL | CREAD | PARENB; // Set baud rate
    options.c_iflag = 0;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_f, TCIFLUSH);
    tcsetattr(uart0_f, TCSANOW, &options);

    if(pthread_create(&thread, NULL, receive_data, &uart0_f) != 0)
    {
        printf("Error - Unable to create thread.\n");
        return 1;
    }


    /*
     * Reset FT 1.2
     */

    printf("\nReset Ft 1.2\n");
    ft12_reset(uart0_f);
    usleep(DELAY*1000);


    /*
     * Switch to Link Layer mode
     */

    printf("\nSwitch to link layer mode\n");
    p = &tx_buffer[0];
    *p++ = 0xf6;                /* M-PropWrite.req */
    *p++ = 0x00;                /* InterfaceObjectType (hi) */
    *p++ = 0x08;                /* InterfaceObjectType (lo) */
    *p++ = 0x01;                /* ObjectInstance */
    *p++ = 0x34;                /* PropertyId */
    *p++ = 0x10;                /* NrOfElem: 0x10 = 1 */
    *p++ = 0x01;                /* StartIndex: 1 */
    *p++ = 0x00;                /* Data */

    ft12_send(uart0_f, tx_buffer, p - &tx_buffer[0]);
    usleep(DELAY*1000);


    /*
     * Send a telegram
     */

    printf("\nSend a telegram to the link layer\n");
    p = &tx_buffer[0];
    *p++ = 0x11;                /* Message code */
    *p++ = 0x00;                /* Additional info */
    *p++ = 0x9c;                /* Control field 1 */
    *p++ = 0xe0;                /* Control field 2 */
    *p++ = 0x21;                /* Source address (hi) */
    *p++ = 0x20;                /* Source address (lo) */
    *p++ = 0x7b;                /* Destination address (hi) */
    *p++ = 0x01;                /* Destination address (lo) */
    *p++ = 0x02;                /* L */
    *p++ = 0x00;                /* TPDU */
    *p++ = 0x80;                /* APDU + Length */
    *p++ = 0x01;                /* Data */

    ft12_send(uart0_f, tx_buffer, p - &tx_buffer[0]);
    usleep(DELAY*1000);


    /*
     * Back to BAOS mode
     */

    printf("\nSwitch back to BAOS mode\n");
    p = &tx_buffer[0];
    *p++ = 0xf6;                /* M-PropWrite.req */
    *p++ = 0x00;                /* InterfaceObjectType (hi) */
    *p++ = 0x08;                /* InterfaceObjectType (lo) */
    *p++ = 0x01;                /* ObjectInstance */
    *p++ = 0x34;                /* PropertyId */
    *p++ = 0x10;                /* NrOfElem: 0x10 = 1 */
    *p++ = 0x01;                /* StartIndex: 1 */
    *p++ = 0xf0;                /* Data */

    ft12_send(uart0_f, tx_buffer, p - &tx_buffer[0]);
    usleep(DELAY*1000);


    /*
     * Get Server item: serial number
     */

    printf("\nGet serial number (in BAOS mode)\n");
    p = &tx_buffer[0];
    *p++ = 0xf0;                /* Main service code */
    *p++ = 0x01;                /* Subservice code */
    *p++ = 0x00;                /* Start item (hi) */
    *p++ = 0x08;                /* Start item (lo) */
    *p++ = 0x00;                /* Number of items (hi) */
    *p++ = 0x01;                /* Number of items (lo) */

    ft12_send(uart0_f, tx_buffer, p - &tx_buffer[0]);
    usleep(DELAY*1000);


    usleep(1000*1000);
    printf("RETURN to end: ");
    fflush(stdout);
    getchar();
    close(uart0_f);
    uart0_f = -1;
    usleep(100);

    return 0;
}
