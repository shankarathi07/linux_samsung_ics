/*
 * arch/arm/mach-s5pv210/cpuidle.c
 *
 * Copyright (c) Samsung Electronics Co. Ltd
 *
 * CPU idle driver for S5PV210
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/cpuidle.h>
#include <linux/io.h>
#include <asm/proc-fns.h>
#include <asm/cacheflush.h>

#include <mach/map.h>
#include <mach/regs-irq.h>
#include <mach/regs-clock.h>
#include <plat/pm.h>
#include <plat/devs.h>

#include <mach/dma.h>
#include <mach/regs-gpio.h>

#define S5PC110_MAX_STATES	1

static int s5p_enter_idle_normal(struct cpuidle_device *dev,
			struct cpuidle_driver *drv,
			      int index);

static struct cpuidle_state sp5_cpuidle_set[] = {
	[0] = {
		.enter			= s5p_enter_idle_normal,
		.exit_latency		= 1,
		.target_residency	= 100000,
		.flags			= CPUIDLE_FLAG_TIME_VALID,
		.name			= "IDLE",
		.desc			= "ARM clock gating(WFI)",
	},
};

static void s5p_enter_idle(void)
{
	unsigned long tmp;

	tmp = __raw_readl(S5P_IDLE_CFG);
	tmp &= ~((3<<30)|(3<<28)|(1<<0));
	tmp |= ((2<<30)|(2<<28));
	__raw_writel(tmp, S5P_IDLE_CFG);

	tmp = __raw_readl(S5P_PWR_CFG);
	tmp &= S5P_CFG_WFI_CLEAN;
	__raw_writel(tmp, S5P_PWR_CFG);

	cpu_do_idle();
}

/* Actual code that puts the SoC in different idle states */
static int s5p_enter_idle_normal(struct cpuidle_device *dev,
				struct cpuidle_driver *drv,
			      int index)
{
	struct timeval before, after;
	int idle_time;

	local_irq_disable();
	do_gettimeofday(&before);

	s5p_enter_idle();

	do_gettimeofday(&after);
	local_irq_enable();
	idle_time = (after.tv_sec - before.tv_sec) * USEC_PER_SEC +
			(after.tv_usec - before.tv_usec);

	dev->last_residency = idle_time;
	return index;
}

static DEFINE_PER_CPU(struct cpuidle_device, s5p_cpuidle_device);

static struct cpuidle_driver s5p_idle_driver = {
	.name =         "s5p_idle",
	.owner =        THIS_MODULE,
};

/* Initialize CPU idle by registering the idle states */
static int s5p_init_cpuidle(void)
{
	int i, max_cpuidle_state, cpu_id;
	struct cpuidle_device *device;
	struct cpuidle_driver *drv = &s5p_idle_driver;

	/* Setup cpuidle driver */
	drv->state_count = (sizeof(sp5_cpuidle_set) /
				       sizeof(struct cpuidle_state));
	max_cpuidle_state = drv->state_count;
	for (i = 0; i < max_cpuidle_state; i++) {
		memcpy(&drv->states[i], &sp5_cpuidle_set[i],
				sizeof(struct cpuidle_state));
	}
	cpuidle_register_driver(&s5p_idle_driver);

	for_each_cpu(cpu_id, cpu_online_mask) {
		device = &per_cpu(s5p_cpuidle_device, cpu_id);
		device->cpu = cpu_id;

		device->state_count = drv->state_count;

		if (cpuidle_register_device(device)) {
			printk(KERN_ERR "s5p_init_cpuidle: Failed registering\n");
			return -EIO;
		}
	}

	return 0;
}

device_initcall(s5p_init_cpuidle);
