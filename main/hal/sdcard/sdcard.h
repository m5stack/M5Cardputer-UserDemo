/**
 * @file sdcard.h
 * @author Anderson Antunes
 * @brief 
 * @version 0.1
 * @date 2024-01-14
 *
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef SD_CARD_MANAGER_H
#define SD_CARD_MANAGER_H

#include "driver/sdmmc_host.h"

class SDCard
{
	public:
		bool mount(bool format_if_mount_failed);
		bool eject();
		bool is_mounted();
		char *get_mount_point();
		char *get_filepath(const char *path);
		bool file_exists(const char *path);
    private:
        sdmmc_card_t* card = nullptr;
		bool mounted = false;
};

#endif // SD_CARD_MANAGER_H
