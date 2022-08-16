/*
 *  linux/drivers/mmc/host/sdhci.h - Secure Digital Host Controller Interface driver
 *
 * Header file for Host Controller registers and I/O accessors.
 *
 *  Copyright (C) 2005-2008 Pierre Ossman, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 */
#ifndef __SDHCI_HW_H
#define __SDHCI_HW_H

#include <linux/scatterlist.h>
#include <linux/compiler.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/ratelimit.h>
#include <linux/mmc/host.h>

/*
 * Controller registers
 */

#define SDHCI_DMA_ADDRESS	0x00
#define SDHCI_ARGUMENT2		SDHCI_DMA_ADDRESS

#define SDHCI_BLOCK_SIZE	0x04
#define  SDHCI_MAKE_BLKSZ(dma, blksz) (((dma & 0x7) << 12) | (blksz & 0xFFF))

#define SDHCI_BLOCK_COUNT	0x06

#define SDHCI_ARGUMENT		0x08

#define SDHCI_TRANSFER_MODE	0x0C
#define  SDHCI_TRNS_DMA		0x01
#define  SDHCI_TRNS_BLK_CNT_EN	0x02
#define  SDHCI_TRNS_AUTO_CMD12	0x04
#define  SDHCI_TRNS_AUTO_CMD23	0x08
#define  SDHCI_TRNS_READ	0x10
#define  SDHCI_TRNS_MULTI	0x20

#define SDHCI_COMMAND		0x0E
#define  SDHCI_CMD_RESP_MASK	0x03
#define  SDHCI_CMD_CRC		0x08
#define  SDHCI_CMD_INDEX	0x10
#define  SDHCI_CMD_DATA		0x20
#define  SDHCI_CMD_ABORTCMD	0xC0

#define  SDHCI_CMD_RESP_NONE	0x00
#define  SDHCI_CMD_RESP_LONG	0x01
#define  SDHCI_CMD_RESP_SHORT	0x02
#define  SDHCI_CMD_RESP_SHORT_BUSY 0x03

#define SDHCI_MAKE_CMD(c, f) (((c & 0xff) << 8) | (f & 0xff))
#define SDHCI_GET_CMD(c) ((c>>8) & 0x3f)

#define SDHCI_RESPONSE		0x10

#define SDHCI_BUFFER		0x20

#define SDHCI_PRESENT_STATE	0x24
#define  SDHCI_CMD_INHIBIT	0x00000001
#define  SDHCI_DATA_INHIBIT	0x00000002
#define  SDHCI_DOING_WRITE	0x00000100
#define  SDHCI_DOING_READ	0x00000200
#define  SDHCI_SPACE_AVAILABLE	0x00000400
#define  SDHCI_DATA_AVAILABLE	0x00000800
#define  SDHCI_CARD_PRESENT	0x00010000
#define  SDHCI_WRITE_PROTECT	0x00080000
#define  SDHCI_DATA_LVL_MASK	0x00F00000
#define   SDHCI_DATA_LVL_SHIFT	20
#define   SDHCI_DATA_0_LVL_MASK	0x00100000

#define SDHCI_HOST_CONTROL	0x28
#define  SDHCI_CTRL_LED		0x01
#define  SDHCI_CTRL_4BITBUS	0x02
#define  SDHCI_CTRL_HISPD	0x04
#define  SDHCI_CTRL_DMA_MASK	0x18
#define   SDHCI_CTRL_SDMA	0x00
#define   SDHCI_CTRL_ADMA1	0x08
#define   SDHCI_CTRL_ADMA32	0x10
#define   SDHCI_CTRL_ADMA64	0x18
#define   SDHCI_CTRL_8BITBUS	0x20

#define SDHCI_POWER_CONTROL	0x29
#define  SDHCI_POWER_ON		0x01
#define  SDHCI_POWER_180	0x0A
#define  SDHCI_POWER_300	0x0C
#define  SDHCI_POWER_330	0x0E

#define SDHCI_BLOCK_GAP_CONTROL	0x2A

#define SDHCI_WAKE_UP_CONTROL	0x2B
#define  SDHCI_WAKE_ON_INT	0x01
#define  SDHCI_WAKE_ON_INSERT	0x02
#define  SDHCI_WAKE_ON_REMOVE	0x04

#define SDHCI_CLOCK_CONTROL	0x2C
#define  SDHCI_DIVIDER_SHIFT	8
#define  SDHCI_DIVIDER_HI_SHIFT	6
#define  SDHCI_DIV_MASK	0xFF
#define  SDHCI_DIV_MASK_LEN	8
#define  SDHCI_DIV_HI_MASK	0x300
#define  SDHCI_PROG_CLOCK_MODE	0x0020
#define  SDHCI_CLOCK_CARD_EN	0x0004
#define  SDHCI_CLOCK_INT_STABLE	0x0002
#define  SDHCI_CLOCK_INT_EN	0x0001

#define SDHCI_TIMEOUT_CONTROL	0x2E

#define SDHCI_SOFTWARE_RESET	0x2F
#define  SDHCI_RESET_ALL	0x01
#define  SDHCI_RESET_CMD	0x02
#define  SDHCI_RESET_DATA	0x04

