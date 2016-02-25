#ifndef ROM_WLAN_RAM_MAP_H
#define ROM_WLAN_RAM_MAP_H

struct _rom_wlan_ram_map {
	unsigned char * (*rtw_malloc)(unsigned int sz);
	void (*rtw_mfree)(unsigned char *pbuf, unsigned int sz);
};

#endif	/* ROM_WLAN_RAM_MAP_H */
