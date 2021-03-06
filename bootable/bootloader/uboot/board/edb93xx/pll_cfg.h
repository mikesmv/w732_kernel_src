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
 * PLL register values for Cirrus edb93xx boards
 *
 * Copyright (C) 2009 Matthias Kaehlcke <matthias@kaehlcke.net>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <asm/arch/ep93xx.h>

#if defined(CONFIG_EDB9301) || defined(CONFIG_EDB9302) ||	\
	defined(CONFIG_EDB9302A)
/*
 * fclk_div: 2, nbyp1: 1, hclk_div: 5, pclk_div: 2
 * pll1_x1: 294912000.000000, pll1_x2ip: 36864000.000000,
 * pll1_x2: 331776000.000000, pll1_out: 331776000.000000
 */
#define CLKSET1_VAL	(7 << SYSCON_CLKSET_PLL_X2IPD_SHIFT |	\
			8 << SYSCON_CLKSET_PLL_X2FBD2_SHIFT |	\
			19 << SYSCON_CLKSET_PLL_X1FBD1_SHIFT |	\
			1 << SYSCON_CLKSET1_PCLK_DIV_SHIFT |	\
			3 << SYSCON_CLKSET1_HCLK_DIV_SHIFT |	\
			SYSCON_CLKSET1_NBYP1 |			\
			1 << SYSCON_CLKSET1_FCLK_DIV_SHIFT)
#elif defined(CONFIG_EDB9307) || defined(CONFIG_EDB9307A) ||	\
	defined CONFIG_EDB9312 || defined(CONFIG_EDB9315) ||	\
	defined(CONFIG_EDB9315A)
/*
 * fclk_div: 2, nbyp1: 1, hclk_div: 4, pclk_div: 2
 * pll1_x1: 3096576000.000000, pll1_x2ip: 129024000.000000,
 * pll1_x2: 3999744000.000000, pll1_out: 1999872000.000000
 */
#define CLKSET1_VAL	(23 << SYSCON_CLKSET_PLL_X2IPD_SHIFT |	\
			30 << SYSCON_CLKSET_PLL_X2FBD2_SHIFT |	\
			20 << SYSCON_CLKSET_PLL_X1FBD1_SHIFT |	\
			1 << SYSCON_CLKSET1_PCLK_DIV_SHIFT |	\
			2 << SYSCON_CLKSET1_HCLK_DIV_SHIFT |	\
			SYSCON_CLKSET1_NBYP1 |			\
			1 << SYSCON_CLKSET1_FCLK_DIV_SHIFT)
#else
#error "Undefined board"
#endif

/*
 * usb_div: 4, nbyp2: 1, pll2_en: 1
 * pll2_x1: 368640000.000000, pll2_x2ip: 15360000.000000,
 * pll2_x2: 384000000.000000, pll2_out: 192000000.000000
 */
#define CLKSET2_VAL	(23 << SYSCON_CLKSET_PLL_X2IPD_SHIFT |	\
			24 << SYSCON_CLKSET_PLL_X2FBD2_SHIFT |	\
			24 << SYSCON_CLKSET_PLL_X1FBD1_SHIFT |	\
			1 << SYSCON_CLKSET_PLL_PS_SHIFT |	\
			SYSCON_CLKSET2_PLL2_EN |		\
			SYSCON_CLKSET2_NBYP2 |			\
			3 << SYSCON_CLKSET2_USB_DIV_SHIFT)
