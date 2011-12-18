/*
 * Load firmware files from Analog Devices SigmaStudio
 *
 * Copyright 2009-2011 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef __SIGMA_FIRMWARE_H__
#define __SIGMA_FIRMWARE_H__

#include <linux/firmware.h>
#include <linux/types.h>

struct i2c_client;

#define SIGMA_MAGIC "ADISIGM"

struct sigma_firmware {
	const struct firmware *fw;
	size_t pos;
};

struct sigma_firmware_header {
	unsigned char magic[7];
	u8 version;
<<<<<<< HEAD
	__le32 crc;
=======
	u32 crc;
>>>>>>> 2f57f5b... Merge branch 'androidsource' android-samsung-3.0-ics-mr1 into nexus-s-voodoo
};

enum {
	SIGMA_ACTION_WRITEXBYTES = 0,
	SIGMA_ACTION_WRITESINGLE,
	SIGMA_ACTION_WRITESAFELOAD,
	SIGMA_ACTION_DELAY,
	SIGMA_ACTION_PLLWAIT,
	SIGMA_ACTION_NOOP,
	SIGMA_ACTION_END,
};

struct sigma_action {
	u8 instr;
	u8 len_hi;
<<<<<<< HEAD
	__le16 len;
	__be16 addr;
=======
	u16 len;
	u16 addr;
>>>>>>> 2f57f5b... Merge branch 'androidsource' android-samsung-3.0-ics-mr1 into nexus-s-voodoo
	unsigned char payload[];
};

static inline u32 sigma_action_len(struct sigma_action *sa)
{
<<<<<<< HEAD
	return (sa->len_hi << 16) | le16_to_cpu(sa->len);
=======
	return (sa->len_hi << 16) | sa->len;
}

static inline size_t sigma_action_size(struct sigma_action *sa, u32 payload_len)
{
	return sizeof(*sa) + payload_len + (payload_len % 2);
>>>>>>> 2f57f5b... Merge branch 'androidsource' android-samsung-3.0-ics-mr1 into nexus-s-voodoo
}

extern int process_sigma_firmware(struct i2c_client *client, const char *name);

#endif
