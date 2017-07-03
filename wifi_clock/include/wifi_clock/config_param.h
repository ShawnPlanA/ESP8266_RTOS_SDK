#ifndef CONFIG_PARAM_H
#define CONFIG_PARAM_H
#include <esp_common.h>


/* 1024 + 1024 MAP */
/* end before 1024KB address 32KB   */
#define CONFIG_PARAM_ADDR_START		0xf7000	
#define CONFIG_PARAM_ADDR_END		0xfb000	
#define CONFIG_PARAM_SECTOR_START		247
#define CONFIG_PARAM_SECTOR_END			250
/* 16KB */
#define CONFIG_PARAM_LEN	0x4000

typedef struct test_param{
	char name[20];
	int value;
}t_test_param;

typedef struct sdk_param{
	t_test_param test_param;
}t_sdk_param;

extern t_sdk_param defaultSdkParam;

SpiFlashOpResult ICACHE_FLASH_ATTR config_param_flash(uint32* src,uint32 size);
SpiFlashOpResult ICACHE_FLASH_ATTR config_param_load(uint32* dst,int size);
void ICACHE_FLASH_ATTR dump_config_param();
void ICACHE_FLASH_ATTR dump_default_sdk_param();

#endif