#define SDHCI_INT_STATUS	0x30
#define SDHCI_INT_ENABLE	0x34
#define SDHCI_SIGNAL_ENABLE	0x38
#define  SDHCI_INT_RESPONSE	0x00000001
#define  SDHCI_INT_DATA_END	0x00000002
#define  SDHCI_INT_BLK_GAP	0x00000004
#define  SDHCI_INT_DMA_END	0x00000008
#define  SDHCI_INT_SPACE_AVAIL	0x00000010
#define  SDHCI_INT_DATA_AVAIL	0x00000020
#define  SDHCI_INT_CARD_INSERT	0x00000040
#define  SDHCI_INT_CARD_REMOVE	0x00000080
#define  SDHCI_INT_CARD_INT	0x00000100
#define  SDHCI_INT_ERROR	0x00008000
#define  SDHCI_INT_TIMEOUT	0x00010000
#define  SDHCI_INT_CRC		0x00020000
#define  SDHCI_INT_END_BIT	0x00040000
#define  SDHCI_INT_INDEX	0x00080000
#define  SDHCI_INT_DATA_TIMEOUT	0x00100000
#define  SDHCI_INT_DATA_CRC	0x00200000
#define  SDHCI_INT_DATA_END_BIT	0x00400000
#define  SDHCI_INT_BUS_POWER	0x00800000
#define  SDHCI_INT_AUTO_CMD_ERR	0x01000000
#define  SDHCI_INT_ADMA_ERROR	0x02000000

#define  SDHCI_INT_NORMAL_MASK	0x00007FFF
#define  SDHCI_INT_ERROR_MASK	0xFFFF8000

#define  SDHCI_INT_CMD_MASK	(SDHCI_INT_RESPONSE | SDHCI_INT_TIMEOUT | \
		SDHCI_INT_CRC | SDHCI_INT_END_BIT | SDHCI_INT_INDEX | \
				 SDHCI_INT_AUTO_CMD_ERR)

#define  SDHCI_INT_DATA_MASK	(SDHCI_INT_DATA_END | SDHCI_INT_DMA_END | \
		SDHCI_INT_DATA_AVAIL | SDHCI_INT_SPACE_AVAIL | \
		SDHCI_INT_DATA_TIMEOUT | SDHCI_INT_DATA_CRC | \
		SDHCI_INT_DATA_END_BIT | SDHCI_INT_ADMA_ERROR | \
		SDHCI_INT_BLK_GAP)

#define SDHCI_INT_CMDQ_EN	(0x1 << 14)
#define SDHCI_INT_ALL_MASK	((unsigned int)-1)

#define SDHCI_AUTO_CMD_ERR		0x3C
#define SDHCI_AUTO_CMD12_NOT_EXEC	0x0001
#define SDHCI_AUTO_CMD_TIMEOUT_ERR	0x0002
#define SDHCI_AUTO_CMD_CRC_ERR		0x0004
#define SDHCI_AUTO_CMD_ENDBIT_ERR	0x0008
#define SDHCI_AUTO_CMD_INDEX_ERR	0x0010
#define SDHCI_AUTO_CMD12_NOT_ISSUED	0x0080

#define SDHCI_HOST_CONTROL2		0x3E
#define  SDHCI_CTRL_UHS_MASK		0x0007
#define   SDHCI_CTRL_UHS_SDR12		0x0000
#define   SDHCI_CTRL_UHS_SDR25		0x0001
#define   SDHCI_CTRL_UHS_SDR50		0x0002
#define   SDHCI_CTRL_UHS_SDR104		0x0003
#define   SDHCI_CTRL_UHS_DDR50		0x0004
#define   SDHCI_CTRL_HS400		0x0005 /* Non-standard */
#define  SDHCI_CTRL_VDD_180		0x0008
#define  SDHCI_CTRL_DRV_TYPE_MASK	0x0030
#define   SDHCI_CTRL_DRV_TYPE_B		0x0000
#define   SDHCI_CTRL_DRV_TYPE_A		0x0010
#define   SDHCI_CTRL_DRV_TYPE_C		0x0020
#define   SDHCI_CTRL_DRV_TYPE_D		0x0030
#define  SDHCI_CTRL_EXEC_TUNING		0x0040
#define  SDHCI_CTRL_TUNED_CLK		0x0080
#define  SDHCI_CTRL_ASYNC_INT_ENABLE	0x4000
#define  SDHCI_CTRL_PRESET_VAL_ENABLE	0x8000

