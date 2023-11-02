/**
 * @file test.c
 * @author xuwithbean
 * @brief This is a test for linux communicate with stm32 via ch341
 * @version 1.0
 * @date 2023-10-18
 * @copyright Copyright (c) 2023
 * 
 */

/**
 * @brief includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <linux/byteorder/little_endian.h>
#include "ch341_lib.h"

/**
 * @brief defines
 */
#define LENTH 1

/**
 * @brief some temp prama
 */
uint8_t CH341Buffer[LENTH]={0x11};

/**
 * @brief the right imformation of ch34x_pis
 */
struct ch34x {
	int fd;
	char version[100];
	CHIP_TYPE chiptype;
	uint32_t dev_id;
};
static struct ch34x ch341device;

/**
 * @brief show the information of connected ch341 chip
 * 
 * @param pathname 
 * @return true 
 * @return false 
 */
bool Show_DevMsg(char *pathname)
{
	unsigned char buf[256];
	int ret;
	int i;
	uint16_t vendor, product;
	CHIP_TYPE chiptype;
	unsigned char chipver;

	/* Get Driver Version */
	ret = CH34x_GetDriverVersion(ch341device.fd, ch341device.version);
	if (ret == false) {
		printf("CH34x_GetDriverVersion error.\n");
		goto exit;
	}
	printf("Driver version: %s\n", ch341device.version);

	/* Get Chip Type */
	ret = CH34x_GetChipType(ch341device.fd, &chiptype);
	if (ret == false) {
		printf("CH34x_GetChipType error.\n");
		goto exit;
	}

	/* Get Chip Version */
	ret = CH34x_GetChipVersion(ch341device.fd, &chipver);
	if (ret == false) {
		printf("CH34x_GetChipVersion error.\n");
		goto exit;
	}
	printf("Chip version: 0x%2x\n", chipver);

	/* Get Device ID */
	ret = CH34X_GetDeviceID(ch341device.fd, &ch341device.dev_id);
	if (ret == false) {
		printf("CH34X_GetDeviceID error.\n");
		goto exit;
	}
	vendor = ch341device.dev_id;
	product = ch341device.dev_id >> 16;
	printf("Vendor ID: 0x%4x, Product ID: 0x%4x\n", vendor, product);
	printf("Device operating has function [SPI+I2C+PARALLEL+GPIO].\n");

	return true;

exit:
	return false;
}

/**
 * @brief Initlize and Enable the SPI communication
 * 
 * @return true 
 * @return false 
 */
bool CH341_SPI_Init()
{
	bool ret;
	uint8_t iMode;

	/* set spi interface in [MSB] */
	iMode = 0x80;

	/* init spi interface */
	ret = CH34xSetStream(ch341device.fd, iMode);
	if (!ret) {
		printf("Failed to init SPI interface.\n");
		return false;
	}

	return true;
}

int main(int argc,char *argv[])
{
    bool ret;
	char choice, ch;
    if (argc != 2)
    {
		printf("Usage: sudo %s [device]\n", argv[0]);
		return -1;
	}
    ch341device.fd = CH34xOpenDevice(argv[1]);
	if (ch341device.fd < 0)
    {
		printf("CH34xOpenDevice false.\n");
		return -1;
	}
	printf("Open device %s succeed, fd: %d\n", argv[1], ch341device.fd);
    ret = Show_DevMsg(argv[1]);
	if (ret == false)
		return -1;
	sleep(1);
	ret = CH34xSetTimeout(ch341device.fd, 2000, 2000);
	if (ret == false) {
		printf("CH34xSetTimeout false.\n");
		return -1;
	}
    while (1) 
    {
		printf("\nthere are the commands\n"
		       "press p to quit\n"
		       "press t to start test\n");

		scanf("%c", &choice);
		while ((ch = getchar()) != EOF && ch != '\n') ;
        if(choice == 'p')
        {
            CH34xCloseDevice(ch341device.fd);
            printf("Close device succeed.\n");
            break;
        }
        switch (choice)
        {
            case 't':
                printf("Test begin.\n");
                if(CH34xStreamSPI4(ch341device.fd,0x80,LENTH,&CH341Buffer)==false)
                {
                    printf("error");
                }
                printf("%d",CH341Buffer[0]);
                break;
            default:
                printf("Bad choice, please input again.\n");
                break;
        }
	}
}
