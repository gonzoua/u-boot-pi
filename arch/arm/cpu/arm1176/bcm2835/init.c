/*
 * TNETV107X: Architecture initialization
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <config.h>
#include <common.h>
#include <asm/arch/regs.h>

DECLARE_GLOBAL_DATA_PTR;

#define BCM2835_ARM_MBOX0_BASE 0x2000b880
#define BCM2835_ARM_MBOX1_BASE 0x2000b8a0
#define MBOX_CHAN_POWER   0 /* for use by the power management interface */
#define MBOX_MSG(chan, data28)      (((data28) & ~0xf) | ((chan) & 0xf))

int arch_cpu_init(void)
{
#if defined(CONFIG_USB_DWC_OTG) || defined(CONFIG_BCM2835_SDHCI)
	/*
	 * Request power for USB 
	 */
	volatile uint32_t *mbox0_read = (volatile uint32_t *)BCM2835_ARM_MBOX0_BASE;
	volatile uint32_t *mbox0_write = (volatile uint32_t *)BCM2835_ARM_MBOX1_BASE;
	volatile uint32_t *mbox0_status = (volatile uint32_t *)(BCM2835_ARM_MBOX0_BASE + 0x18);
	volatile uint32_t *mbox0_config = (volatile uint32_t *)(BCM2835_ARM_MBOX0_BASE + 0x1C);
	uint32_t val = 0;

#if defined(CONFIG_USB_DWC_OTG)
	val |= 8 << 4;
#endif

#if defined(CONFIG_BCM2835_SDHCI)
	val |= 1 << 4;
#endif

	while (*mbox0_status & 0x80000000) {
	}

	*mbox0_write = MBOX_MSG(0, val); 
#endif

	return 0;
}