#define SDHCI_CAPABILITIES	0x40
#define  SDHCI_TIMEOUT_CLK_MASK	0x0000003F
#define  SDHCI_TIMEOUT_CLK_SHIFT 0
#define  SDHCI_TIMEOUT_CLK_UNIT	0x00000080
#define  SDHCI_CLOCK_BASE_MASK	0x00003F00
#define  SDHCI_CLOCK_V3_BASE_MASK	0x0000FF00
#define  SDHCI_CLOCK_BASE_SHIFT	8
#define  SDHCI_MAX_BLOCK_MASK	0x00030000
#define  SDHCI_MAX_BLOCK_SHIFT  16
#define  SDHCI_CAN_DO_8BIT	0x00040000
#define  SDHCI_CAN_DO_ADMA2	0x00080000
#define  SDHCI_CAN_DO_ADMA1	0x00100000
#define  SDHCI_CAN_DO_HISPD	0x00200000
#define  SDHCI_CAN_DO_SDMA	0x00400000
#define  SDHCI_CAN_VDD_330	0x01000000
#define  SDHCI_CAN_VDD_300	0x02000000
#define  SDHCI_CAN_VDD_180	0x04000000
#define  SDHCI_CAN_64BIT	0x10000000
#define  SDHCI_CAN_ASYNC_INT	0x20000000

#define  SDHCI_SUPPORT_SDR50	0x00000001
#define  SDHCI_SUPPORT_SDR104	0x00000002
#define  SDHCI_SUPPORT_DDR50	0x00000004
#define  SDHCI_DRIVER_TYPE_A	0x00000010
#define  SDHCI_DRIVER_TYPE_C	0x00000020
#define  SDHCI_DRIVER_TYPE_D	0x00000040
#define  SDHCI_RETUNING_TIMER_COUNT_MASK	0x00000F00
#define  SDHCI_RETUNING_TIMER_COUNT_SHIFT	8
#define  SDHCI_USE_SDR50_TUNING			0x00002000
#define  SDHCI_RETUNING_MODE_MASK		0x0000C000
#define  SDHCI_RETUNING_MODE_SHIFT		14
#define  SDHCI_CLOCK_MUL_MASK	0x00FF0000
#define  SDHCI_CLOCK_MUL_SHIFT	16
#define  SDHCI_SUPPORT_HS400	0x80000000 /* Non-standard */

#define SDHCI_CAPABILITIES_1	0x44

#define SDHCI_MAX_CURRENT		0x48
#define  SDHCI_MAX_CURRENT_LIMIT	0xFF
#define  SDHCI_MAX_CURRENT_330_MASK	0x0000FF
#define  SDHCI_MAX_CURRENT_330_SHIFT	0
#define  SDHCI_MAX_CURRENT_300_MASK	0x00FF00
#define  SDHCI_MAX_CURRENT_300_SHIFT	8
#define  SDHCI_MAX_CURRENT_180_MASK	0xFF0000
#define  SDHCI_MAX_CURRENT_180_SHIFT	16
#define   SDHCI_MAX_CURRENT_MULTIPLIER	4

/* 4C-4F reserved for more max current */

#define SDHCI_SET_ACMD12_ERROR	0x50
#define SDHCI_SET_INT_ERROR	0x52

#define SDHCI_ADMA_ERROR	0x54

/* 55-57 reserved */

#define SDHCI_ADMA_ADDRESS	0x58
#define SDHCI_ADMA_ADDRESS_HI	0x5C

/* 60-FB reserved */

#define SDHCI_PRESET_FOR_HIGH_SPEED	0x64
#define SDHCI_PRESET_FOR_SDR12 0x66
#define SDHCI_PRESET_FOR_SDR25 0x68
#define SDHCI_PRESET_FOR_SDR50 0x6A
#define SDHCI_PRESET_FOR_SDR104        0x6C
#define SDHCI_PRESET_FOR_DDR50 0x6E
#define SDHCI_PRESET_FOR_HS400 0x74 /* Non-standard */
#define SDHCI_PRESET_DRV_MASK  0xC000
#define SDHCI_PRESET_DRV_SHIFT  14
#define SDHCI_PRESET_CLKGEN_SEL_MASK   0x400
#define SDHCI_PRESET_CLKGEN_SEL_SHIFT	10
#define SDHCI_PRESET_SDCLK_FREQ_MASK   0x3FF
#define SDHCI_PRESET_SDCLK_FREQ_SHIFT	0

#define SDHCI_SLOT_INT_STATUS	0xFC

#define SDHCI_HOST_VERSION	0xFE
#define  SDHCI_VENDOR_VER_MASK	0xFF00
#define  SDHCI_VENDOR_VER_SHIFT	8
#define  SDHCI_SPEC_VER_MASK	0x00FF
#define  SDHCI_SPEC_VER_SHIFT	0
#define   SDHCI_SPEC_100	0
#define   SDHCI_SPEC_200	1
#define   SDHCI_SPEC_300	2

/*
 * End of controller registers.
 */

#define SDHCI_MAX_DIV_SPEC_200	256
#define SDHCI_MAX_DIV_SPEC_300	2046

/*
 * Host SDMA buffer boundary. Valid values from 4K to 512K in powers of 2.
 */
#define SDHCI_DEFAULT_BOUNDARY_SIZE  (512 * 1024)
#define SDHCI_DEFAULT_BOUNDARY_ARG   (ilog2(SDHCI_DEFAULT_BOUNDARY_SIZE) - 12)

/* ADMA2 32-bit DMA descriptor size */
#define SDHCI_ADMA2_32_DESC_SZ	8

/* ADMA2 32-bit descriptor */
struct sdhci_adma2_32_desc {
	__le16	cmd;
	__le16	len;
	__le32	addr;
}  __packed __aligned(4);

