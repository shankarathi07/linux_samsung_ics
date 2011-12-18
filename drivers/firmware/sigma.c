/*
 * Load Analog Devices SigmaStudio firmware files
 *
 * Copyright 2009-2011 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/crc32.h>
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/sigma.h>

<<<<<<< HEAD
static size_t sigma_action_size(struct sigma_action *sa)
{
	size_t payload = 0;

	switch (sa->instr) {
	case SIGMA_ACTION_WRITEXBYTES:
	case SIGMA_ACTION_WRITESINGLE:
	case SIGMA_ACTION_WRITESAFELOAD:
		payload = sigma_action_len(sa);
		break;
	default:
		break;
	}

	payload = ALIGN(payload, 2);

	return payload + sizeof(struct sigma_action);
}

/*
 * Returns a negative error value in case of an error, 0 if processing of
 * the firmware should be stopped after this action, 1 otherwise.
 */
static int
process_sigma_action(struct i2c_client *client, struct sigma_action *sa)
{
	size_t len = sigma_action_len(sa);
	int ret;
=======
/* Return: 0==OK, <0==error, =1 ==no more actions */
static int
process_sigma_action(struct i2c_client *client, struct sigma_firmware *ssfw)
{
	struct sigma_action *sa = (void *)(ssfw->fw->data + ssfw->pos);
	size_t len = sigma_action_len(sa);
	int ret = 0;
>>>>>>> 2f57f5b... Merge branch 'androidsource' android-samsung-3.0-ics-mr1 into nexus-s-voodoo

	pr_debug("%s: instr:%i addr:%#x len:%zu\n", __func__,
		sa->instr, sa->addr, len);

	switch (sa->instr) {
	case SIGMA_ACTION_WRITEXBYTES:
	case SIGMA_ACTION_WRITESINGLE:
	case SIGMA_ACTION_WRITESAFELOAD:
<<<<<<< HEAD
=======
		if (ssfw->fw->size < ssfw->pos + len)
			return -EINVAL;
>>>>>>> 2f57f5b... Merge branch 'androidsource' android-samsung-3.0-ics-mr1 into nexus-s-voodoo
		ret = i2c_master_send(client, (void *)&sa->addr, len);
		if (ret < 0)
			return -EINVAL;
		break;
<<<<<<< HEAD
	case SIGMA_ACTION_DELAY:
		udelay(len);
		len = 0;
		break;
	case SIGMA_ACTION_END:
		return 0;
=======

	case SIGMA_ACTION_DELAY:
		ret = 0;
		udelay(len);
		len = 0;
		break;

	case SIGMA_ACTION_END:
		return 1;

>>>>>>> 2f57f5b... Merge branch 'androidsource' android-samsung-3.0-ics-mr1 into nexus-s-voodoo
	default:
		return -EINVAL;
	}

<<<<<<< HEAD
	return 1;
=======
	/* when arrive here ret=0 or sent data */
	ssfw->pos += sigma_action_size(sa, len);
	return ssfw->pos == ssfw->fw->size;
>>>>>>> 2f57f5b... Merge branch 'androidsource' android-samsung-3.0-ics-mr1 into nexus-s-voodoo
}

static int
process_sigma_actions(struct i2c_client *client, struct sigma_firmware *ssfw)
{
<<<<<<< HEAD
	struct sigma_action *sa;
	size_t size;
	int ret;

	while (ssfw->pos + sizeof(*sa) <= ssfw->fw->size) {
		sa = (struct sigma_action *)(ssfw->fw->data + ssfw->pos);

		size = sigma_action_size(sa);
		ssfw->pos += size;
		if (ssfw->pos > ssfw->fw->size || size == 0)
			break;

		ret = process_sigma_action(client, sa);

		pr_debug("%s: action returned %i\n", __func__, ret);

		if (ret <= 0)
			return ret;
	}

	if (ssfw->pos != ssfw->fw->size)
		return -EINVAL;

	return 0;
=======
	pr_debug("%s: processing %p\n", __func__, ssfw);

	while (1) {
		int ret = process_sigma_action(client, ssfw);
		pr_debug("%s: action returned %i\n", __func__, ret);
		if (ret == 1)
			return 0;
		else if (ret)
			return ret;
	}
>>>>>>> 2f57f5b... Merge branch 'androidsource' android-samsung-3.0-ics-mr1 into nexus-s-voodoo
}

int process_sigma_firmware(struct i2c_client *client, const char *name)
{
	int ret;
	struct sigma_firmware_header *ssfw_head;
	struct sigma_firmware ssfw;
	const struct firmware *fw;
	u32 crc;

	pr_debug("%s: loading firmware %s\n", __func__, name);

	/* first load the blob */
	ret = request_firmware(&fw, name, &client->dev);
	if (ret) {
		pr_debug("%s: request_firmware() failed with %i\n", __func__, ret);
		return ret;
	}
	ssfw.fw = fw;

	/* then verify the header */
	ret = -EINVAL;
<<<<<<< HEAD

	/*
	 * Reject too small or unreasonable large files. The upper limit has been
	 * chosen a bit arbitrarily, but it should be enough for all practical
	 * purposes and having the limit makes it easier to avoid integer
	 * overflows later in the loading process.
	 */
	if (fw->size < sizeof(*ssfw_head) || fw->size >= 0x4000000)
=======
	if (fw->size < sizeof(*ssfw_head))
>>>>>>> 2f57f5b... Merge branch 'androidsource' android-samsung-3.0-ics-mr1 into nexus-s-voodoo
		goto done;

	ssfw_head = (void *)fw->data;
	if (memcmp(ssfw_head->magic, SIGMA_MAGIC, ARRAY_SIZE(ssfw_head->magic)))
		goto done;

<<<<<<< HEAD
	crc = crc32(0, fw->data + sizeof(*ssfw_head),
			fw->size - sizeof(*ssfw_head));
	pr_debug("%s: crc=%x\n", __func__, crc);
	if (crc != le32_to_cpu(ssfw_head->crc))
=======
	crc = crc32(0, fw->data, fw->size);
	pr_debug("%s: crc=%x\n", __func__, crc);
	if (crc != ssfw_head->crc)
>>>>>>> 2f57f5b... Merge branch 'androidsource' android-samsung-3.0-ics-mr1 into nexus-s-voodoo
		goto done;

	ssfw.pos = sizeof(*ssfw_head);

	/* finally process all of the actions */
	ret = process_sigma_actions(client, &ssfw);

 done:
	release_firmware(fw);

	pr_debug("%s: loaded %s\n", __func__, name);

	return ret;
}
EXPORT_SYMBOL(process_sigma_firmware);

MODULE_LICENSE("GPL");
