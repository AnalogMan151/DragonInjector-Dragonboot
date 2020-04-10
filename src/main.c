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

#define CENTERED_TEXT_START(n) (((1280 - n * CHAR_WIDTH * g_gfx_con.scale) / 2))

extern void pivot_stack(u32 stack_top);
extern u8 get_payload_num(void);

bool g_render_embedded_splash = true;

static inline void setup_gfx()
{
    u32 *fb = display_init_framebuffer();
    gfx_init_ctxt(&g_gfx_ctxt, fb, 1280, 720, 720);
    gfx_clear_color(&g_gfx_ctxt, 0xFF000000);
    gfx_con_init(&g_gfx_con, &g_gfx_ctxt);
    g_gfx_con.scale = 1;
}

void display_logo_with_message(int count, ...)
{
    /* vardiac arg list for multiple strings. */
    va_list strs;
    va_start(strs, count);

    /* save the scale factor so we can restore it later. */
    u8 scale_old = g_gfx_con.scale;
    
    if(g_render_embedded_splash)
    {
        /* set scale and position for logo */
        g_gfx_con.scale = 3;
        g_gfx_con.x = 370;
        g_gfx_con.y = 98;

        /* print logo.                                     */
        /* Logo chars are stored as part of the font,      */
        /* outside of normal ascii range.                  */
        /* The logo is comprised of 4 rows and 12 columns. */
        for(char i = 0; i < 4; i++)
        {
            for(char j = 0; j < 12; j++)
            {
                gfx_putc(&g_gfx_con, (i * 12) + j + 0x7F);
            }
            gfx_putc(&g_gfx_con, '\n');
            g_gfx_con.x = 370;
        }
    }

    /* set scale and y position for text. */
    g_gfx_con.scale = 1;
    g_gfx_con.y = 593;

    /* Allocate space for pointers to each string,                   */
    /* along with var to hold total number of chars to print         */
    /* and starting index to keep track of already processed strings */
    char **str_ptrs = malloc(count * sizeof(char *));
    int start = 0;
    int total_len = 0;
    for(int i = 0; i <= count; i++)
    {
        /* Get next string from va list */
        char *str = va_arg(strs, char *);
        str_ptrs[i] = str; 

        /* Handle newlines separately, so text centering isn't thrown off, */
        /* and print characters that haven't been printed yet.             */
        if(str[0] == '\n' || i == count)
        {
            /* Set text position. Center text horizontally. */
            g_gfx_con.x = CENTERED_TEXT_START(total_len);

            /* Print each string, starting at calculated x position. */
            for(int j = start; j < i; j++)
                gfx_puts(&g_gfx_con, str_ptrs[j]);

            /* Next string to worry about is after current string.        */
            /* Reset number of characters to print to zero for next line. */
            start = i + 1;
            total_len = 0;

            gfx_putc(&g_gfx_con, '\n');
            continue;
        }

        total_len += strlen(str);
    }
    /* Free list of pointers. */
    free(str_ptrs);
    str_ptrs = 0;

    /* Restore the scale factor. */
    g_gfx_con.scale = scale_old;
}

void find_and_launch_payload(const char *folder)
{
    DIR dir;
    FILINFO finfo;
    FRESULT res = f_findfirst(&dir, &finfo, folder, "*.bin");
    if(res == FR_OK && (strlen(finfo.fname) != 0))
    {
        size_t path_size = strlen(finfo.fname) + strlen(folder) + 2;
        char *payload_path = malloc(path_size);
        if(payload_path != NULL)
        {
            memset(payload_path, 0, path_size);
            strcpy(payload_path, folder);
            strcat(payload_path, "/");
            strcat(payload_path, finfo.fname);
            if(g_render_embedded_splash)
                display_logo_with_message(2, "Launching ", payload_path);
            msleep(5000);
            launch_payload(payload_path);
        }
    }
    else if((strlen(finfo.fname) == 0) && (res != FR_NO_PATH))
    {
        g_gfx_con.fgcol = 0xFFFF0000;
        display_logo_with_message(5, "No payload found in ", folder, "!", "\n", "Press any button to reboot into RCM.");
    }
    else if(res == FR_NO_PATH)
    {
        g_gfx_con.fgcol = 0xFFFF0000;
        display_logo_with_message(7, "Folder not found!", "\n", "Please create ", folder, " on your SD card and add your payload to it.", "\n", "Press any button to reboot into RCM.");
    }
}

void ipl_main()
{
    config_hw();

    /* Init the stack and the heap */
    pivot_stack(0x90010000);
    heap_init(0x90020000);

    /* Init display and gfx module */
    display_init();
    setup_gfx();
    display_backlight_pwm_init();
    display_backlight_brightness(100, 1000);

    u8 payload_num = get_payload_num() + 1;

    if(sd_mount())
    {
        if(sd_file_read("dragonboot/splash.raw", g_gfx_ctxt.next))
        {
            g_render_embedded_splash = false;
            gfx_swap_buffer(&g_gfx_ctxt);
        }

        if(payload_num == 0)
            find_and_launch_payload("dragonboot");

        char folder[] = "dragonboot/\0\0\0";

        const char *num_table[] = { "00", "01", "02", "03", "04", "05", "06", "07", "08" };
        strcat(folder, num_table[payload_num]);

        find_and_launch_payload(folder);
    }
    else
    {
        g_gfx_con.fgcol = 0xFFFF0000;
        display_logo_with_message(3, "SD card not inserted!", "\n", "Press any button to reboot into RCM.");
    }

    btn_wait();
    PMC(APBDEV_PMC_SCRATCH0) |= 2;
    PMC(APBDEV_PMC_CNTRL) = PMC_CNTRL_MAIN_RST;
    while(1);
}