/* ADMA2 data alignment */
#define SDHCI_ADMA2_ALIGN	4
#define SDHCI_ADMA2_MASK	(SDHCI_ADMA2_ALIGN - 1)

/*
 * ADMA2 descriptor alignment.  Some controllers (e.g. Intel) require 8 byte
 * alignment for the descriptor table even in 32-bit DMA mode.  Memory
 * allocation is at least 8 byte aligned anyway, so just stipulate 8 always.
 */
#define SDHCI_ADMA2_DESC_ALIGN	8

/* ADMA2 64-bit DMA descriptor size */
#define SDHCI_ADMA2_64_DESC_SZ	12

/*
 * ADMA2 64-bit descriptor. Note 12-byte descriptor can't always be 8-byte
 * aligned.
 */
struct sdhci_adma2_64_desc {
	__le16	cmd;
	__le16	len;
	__le32	addr_lo;
	__le32	addr_hi;
}  __packed __aligned(4);

#define ADMA2_TRAN_VALID	0x21
#define ADMA2_NOP_END_VALID	0x3
#define ADMA2_END		0x2

/*
 * Maximum segments assuming a 512KiB maximum requisition size and a minimum
 * 4KiB page size.
 */
#define SDHCI_MAX_SEGS		128

enum sdhci_cookie {
	COOKIE_UNMAPPED,
	COOKIE_MAPPED,
	COOKIE_GIVEN,
};

enum sdhci_power_policy {
	SDHCI_PERFORMANCE_MODE,
	SDHCI_POWER_SAVE_MODE,
	SDHCI_POWER_POLICY_NUM /* Always keep this one last */
};

struct sdhci_host {
	/* Data set by hardware interface driver */
	const char *hw_name;	/* Hardware bus name */

	unsigned int quirks;	/* Deviations from spec. */

/* Controller doesn't honor resets unless we touch the clock register */
#define SDHCI_QUIRK_CLOCK_BEFORE_RESET			(1<<0)
/* Controller has bad caps bits, but really supports DMA */
#define SDHCI_QUIRK_FORCE_DMA				(1<<1)
/* Controller doesn't like to be reset when there is no card inserted. */
#define SDHCI_QUIRK_NO_CARD_NO_RESET			(1<<2)
/* Controller doesn't like clearing the power reg before a change */
#define SDHCI_QUIRK_SINGLE_POWER_WRITE			(1<<3)
/* Controller has flaky internal state so reset it on each ios change */
#define SDHCI_QUIRK_RESET_CMD_DATA_ON_IOS		(1<<4)
/* Controller has an unusable DMA engine */
#define SDHCI_QUIRK_BROKEN_DMA				(1<<5)
/* Controller has an unusable ADMA engine */
#define SDHCI_QUIRK_BROKEN_ADMA				(1<<6)
/* Controller can only DMA from 32-bit aligned addresses */
#define SDHCI_QUIRK_32BIT_DMA_ADDR			(1<<7)
/* Controller can only DMA chunk sizes that are a multiple of 32 bits */
#define SDHCI_QUIRK_32BIT_DMA_SIZE			(1<<8)
/* Controller can only ADMA chunks that are a multiple of 32 bits */
#define SDHCI_QUIRK_32BIT_ADMA_SIZE			(1<<9)
/* Controller needs to be reset after each request to stay stable */
#define SDHCI_QUIRK_RESET_AFTER_REQUEST			(1<<10)
/* Controller needs voltage and power writes to happen separately */
#define SDHCI_QUIRK_NO_SIMULT_VDD_AND_POWER		(1<<11)
/* Controller provides an incorrect timeout value for transfers */
#define SDHCI_QUIRK_BROKEN_TIMEOUT_VAL			(1<<12)
/* Controller has an issue with buffer bits for small transfers */
#define SDHCI_QUIRK_BROKEN_SMALL_PIO			(1<<13)
/* Controller does not provide transfer-complete interrupt when not busy */
#define SDHCI_QUIRK_NO_BUSY_IRQ				(1<<14)
/* Controller has unreliable card detection */
#define SDHCI_QUIRK_BROKEN_CARD_DETECTION		(1<<15)
/* Controller reports inverted write-protect state */
#define SDHCI_QUIRK_INVERTED_WRITE_PROTECT		(1<<16)
/* Controller does not like fast PIO transfers */
#define SDHCI_QUIRK_PIO_NEEDS_DELAY			(1<<18)
/* Controller has to be forced to use block size of 2048 bytes */
#define SDHCI_QUIRK_FORCE_BLK_SZ_2048			(1<<20)
/* Controller cannot do multi-block transfers */
#define SDHCI_QUIRK_NO_MULTIBLOCK			(1<<21)
/* Controller can only handle 1-bit data transfers */
#define SDHCI_QUIRK_FORCE_1_BIT_DATA			(1<<22)
/* Controller needs 10ms delay between applying power and clock */
#define SDHCI_QUIRK_DELAY_AFTER_POWER			(1<<23)
/* Controller uses SDCLK instead of TMCLK for data timeouts */
#define SDHCI_QUIRK_DATA_TIMEOUT_USES_SDCLK		(1<<24)
/* Controller reports wrong base clock capability */
#define SDHCI_QUIRK_CAP_CLOCK_BASE_BROKEN		(1<<25)
/* Controller cannot support End Attribute in NOP ADMA descriptor */
#define SDHCI_QUIRK_NO_ENDATTR_IN_NOPDESC		(1<<26)
/* Controller is missing device caps. Use caps provided by host */
#define SDHCI_QUIRK_MISSING_CAPS			(1<<27)
/* Controller uses Auto CMD12 command to stop the transfer */
#define SDHCI_QUIRK_MULTIBLOCK_READ_ACMD12		(1<<28)
/* Controller doesn't have HISPD bit field in HI-SPEED SD card */
#define SDHCI_QUIRK_NO_HISPD_BIT			(1<<29)
/* Controller treats ADMA descriptors with length 0000h incorrectly */
#define SDHCI_QUIRK_BROKEN_ADMA_ZEROLEN_DESC		(1<<30)
/* The read-only detection via SDHCI_PRESENT_STATE register is unstable */
#define SDHCI_QUIRK_UNSTABLE_RO_DETECT			(1<<31)

