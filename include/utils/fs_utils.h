#ifndef _FS_UTILS_H_
#define _FS_UTILS_H_

#include "utils/types.h"
#include "libs/fatfs/ff.h"
#include "storage/sdmmc.h"
#include "storage/sdmmc_driver.h"

extern sdmmc_t g_sd_sdmmc;
extern sdmmc_storage_t g_sd_storage;
extern FATFS g_sd_fs;
extern bool g_sd_mounted;

bool sd_mount();
void sd_unmount();
void *sd_file_read(char *path, void *ext_buf);
int sd_save_to_file(void *buf, u32 size, const char *filename);
bool sd_file_exists(const char* filename);
void flipVertically(unsigned char* pixels_buffer, const unsigned int width, const unsigned int height, const int bytes_per_pixel);
#endif