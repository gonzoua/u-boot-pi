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
#include <asm/arch/mbox.h>
#include <asm/arch/sdhci.h>
#include <asm/global_data.h>
#include <libfdt.h>

DECLARE_GLOBAL_DATA_PTR;

struct msg_get_arm_mem {
	struct bcm2835_mbox_hdr hdr;
	struct bcm2835_mbox_tag_get_arm_mem get_arm_mem;
	u32 end_tag;
};

struct msg_get_power_state {
	struct bcm2835_mbox_hdr hdr;
	struct bcm2835_mbox_tag_get_power_state get_power_state;
	u32 end_tag;
};

struct msg_set_power_state {
	struct bcm2835_mbox_hdr hdr;
	struct bcm2835_mbox_tag_set_power_state set_power_state;
	u32 end_tag;
};

struct msg_get_clock_rate {
	struct bcm2835_mbox_hdr hdr;
	struct bcm2835_mbox_tag_get_clock_rate get_clock_rate;
	u32 end_tag;
};

#define SMSC_MAC_SIGNATURE "smsc95xx.macaddr="
#define SMSC_MAC_SIGNATURE_LEN strlen("smsc95xx.macaddr=")
// XX:XX:XX:XX:XX:XX
#define MAC_LEN (17)

int dram_init(void)
{
	ALLOC_ALIGN_BUFFER(struct msg_get_arm_mem, msg, 1, 16);
	int ret;

	BCM2835_MBOX_INIT_HDR(msg);
	BCM2835_MBOX_INIT_TAG(&msg->get_arm_mem, GET_ARM_MEMORY);

	ret = bcm2835_mbox_call_prop(BCM2835_MBOX_PROP_CHAN, &msg->hdr);
	if (ret) {
		printf("bcm2835: Could not query ARM memory size\n");
		return -1;
	}

	gd->ram_size = msg->get_arm_mem.body.resp.mem_size;

	return 0;
}

int board_init(void)
{
	gd->bd->bi_boot_params = 0x100;

	return 0;
}

int board_mmc_init(void)
{
	ALLOC_ALIGN_BUFFER(struct msg_set_power_state, msg_pwr, 1, 16);
	ALLOC_ALIGN_BUFFER(struct msg_get_clock_rate, msg_clk, 1, 16);
	int ret;

	BCM2835_MBOX_INIT_HDR(msg_pwr);
	BCM2835_MBOX_INIT_TAG(&msg_pwr->set_power_state,
				SET_POWER_STATE);
	msg_pwr->set_power_state.body.req.device_id =
		BCM2835_MBOX_POWER_DEVID_SDHCI;
	msg_pwr->set_power_state.body.req.state =
		BCM2835_MBOX_SET_POWER_STATE_REQ_ON |
		BCM2835_MBOX_SET_POWER_STATE_REQ_WAIT;

	ret = bcm2835_mbox_call_prop(BCM2835_MBOX_PROP_CHAN,
					&msg_pwr->hdr);
	if (ret) {
		printf("bcm2835: Could not set SDHCI power state\n");
		return -1;
	}

	BCM2835_MBOX_INIT_HDR(msg_clk);
	BCM2835_MBOX_INIT_TAG(&msg_clk->get_clock_rate, GET_CLOCK_RATE);
	msg_clk->get_clock_rate.body.req.clock_id = BCM2835_MBOX_CLOCK_ID_EMMC;

	ret = bcm2835_mbox_call_prop(BCM2835_MBOX_PROP_CHAN, &msg_clk->hdr);
	if (ret) {
		printf("bcm2835: Could not query eMMC clock rate\n");
		return -1;
	}

	return bcm2835_sdhci_init(BCM2835_SDHCI_BASE,
				  msg_clk->get_clock_rate.body.resp.rate_hz);
}

int misc_init_r()
{	
	uint32_t tag_value;
	unsigned char *atags;
	unsigned char *mac_param;
	unsigned char mac[MAC_LEN + 1];
	ALLOC_ALIGN_BUFFER(struct msg_set_power_state, msg_pwr, 1, 16);
	int ret;
	int check_atags = 0;
#if defined(CONFIG_OF_LIBFDT)
	int proplen;
	void *fdt = (void*)0x100;
	unsigned char *prop;

	if (fdt_check_header(fdt) == 0) {
        	int nodeoff = fdt_path_offset(fdt, "/axi/usb/hub/ethernet");
        	if (nodeoff >= 0) {
			prop = (unsigned char*)fdt_getprop(fdt, nodeoff, "mac-address", &proplen);
                        if (prop && (proplen == 6)) {
				snprintf(mac, sizeof(mac)-1,
				   "%02x:%02x:%02x:%02x:%02x:%02x",
				   prop[0], prop[1], prop[2],
				   prop[3], prop[4], prop[5]);
				mac[MAC_LEN] = 0;
				setenv("usbethaddr", mac);
                        }

		}
	}
#endif
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

	BCM2835_MBOX_INIT_HDR(msg_pwr);
	BCM2835_MBOX_INIT_TAG(&msg_pwr->set_power_state,
				SET_POWER_STATE);
	msg_pwr->set_power_state.body.req.device_id =
		BCM2835_MBOX_POWER_DEVID_USB_HCD;
	msg_pwr->set_power_state.body.req.state =
		BCM2835_MBOX_SET_POWER_STATE_REQ_ON |
		BCM2835_MBOX_SET_POWER_STATE_REQ_WAIT;

	ret = bcm2835_mbox_call_prop(BCM2835_MBOX_PROP_CHAN,
					&msg_pwr->hdr);
	if (ret) {
		printf("bcm2835: Could not set USB power state\n");
		return -1;
	}
	else {
		printf("bcm2835: USB power in ON\n");
	}

	return 0;
}