	unsigned int quirks2;	/* More deviations from spec. */

#define SDHCI_QUIRK2_HOST_OFF_CARD_ON			(1<<0)
#define SDHCI_QUIRK2_HOST_NO_CMD23			(1<<1)
/* The system physically doesn't support 1.8v, even if the host does */
#define SDHCI_QUIRK2_NO_1_8_V				(1<<2)
#define SDHCI_QUIRK2_PRESET_VALUE_BROKEN		(1<<3)
#define SDHCI_QUIRK2_CARD_ON_NEEDS_BUS_ON		(1<<4)
/* Controller has a non-standard host control register */
#define SDHCI_QUIRK2_BROKEN_HOST_CONTROL		(1<<5)
/* Controller does not support HS200 */
#define SDHCI_QUIRK2_BROKEN_HS200			(1<<6)
/* Controller does not support DDR50 */
#define SDHCI_QUIRK2_BROKEN_DDR50			(1<<7)
/* Stop command (CMD12) can set Transfer Complete when not using MMC_RSP_BUSY */
#define SDHCI_QUIRK2_STOP_WITH_TC			(1<<8)
/* Controller does not support 64-bit DMA */
#define SDHCI_QUIRK2_BROKEN_64_BIT_DMA			(1<<9)
/* need clear transfer mode register before send cmd */
#define SDHCI_QUIRK2_CLEAR_TRANSFERMODE_REG_BEFORE_CMD	(1<<10)
/* Capability register bit-63 indicates HS400 support */
#define SDHCI_QUIRK2_CAPS_BIT63_FOR_HS400		(1<<11)
/* forced tuned clock */
#define SDHCI_QUIRK2_TUNING_WORK_AROUND			(1<<12)
/* disable the block count for single block transactions */
#define SDHCI_QUIRK2_SUPPORT_SINGLE			(1<<13)
/* Controller broken with using ACMD23 */
#define SDHCI_QUIRK2_ACMD23_BROKEN			(1<<14)
/* Broken Clock divider zero in controller */
#define SDHCI_QUIRK2_CLOCK_DIV_ZERO_BROKEN		(1<<15)
/*
 * When internal clock is disabled, a delay is needed before modifying the
 * SD clock frequency or enabling back the internal clock.
 */
#define SDHCI_QUIRK2_NEED_DELAY_AFTER_INT_CLK_RST	(1<<16)

/*
 * Read Transfer Active/ Write Transfer Active may be not
 * de-asserted after end of transaction. Issue reset for DAT line.
 */
#define SDHCI_QUIRK2_RDWR_TX_ACTIVE_EOT                 (1<<17)
/*
 * Slow interrupt clearance at 400KHz may cause
 * host controller driver interrupt handler to
 * be called twice.
 */
#define SDHCI_QUIRK2_SLOW_INT_CLR			(1<<18)

/*
 * If the base clock can be scalable, then there should be no further
 * clock dividing as the input clock itself will be scaled down to
 * required frequency.
 */
#define SDHCI_QUIRK2_ALWAYS_USE_BASE_CLOCK		(1<<19)

/*
 * Ignore data timeout error for R1B commands as there will be no
 * data associated and the busy timeout value for these commands
 * could be lager than the maximum timeout value that controller
 * can handle.
 */
#define SDHCI_QUIRK2_IGNORE_DATATOUT_FOR_R1BCMD		(1<<20)

/*
 * The preset value registers are not properly initialized by
 * some hardware and hence preset value must not be enabled for
 * such controllers.
 */
#define SDHCI_QUIRK2_BROKEN_PRESET_VALUE		(1<<21)
/*
 * Some controllers define the usage of 0xF in data timeout counter
 * register (0x2E) which is actually a reserved bit as per
 * specification.
 */
#define SDHCI_QUIRK2_USE_RESERVED_MAX_TIMEOUT		(1<<22)
/*
 * This is applicable for controllers that advertize timeout clock
 * value in capabilities register (bit 5-0) as just 50MHz whereas the
 * base clock frequency is 200MHz. So, the controller internally
 * multiplies the value in timeout control register by 4 with the
 * assumption that driver always uses fixed timeout clock value from
 * capabilities register to calculate the timeout. But when the driver
 * uses SDHCI_QUIRK2_ALWAYS_USE_BASE_CLOCK base clock frequency is directly
 * controller by driver and it's rate varies upto max. 200MHz. This new quirk
 * will be used in such cases to avoid controller mulplication when timeout is
 * calculated based on the base clock.
 */
#define SDHCI_QUIRK2_DIVIDE_TOUT_BY_4 (1 << 23)

/*
 * Some SDHC controllers are unable to handle data-end bit error in
 * 1-bit mode of SDIO.
 */
#define SDHCI_QUIRK2_IGN_DATA_END_BIT_ERROR             (1<<24)

/* Controller has nonstandard clock management */
#define SDHCI_QUIRK_NONSTANDARD_CLOCK			(1<<25)
/* Use reset workaround in case sdhci reset timeouts */
#define SDHCI_QUIRK2_USE_RESET_WORKAROUND		(1<<26)
/* Some controllers doesn't have have any LED control */
#define SDHCI_QUIRK2_BROKEN_LED_CONTROL			(1<<27)
/*
 * Some controllers doesn't follow the tuning procedure as defined in spec.
 * The tuning data has to be compared from SW driver to validate the correct
 * phase.
 */
#define SDHCI_QUIRK2_NON_STANDARD_TUNING (1 << 28)
/*
 * Some controllers may use PIO mode to workaround HW issues in ADMA for
 * eMMC tuning commands.
 */
#define SDHCI_QUIRK2_USE_PIO_FOR_EMMC_TUNING (1 << 29)


