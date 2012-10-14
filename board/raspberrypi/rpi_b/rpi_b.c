/*
 * (C) Copyright 2012 Stephen Warren
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

#define SMSC_MAC_SIGNATURE "smsc95xx.macaddr="
#define SMSC_MAC_SIGNATURE_LEN strlen("smsc95xx.macaddr=")
// XX:XX:XX:XX:XX:XX
#define MAC_LEN (17)

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_SIZE;

	return 0;
}

int board_init(void)
{
	return 0;
}

int misc_init_r()
{	
	uint32_t tag_value;
	unsigned char *atags;
	unsigned char *mac_param;
	unsigned char mac[MAC_LEN + 1];

	for (atags = 0x100; atags < 0x4000; atags += 4) {
		memcpy(&tag_value, atags, sizeof(uint32_t));
		if (tag_value == ATAG_CMDLINE) {
			atags += 4;
			setenv("bootargs", atags);
			if (mac_param = strstr(atags, SMSC_MAC_SIGNATURE)) {
				strncpy(mac, 
				  mac_param + SMSC_MAC_SIGNATURE_LEN,
				  MAC_LEN);
				mac[MAC_LEN] = 0;
				setenv("usbethaddr", mac);
			}
		}

	}

	return 0;
}
