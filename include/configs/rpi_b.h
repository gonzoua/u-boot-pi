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

#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/sizes.h>

#define CONFIG_CMD_ELF
#define CONFIG_API
#define CONFIG_SYS_MMC_MAX_DEVICE 1

/* Architecture, CPU, etc.*/
#define CONFIG_ARM1176
#define CONFIG_BCM2835
#define CONFIG_ARCH_CPU_INIT
#define CONFIG_MISC_INIT_R

/* Timer */
#define CONFIG_SYS_HZ			1000000

/* Memory layout */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_TEXT_BASE		0x00008000
#define CONFIG_SYS_SDRAM_BASE		0x00000000
#define PHYS_SDRAM_SIZE			SZ_128M
#define CONFIG_SYS_UBOOT_BASE		CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_INIT_RAM_SIZE	PHYS_SDRAM_SIZE
#define CONFIG_SYS_INIT_SP_ADDR		(CONFIG_SYS_SDRAM_BASE + \
					 CONFIG_SYS_INIT_RAM_SIZE - \
					 GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_MALLOC_LEN		SZ_4M
#define CONFIG_SYS_MEMTEST_START	0x00100000
#define CONFIG_SYS_MEMTEST_END		0x00200000

/* Flash */
#define CONFIG_SYS_NO_FLASH

/* Devices */
/* None yet */

/* Console UART */
#define CONFIG_PL011_SERIAL
#define CONFIG_PL011_CLOCK		3000000
#define CONFIG_PL01x_PORTS		{ (void *)0x20201000 }
#define CONFIG_CONS_INDEX		0
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/* Console configuration */
#define CONFIG_SYS_CBSIZE		1024
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE +		\
					 sizeof(CONFIG_SYS_PROMPT) + 16)

/* SD/MMC configuration */
#define CONFIG_GENERIC_MMC
#define CONFIG_MMC
#define CONFIG_SDHCI
#define	CONFIG_MMC_SDHCI_IO_ACCESSORS
#define CONFIG_BCM2835_SDHCI

/* ATAGS */
#define CONFIG_CMDLINE_TAG		1
#define CONFIG_SETUP_MEMORY_TAGS	1
#define CONFIG_INITRD_TAG		1

/* Environment */
#define CONFIG_ENV_SIZE			SZ_16K
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_SYS_LOAD_ADDR		0x1000000
#define CONFIG_BOOTDELAY	3

#define CONFIG_EXTRA_ENV_SETTINGS \
	"loadaddr=0x00008000\0" \
	"mmcdev=0\0" \
	"loadbootscript=fatload mmc ${mmcdev} ${loadaddr} boot.scr\0" \
	"bootscript=echo Running bootscript from mmc${mmcdev} ...; " \
		"source ${loadaddr}\0" \
	"bootenv=uEnv.txt\0" \
	"loadbootenv=fatload mmc ${mmcdev} ${loadaddr} ${bootenv}\0" \
	"importbootenv=echo Importing environment from mmc ...; " \
		"env import -t $loadaddr $filesize\0" \

#define CONFIG_BOOTCOMMAND \
	"if mmc rescan ${mmcdev}; then " \
		"if run loadbootenv; then " \
			"run importbootenv; " \
		"fi; " \
		"if run loadbootscript; then " \
			"run bootscript; " \
		"fi; " \
	"fi"

/* Shell */
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_MAXARGS		8
#define CONFIG_SYS_PROMPT		"U-Boot> "
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING
#define CONFIG_COMMAND_HISTORY
#define CONFIG_AUTO_COMPLETE
#define CONFIG_CMD_SOURCE

/* USB Networking options */
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_SMSC95XX
#define CONFIG_USB_DWC_OTG
#define	CONFIG_USB_STORAGE
/* Required for USB */
#define CONFIG_DOS_PARTITION

/* Commands */
#define CONFIG_CMD_RUN
#define CONFIG_CMD_ECHO
#define CONFIG_CMD_MEMORY
#define CONFIG_CMD_MISC
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_USB
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP

#define CONFIG_CMD_MMC
#define CONFIG_CMD_FAT
#define	CONFIG_CMD_EXT2

#endif
