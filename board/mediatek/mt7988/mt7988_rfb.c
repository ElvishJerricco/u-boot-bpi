// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2022 MediaTek Inc.
 * Author: Sam Shih <sam.shih@mediatek.com>
 */

#include <mapmem.h>
#include <hash.h>
#include <u-boot/sha1.h>
#include <net.h>

int board_init(void)
{
	return 0;
}

int board_late_init(void)
{
	void *buf;
	uint8_t cpuid_digest[20];
	u8 cpu_uid[16];
	u8 ethaddr0[6];
	u8 ethaddr1[6];
	u8 ethaddr2[6];

	/*
	 * 	No documentation, but it is said to be a burned in unique ID of the CPU.
	 * 	https://forum.banana-pi.org/t/bpi-r4-serial-number/18711
	 * 	Reported to be unique and static by forum members.
	 */
	buf = map_sysmem(0x11F50140, 16);
	memcpy(cpu_uid, buf, 16);
	unmap_sysmem(buf);

	sha1_csum(cpu_uid, 16, cpuid_digest);

	memcpy(ethaddr0, cpuid_digest, 6);
	memcpy(ethaddr1, cpuid_digest, 6);
	memcpy(ethaddr2, cpuid_digest, 6);

	/*
	 * 	MAC adresses with a "2" for the second hex digit are locally
	 * 	administered and will not clash with devices assigned MACs from the
	 * 	IEEE.
	 * 	https://en.wikipedia.org/wiki/MAC_address#IEEE_802c_local_MAC_address_usage
	 */
	ethaddr0[0] &= 0xf0;
	ethaddr0[0] |= 0x02;

	ethaddr1[0] &= 0xf0;
	ethaddr1[0] |= 0x02;
	ethaddr1[5]++;

	ethaddr2[0] &= 0xf0;
	ethaddr2[0] |= 0x02;
	ethaddr2[5] += 2;

	if (is_valid_ethaddr(ethaddr0)) {
		eth_env_set_enetaddr_by_index("eth", 0, ethaddr0);
	} else {
		puts("Generated ethaddr0 was invalid\n");
	}
	if (is_valid_ethaddr(ethaddr1)) {
		eth_env_set_enetaddr_by_index("eth", 1, ethaddr1);
	} else {
		puts("Generated ethaddr1 was invalid\n");
	}
	if (is_valid_ethaddr(ethaddr2)) {
		eth_env_set_enetaddr_by_index("eth", 2, ethaddr2);
	} else {
		puts("Generated ethaddr2 was invalid\n");
	}

	return 0;
}
