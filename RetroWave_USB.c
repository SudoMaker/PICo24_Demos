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

typedef struct {
	uint8_t buf[128];
	uint16_t buf_used;

	uint8_t byte;
	uint8_t shift_count;
} RetroWaveSerialContext;

RetroWaveSerialContext rws_ctx = {0};

void retrowave_bus_tx(const uint8_t *Buf, uint16_t cdc_pkt_len) {
	uint8_t in_cursor = 0;

	while (in_cursor < cdc_pkt_len) {
		uint8_t cur_byte_in = Buf[in_cursor];
		if (cur_byte_in & 0x01) {
			cur_byte_in &= 0xfe;

			rws_ctx.byte |= (cur_byte_in >> (8 - rws_ctx.shift_count));

			if (rws_ctx.shift_count) {
				rws_ctx.buf[rws_ctx.buf_used] = rws_ctx.byte;
				rws_ctx.buf_used += 1;
			}

			rws_ctx.byte = cur_byte_in << rws_ctx.shift_count;
			rws_ctx.shift_count += 1;

			if (rws_ctx.shift_count > 7) {
				rws_ctx.shift_count = 0;
			}
		} else {
			if (rws_ctx.buf_used) {
				SPI_Transmit(&hspi1, rws_ctx.buf, rws_ctx.buf_used);
				rws_ctx.buf_used = 0;
			}

			rws_ctx.shift_count = 0;

			if (cur_byte_in & 0x02) {
				while (hspi1.STAT->SPIBEC != 0);
				LATFbits.LATF0 = 0;
				LATBbits.LATB11 = 1;
			} else {
				LATFbits.LATF0 = 1;
				LATBbits.LATB11 = 0;
			}
		}

		in_cursor += 1;
	}

	if (rws_ctx.buf_used) {
		SPI_Transmit(&hspi1, rws_ctx.buf, rws_ctx.buf_used);
		rws_ctx.buf_used = 0;
	}

}

void USB_CDC_RxCallback(void *userp, uint8_t *buf, uint16_t len) {
	retrowave_bus_tx(buf, len);
}

USBDeluxeDevice_CDC_ACM_IOps cdc_iops = {
	.RxDone = USB_CDC_RxCallback
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

	// Use RB11 (pin 31 on RPi connector) as SPI CS
	LATBbits.LATB11 = 1;	// Set output latch to 1
	TRISBbits.TRISB11 = 0;	// Set output mode

	// Initialize SPI
	SPI_SetSpeedByPrescaler(&hspi1, SPI_PPRE_1_1, SPI_SPRE_6_1);
	SPI_Initialize(&hspi1, SPI_MASTER | SPI_MODE_0 | SPI_NO_CS);

	// Initialize USB device config
	USBDeluxe_Device_ConfigInit(0x04d8, 0xe966, "SudoMaker", "RetroWave USB Adapter (PICo24)", "123");
	// Add one CDC ACM instance
	USBDeluxe_DeviceFunction_Add_CDC_ACM(NULL, &cdc_iops);
	// Apply USB device config
	USBDeluxe_Device_ConfigApply();
	// Set USB to DEVICE mode
	USBDeluxe_SetRole(USB_ROLE_DEVICE);

	while (1) {
		// Run USB tasks
		USBDeluxe_Tasks();
	}

}