	int irq;		/* Device IRQ */
	void __iomem *ioaddr;	/* Mapped address */

	const struct sdhci_ops *ops;	/* Low level hw interface */

	/* Internal data */
	struct mmc_host *mmc;	/* MMC structure */
	struct mmc_host_ops mmc_host_ops;	/* MMC host ops */
	u64 dma_mask;		/* custom DMA mask */
	u64 coherent_dma_mask;

#if defined(CONFIG_LEDS_CLASS) || defined(CONFIG_LEDS_CLASS_MODULE)
	struct led_classdev led;	/* LED control */
	char led_name[32];
#endif

	spinlock_t lock;	/* Mutex */

	int flags;		/* Host attributes */
#define SDHCI_USE_SDMA		(1<<0)	/* Host is SDMA capable */
#define SDHCI_USE_ADMA		(1<<1)	/* Host is ADMA capable */
#define SDHCI_REQ_USE_DMA	(1<<2)	/* Use DMA for this req. */
#define SDHCI_DEVICE_DEAD	(1<<3)	/* Device unresponsive */
#define SDHCI_SDR50_NEEDS_TUNING (1<<4)	/* SDR50 needs tuning */
#define SDHCI_AUTO_CMD12	(1<<6)	/* Auto CMD12 support */
#define SDHCI_AUTO_CMD23	(1<<7)	/* Auto CMD23 support */
#define SDHCI_PV_ENABLED	(1<<8)	/* Preset value enabled */
#define SDHCI_SDIO_IRQ_ENABLED	(1<<9)	/* SDIO irq enabled */
#define SDHCI_SDR104_NEEDS_TUNING (1<<10)	/* SDR104/HS200 needs tuning */
#define SDHCI_USE_64_BIT_DMA	(1<<12)	/* Use 64-bit DMA */
#define SDHCI_HS400_TUNING	(1<<13)	/* Tuning for HS400 */
#define SDHCI_HOST_IRQ_STATUS	 (1<<14) /* host->irq status */

	unsigned int version;	/* SDHCI spec. version */

	unsigned int max_clk;	/* Max possible freq (MHz) */
	unsigned int timeout_clk;	/* Timeout freq (KHz) */
	unsigned int clk_mul;	/* Clock Muliplier value */

	unsigned int clock;	/* Current clock (MHz) */
	u8 pwr;			/* Current voltage */

	bool runtime_suspended;	/* Host is runtime suspended */
	bool bus_on;		/* Bus power prevents runtime suspend */
	bool preset_enabled;	/* Preset is enabled */
	bool cdr_support;

	struct mmc_request *mrq;	/* Current request */
	struct mmc_command *cmd;	/* Current command */
	struct mmc_data *data;	/* Current data request */
	unsigned int data_early:1;	/* Data finished before cmd */
	unsigned int busy_handle:1;	/* Handling the order of Busy-end */

	struct sg_mapping_iter sg_miter;	/* SG state for PIO */
	unsigned int blocks;	/* remaining PIO blocks */

	int sg_count;		/* Mapped sg entries */

	void *adma_table;	/* ADMA descriptor table */
	void *align_buffer;	/* Bounce buffer */

	size_t adma_table_sz;	/* ADMA descriptor table size */
	size_t align_buffer_sz;	/* Bounce buffer size */

