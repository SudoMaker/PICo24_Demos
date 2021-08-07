/*
    This file is part of PICo24 SDK.

    Copyright (C) 2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <PICo24/PICo24.h>

uint8_t MediaInitialize(void *userp, uint8_t lun) {

	return true;
}

uint8_t MediaDetect(void *userp, uint8_t lun) {
	return true;
}

uint8_t WriteProtectState(void *userp, uint8_t lun) {
	return true;
}

uint32_t ReadCapacity(void *userp, uint8_t lun) {
	return 1048576;
}

uint16_t ReadSectorSize(void *userp, uint8_t lun) {
	return 512;
}

uint8_t SectorRead(void *userp, uint8_t lun, uint32_t sector_addr, uint8_t* buffer) {

	return true;
}

uint8_t SectorWrite(void *userp, uint8_t lun, uint32_t sector_addr, uint8_t* buffer, uint8_t allowWriteToZero) {

	return true;
}

USBDeluxeDevice_MSD_DiskOps disk_ops = {
	.MediaInitialize = MediaInitialize,
	.MediaDetect = MediaDetect,
	.WriteProtectState = WriteProtectState,
	.ReadCapacity = ReadCapacity,
	.ReadSectorSize = ReadSectorSize,
	.SectorRead = SectorRead,
	.SectorWrite = SectorWrite
};

int main() {
	// Use default PinMux configuration
	PICo24_PinConfig_UseDefault();

	// ... and initialize clocks
	PICo24_Clock_Initialize();

	// RF1 LED On
	LATFbits.LATF1 = 1;

	// Initialize UART
	UART_Initialize(&huart1, UART_CS8, B115200);
	STDOUT_FILENO = UART1_FILENO;
	printf("Hello from %s!\n", "PICo24");

	// Initialize USB device config
	USBDeluxe_Device_ConfigInit(0x04d8, 0xe966, "SudoMaker", "PICo24 16x MSD Test", "123");
	// Add one MSD instance with 16 LUNs
	USBDeluxe_DeviceFunction_Add_MSD(NULL, 16, "PICo24", "Mass Storage", "0001", &disk_ops);

	// Apply USB device config
	USBDeluxe_Device_ConfigApply();
	// Set USB to DEVICE mode
	USBDeluxe_SetRole(USB_ROLE_DEVICE);

	uint16_t cnt;

	while (1) {
		if (cnt == 0) {
			LATFbits.LATF0 = !LATFbits.LATF0;
		}

		USBDeluxe_Tasks();

		cnt++;
	}
}