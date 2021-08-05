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

#include <Lua/lua.h>
#include <Lua/lauxlib.h>
#include <Lua/lualib.h>

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
	USBDeluxe_Device_ConfigInit(0x04d8, 0xe966, "SudoMaker", "PotatoPi PICo24", "123");
	// Apply USB device config
	USBDeluxe_Device_ConfigApply();
	// Set USB to DEVICE mode
	USBDeluxe_SetRole(USB_ROLE_DEVICE);

	lua_State *L = luaL_newstate();
	printf("luaL_newstate done\n");

	luaL_openlibs(L);
	printf("luaL_openlibs done\n");

	while (1) {
		luaL_dostring(L, "print(\"Hello Lua!\")");
	}

}