	dma_addr_t adma_addr;	/* Mapped ADMA descr. table */
	dma_addr_t align_addr;	/* Mapped bounce buffer */

	unsigned int desc_sz;	/* ADMA descriptor size */

	struct tasklet_struct finish_tasklet;	/* Tasklet structures */

	struct timer_list timer;	/* Timer for timeouts */

	u32 caps;		/* Alternative CAPABILITY_0 */
	u32 caps1;		/* Alternative CAPABILITY_1 */

	unsigned int            ocr_avail_sdio;	/* OCR bit masks */
	unsigned int            ocr_avail_sd;
	unsigned int            ocr_avail_mmc;
	u32 ocr_mask;		/* available voltages */

	unsigned		timing;		/* Current timing */

	u32			thread_isr;

	/* cached registers */
	u32			ier;

	wait_queue_head_t	buf_ready_int;	/* Waitqueue for Buffer Read Ready interrupt */
	unsigned int		tuning_done;	/* Condition flag set when CMD19 succeeds */

	unsigned int		tuning_count;	/* Timer count for re-tuning */
	unsigned int		tuning_mode;	/* Re-tuning mode supported by host */
#define SDHCI_TUNING_MODE_1	0
	ktime_t data_start_time;

	enum sdhci_power_policy power_policy;

	spinlock_t next_lock;	/* Mutex for next_data */

	bool is_crypto_en;
	bool crypto_reset_reqd;
	bool sdio_irq_async_status;

	u32 auto_cmd_err_sts;
	struct ratelimit_state dbg_dump_rs;
	struct cmdq_host *cq_host;
	int reset_wa_applied; /* reset workaround status */
	ktime_t reset_wa_t; /* time when the reset workaround is applied */
	int reset_wa_cnt; /* total number of times workaround is used */

	unsigned long private[0] ____cacheline_aligned;
};

struct sdhci_ops {
#ifdef CONFIG_MMC_SDHCI_IO_ACCESSORS
	u32		(*read_l)(struct sdhci_host *host, int reg);
	u16		(*read_w)(struct sdhci_host *host, int reg);
	u8		(*read_b)(struct sdhci_host *host, int reg);
	void		(*write_l)(struct sdhci_host *host, u32 val, int reg);
	void		(*write_w)(struct sdhci_host *host, u16 val, int reg);
	void		(*write_b)(struct sdhci_host *host, u8 val, int reg);
#endif

	void	(*set_clock)(struct sdhci_host *host, unsigned int clock);
	void	(*set_power)(struct sdhci_host *host, unsigned char mode,
			     unsigned short vdd);

	int		(*enable_dma)(struct sdhci_host *host);
	unsigned int	(*get_max_clock)(struct sdhci_host *host);
	unsigned int	(*get_min_clock)(struct sdhci_host *host);
	unsigned int	(*get_timeout_clock)(struct sdhci_host *host);
	unsigned int	(*get_max_timeout_count)(struct sdhci_host *host);
	void		(*set_timeout)(struct sdhci_host *host,
				       struct mmc_command *cmd);
	void		(*set_bus_width)(struct sdhci_host *host, int width);
	void (*platform_send_init_74_clocks)(struct sdhci_host *host,
					     u8 power_mode);
	unsigned int    (*get_ro)(struct sdhci_host *host);
	void		(*reset)(struct sdhci_host *host, u8 mask);
	int	(*platform_execute_tuning)(struct sdhci_host *host, u32 opcode);
	int	(*crypto_engine_cfg)(struct sdhci_host *host,
				struct mmc_request *mrq, u32 slot);
	int	(*crypto_engine_cmdq_cfg)(struct sdhci_host *host,
			struct mmc_request *mrq, u32 slot, u64 *ice_ctx);
	int	(*crypto_engine_cfg_end)(struct sdhci_host *host,
					struct mmc_request *mrq);
	int	(*crypto_engine_reset)(struct sdhci_host *host);
	void	(*crypto_cfg_reset)(struct sdhci_host *host, unsigned int slot);
	void	(*set_uhs_signaling)(struct sdhci_host *host, unsigned int uhs);
	void	(*hw_reset)(struct sdhci_host *host);
	void    (*adma_workaround)(struct sdhci_host *host, u32 intmask);
	unsigned int	(*get_max_segments)(void);
	void	(*platform_init)(struct sdhci_host *host);
#define REQ_BUS_OFF	(1 << 0)
#define REQ_BUS_ON	(1 << 1)
#define REQ_IO_LOW	(1 << 2)
#define REQ_IO_HIGH	(1 << 3)
	void    (*card_event)(struct sdhci_host *host);
	int	(*enhanced_strobe)(struct sdhci_host *host);
	void	(*platform_bus_voting)(struct sdhci_host *host, u32 enable);
	void	(*check_power_status)(struct sdhci_host *host, u32 req_type);
	int	(*config_auto_tuning_cmd)(struct sdhci_host *host,
					  bool enable,
					  u32 type);
	int	(*enable_controller_clock)(struct sdhci_host *host);
	void	(*clear_set_dumpregs)(struct sdhci_host *host, bool set);
	void	(*enhanced_strobe_mask)(struct sdhci_host *host, bool set);
	void	(*dump_vendor_regs)(struct sdhci_host *host);
	void	(*toggle_cdr)(struct sdhci_host *host, bool enable);
	void	(*voltage_switch)(struct sdhci_host *host);
	int	(*select_drive_strength)(struct sdhci_host *host,
					 struct mmc_card *card,
					 unsigned int max_dtr, int host_drv,
					 int card_drv, int *drv_type);
	int	(*notify_load)(struct sdhci_host *host, enum mmc_load state);
	int     (*check_temp)(struct sdhci_host *host);
	int     (*reg_temp_callback)(struct sdhci_host *host);
	int     (*dereg_temp_callback)(struct sdhci_host *host);
	void	(*reset_workaround)(struct sdhci_host *host, u32 enable);
	void	(*init)(struct sdhci_host *host);
	void	(*pre_req)(struct sdhci_host *host, struct mmc_request *req);
	void	(*post_req)(struct sdhci_host *host, struct mmc_request *req);
	unsigned int	(*get_current_limit)(struct sdhci_host *host);
};

