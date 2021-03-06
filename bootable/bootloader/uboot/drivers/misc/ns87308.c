/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*
 * (C) Copyright 2000
 * Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>

#include <ns87308.h>

void initialise_ns87308 (void)
{
#ifdef CONFIG_SYS_NS87308_PS2MOD
	unsigned char data;

	/*
	 * Switch floppy drive to PS/2 mode.
	 */
	read_pnp_config(SUPOERIO_CONF1, &data);
	data &= 0xFB;
	write_pnp_config(SUPOERIO_CONF1, data);
#endif

#if (CONFIG_SYS_NS87308_DEVS & CONFIG_SYS_NS87308_KBC1)
	PNP_SET_DEVICE_BASE(LDEV_KBC1, CONFIG_SYS_NS87308_KBC1_BASE);
	write_pnp_config(LUN_CONFIG_REG, 0);
	write_pnp_config(CBASE_HIGH, 0x00);
	write_pnp_config(CBASE_LOW, 0x64);
#endif

#if (CONFIG_SYS_NS87308_DEVS & CONFIG_SYS_NS87308_MOUSE)
	PNP_ACTIVATE_DEVICE(LDEV_MOUSE);
#endif

#if (CONFIG_SYS_NS87308_DEVS & CONFIG_SYS_NS87308_RTC_APC)
	PNP_SET_DEVICE_BASE(LDEV_RTC_APC, CONFIG_SYS_NS87308_RTC_BASE);
#endif

#if (CONFIG_SYS_NS87308_DEVS & CONFIG_SYS_NS87308_FDC)
	PNP_SET_DEVICE_BASE(LDEV_FDC, CONFIG_SYS_NS87308_FDC_BASE);
	write_pnp_config(LUN_CONFIG_REG, 0x40);
#endif

#if (CONFIG_SYS_NS87308_DEVS & CONFIG_SYS_NS87308_RARP)
	PNP_SET_DEVICE_BASE(LDEV_PARP, CONFIG_SYS_NS87308_LPT_BASE);
#endif

#if (CONFIG_SYS_NS87308_DEVS & CONFIG_SYS_NS87308_UART1)
	PNP_SET_DEVICE_BASE(LDEV_UART1, CONFIG_SYS_NS87308_UART1_BASE);
#endif

#if (CONFIG_SYS_NS87308_DEVS & CONFIG_SYS_NS87308_UART2)
	PNP_SET_DEVICE_BASE(LDEV_UART2, CONFIG_SYS_NS87308_UART2_BASE);
#endif

#if (CONFIG_SYS_NS87308_DEVS & CONFIG_SYS_NS87308_GPIO)
	PNP_SET_DEVICE_BASE(LDEV_GPIO, CONFIG_SYS_NS87308_GPIO_BASE);
#endif

#if (CONFIG_SYS_NS87308_DEVS & CONFIG_SYS_NS87308_POWRMAN)
#ifndef CONFIG_SYS_NS87308_PWMAN_BASE
	PNP_ACTIVATE_DEVICE(LDEV_POWRMAN);
#else
	PNP_SET_DEVICE_BASE(LDEV_POWRMAN, CONFIG_SYS_NS87308_PWMAN_BASE);

	/*
	 * Enable all units
	 */
	write_pm_reg(CONFIG_SYS_NS87308_PWMAN_BASE, PWM_FER1, 0x7d);
	write_pm_reg(CONFIG_SYS_NS87308_PWMAN_BASE, PWM_FER2, 0x87);

#ifdef CONFIG_SYS_NS87308_PMC1
	write_pm_reg(CONFIG_SYS_NS87308_PWMAN_BASE, PWM_PMC1, CONFIG_SYS_NS87308_PMC1);
#endif

#ifdef CONFIG_SYS_NS87308_PMC2
	write_pm_reg(CONFIG_SYS_NS87308_PWMAN_BASE, PWM_PMC2, CONFIG_SYS_NS87308_PMC2);
#endif

#ifdef CONFIG_SYS_NS87308_PMC3
	write_pm_reg(CONFIG_SYS_NS87308_PWMAN_BASE, PWM_PMC3, CONFIG_SYS_NS87308_PMC3);
#endif
#endif
#endif

#ifdef CONFIG_SYS_NS87308_CS0_BASE
	PNP_PGCS_CSLINE_BASE(0, CONFIG_SYS_NS87308_CS0_BASE);
	PNP_PGCS_CSLINE_CONF(0, CONFIG_SYS_NS87308_CS0_CONF);
#endif

#ifdef CONFIG_SYS_NS87308_CS1_BASE
	PNP_PGCS_CSLINE_BASE(1, CONFIG_SYS_NS87308_CS1_BASE);
	PNP_PGCS_CSLINE_CONF(1, CONFIG_SYS_NS87308_CS1_CONF);
#endif

#ifdef CONFIG_SYS_NS87308_CS2_BASE
	PNP_PGCS_CSLINE_BASE(2, CONFIG_SYS_NS87308_CS2_BASE);
	PNP_PGCS_CSLINE_CONF(2, CONFIG_SYS_NS87308_CS2_CONF);
#endif
}
