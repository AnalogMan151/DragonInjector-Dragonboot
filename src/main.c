/*
 * Copyright (c) 2018 Guillem96
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdarg.h>
#include <string.h>

#include "gfx/di.h"
#include "gfx/gfx.h"

#include "mem/heap.h"

#include "soc/hw_init.h"
#include "soc/t210.h"
#include "soc/pmc.h"

#include "core/launcher.h"

#include "utils/util.h"
#include "utils/fs_utils.h"
#include "utils/btn.h"

sdmmc_t g_sd_sdmmc;
sdmmc_storage_t g_sd_storage;
FATFS g_sd_fs;
bool g_sd_mounted;
gfx_ctxt_t g_gfx_ctxt;
gfx_con_t g_gfx_con;

extern void pivot_stack(u32 stack_top);

void ipl_main()
{
    config_hw();

    /* Init the stack and the heap */
    pivot_stack(0x90010000);
    heap_init(0x90020000);

    /* Init display and gfx module */
    display_init();
    display_backlight_pwm_init();
    display_backlight_brightness(100, 1000);

    if(sd_mount())
    {
        msleep(1000);
        launch_payload("atmosphere/reboot_payload.bin");
    }

    btn_wait();
    PMC(APBDEV_PMC_SCRATCH0) |= 2;
    PMC(APBDEV_PMC_CNTRL) = PMC_CNTRL_MAIN_RST;
    while(1);
}