#ifdef CONFIG_MMC_SDHCI_IO_ACCESSORS

static inline void sdhci_writel(struct sdhci_host *host, u32 val, int reg)
{
	if (unlikely(host->ops->write_l))
		host->ops->write_l(host, val, reg);
	else
		writel(val, host->ioaddr + reg);
}

static inline void sdhci_writew(struct sdhci_host *host, u16 val, int reg)
{
	if (unlikely(host->ops->write_w))
		host->ops->write_w(host, val, reg);
	else
		writew(val, host->ioaddr + reg);
}

static inline void sdhci_writeb(struct sdhci_host *host, u8 val, int reg)
{
	if (unlikely(host->ops->write_b))
		host->ops->write_b(host, val, reg);
	else
		writeb(val, host->ioaddr + reg);
}

static inline u32 sdhci_readl(struct sdhci_host *host, int reg)
{
	if (unlikely(host->ops->read_l))
		return host->ops->read_l(host, reg);
	else
		return readl(host->ioaddr + reg);
}

static inline u16 sdhci_readw(struct sdhci_host *host, int reg)
{
	if (unlikely(host->ops->read_w))
		return host->ops->read_w(host, reg);
	else
		return readw(host->ioaddr + reg);
}

static inline u8 sdhci_readb(struct sdhci_host *host, int reg)
{
	if (unlikely(host->ops->read_b))
		return host->ops->read_b(host, reg);
	else
		return readb(host->ioaddr + reg);
}

#else

static inline void sdhci_writel(struct sdhci_host *host, u32 val, int reg)
{
	writel(val, host->ioaddr + reg);
}

static inline void sdhci_writew(struct sdhci_host *host, u16 val, int reg)
{
	writew(val, host->ioaddr + reg);
}

static inline void sdhci_writeb(struct sdhci_host *host, u8 val, int reg)
{
	writeb(val, host->ioaddr + reg);
}

static inline u32 sdhci_readl(struct sdhci_host *host, int reg)
{
	return readl(host->ioaddr + reg);
}

static inline u16 sdhci_readw(struct sdhci_host *host, int reg)
{
	return readw(host->ioaddr + reg);
}

static inline u8 sdhci_readb(struct sdhci_host *host, int reg)
{
	return readb(host->ioaddr + reg);
}

#endif /* CONFIG_MMC_SDHCI_IO_ACCESSORS */

extern struct sdhci_host *sdhci_alloc_host(struct device *dev,
	size_t priv_size);
extern void sdhci_free_host(struct sdhci_host *host);

static inline void *sdhci_priv(struct sdhci_host *host)
{
	return (void *)host->private;
}

extern void sdhci_card_detect(struct sdhci_host *host);
extern int sdhci_add_host(struct sdhci_host *host);
extern void sdhci_remove_host(struct sdhci_host *host, int dead);
extern void sdhci_send_command(struct sdhci_host *host,
				struct mmc_command *cmd);

static inline bool sdhci_sdio_irq_enabled(struct sdhci_host *host)
{
	return !!(host->flags & SDHCI_SDIO_IRQ_ENABLED);
}

void sdhci_set_clock(struct sdhci_host *host, unsigned int clock);
void sdhci_set_power(struct sdhci_host *host, unsigned char mode,
		     unsigned short vdd);
void sdhci_set_bus_width(struct sdhci_host *host, int width);
void sdhci_reset(struct sdhci_host *host, u8 mask);
void sdhci_set_uhs_signaling(struct sdhci_host *host, unsigned timing);

#ifdef CONFIG_PM
extern int sdhci_suspend_host(struct sdhci_host *host);
extern int sdhci_resume_host(struct sdhci_host *host);
extern void sdhci_enable_irq_wakeups(struct sdhci_host *host);
extern int sdhci_runtime_suspend_host(struct sdhci_host *host);
extern int sdhci_runtime_resume_host(struct sdhci_host *host);
#endif

void sdhci_cfg_irq(struct sdhci_host *host, bool enable, bool sync);
#endif /* __SDHCI_HW_H */
