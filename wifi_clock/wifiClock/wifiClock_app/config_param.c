



#include <wifi_clock/config_param.h>

#define FLASH_UNIT_SIZE		4
#define ALIGNE(size)	(((size + (FLASH_UNIT_SIZE - 1)) & -FLASH_UNIT_SIZE) + FLASH_UNIT_SIZE)



SpiFlashOpResult ICACHE_FLASH_ATTR 
config_param_flash(uint32* src,uint32 size)
{
	SpiFlashOpResult ret = ~SPI_FLASH_RESULT_OK;
	u32_t alignedSize = 0;
	char eraseSectorNum  = 0,retry = 0;
	for(eraseSectorNum = CONFIG_PARAM_SECTOR_START;eraseSectorNum <= CONFIG_PARAM_SECTOR_END;eraseSectorNum++)
	{		
		while(ret != SPI_FLASH_RESULT_OK){
			if(retry++ > 3){
				retry = 0;
				break;
			}
			printf("\r\n flash erase retry is %d\r\n",retry);
			ret = spi_flash_erase_sector(eraseSectorNum);			
		}
	}
	alignedSize = ALIGNE(size);
	printf("aligned size is %d\r\n",alignedSize);	

	retry = 0;
	ret = ~SPI_FLASH_RESULT_OK;

	while(ret != SPI_FLASH_RESULT_OK){
			if(retry++ > 3){
				retry = 0;
				break;
			}
			printf("\r\n flash write retry is %d",retry);
		ret = spi_flash_write(CONFIG_PARAM_ADDR_START,src,alignedSize);
	}

	return ret;
}

SpiFlashOpResult ICACHE_FLASH_ATTR
config_param_load(uint32* dst,int size)
{
	char retry = 0;
	SpiFlashOpResult ret = ~SPI_FLASH_RESULT_OK;
	u32_t alignedSize = 0;
	alignedSize = ALIGNE(size);
	
	while(ret != SPI_FLASH_RESULT_OK){
		if(retry++ > 3){
			retry = 0;
			break;
		}
		ret = spi_flash_read(CONFIG_PARAM_ADDR_START,dst,alignedSize);
	}

	return ret;
}

void ICACHE_FLASH_ATTR
dump_config_param()
{
	char retry = 0;
	SpiFlashOpResult ret = ~SPI_FLASH_RESULT_OK;
	t_sdk_param sdkParam;
	u32_t alignedSize = 0;
	alignedSize = ALIGNE(sizeof(t_sdk_param));
	
	while(ret != SPI_FLASH_RESULT_OK){
		if(retry++ > 3){
			retry = 0;
			break;
		}
		ret = spi_flash_read(CONFIG_PARAM_ADDR_START,(uint32* )&sdkParam,alignedSize);
	}

	printf("name %s, value %d \r\n",
			sdkParam.test_param.name,
			sdkParam.test_param.value
			);

	
}
void ICACHE_FLASH_ATTR
dump_default_sdk_param()
{
	printf("default name %s, default value %d \r\n",
		defaultSdkParam.test_param.name,
		defaultSdkParam.test_param.value
	);	
}

