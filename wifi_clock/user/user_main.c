/********************************************************************************
* author: Zwxf
*descripe: main app for wifi clcok app
*
*
***********************************************************************************/
#include "esp_common.h"
#include "uart.h"
#include "wifi_clock/config_param.h"

//static ap_up
//typedef struct sta_param{ 
#define SSID	"fjjxxy"
#define PSWD	"wtlwzwhxq"
	
	
//}TstaParam;

t_sdk_param sdkParam;


void ICACHE_FLASH_ATTR
scan_done_cb(void* param,STATUS st)
{
	if(st == OK){
		printf("\r\n =====scan cb done=======\r\n");
	}else{
		printf("\r\n =====scan cb failure=======\r\n");
		printf("\r\n param is %s",param);	
	}
}

bool ICACHE_FLASH_ATTR
is_equal_to_current_config(struct station_config* currentCfg,struct station_config* defaultCfg)
{
	return ((memcmp(currentCfg->ssid,defaultCfg->ssid,sizeof(defaultCfg->ssid)) == 0) && (memcmp(currentCfg->password,defaultCfg->password,sizeof(defaultCfg->password)) == 0));		
}

static ICACHE_FLASH_ATTR
STATION_STATUS station_up()
{
	bool isOk;
	struct station_config DefaultStaCfg,currentStaCfg;
	WIFI_MODE defaultWifiMode;

	memset(&DefaultStaCfg,0,sizeof(DefaultStaCfg));
	memset(&currentStaCfg,0,sizeof(currentStaCfg));

	strcpy(currentStaCfg.ssid,SSID);
	strcpy(currentStaCfg.password,PSWD);

	
	defaultWifiMode = wifi_get_opmode_default();
	if(defaultWifiMode & STATION_MODE){
		printf("\r\n=====default mode %d ",defaultWifiMode);
	}else{
		wifi_set_opmode(STATION_MODE);
	}
	
	isOk = wifi_station_get_config_default(&DefaultStaCfg);
	if(isOk == TRUE){
		printf("default ssid %s, default password %s",
				DefaultStaCfg.ssid,
				DefaultStaCfg.password
				);
		isOk = is_equal_to_current_config(&currentStaCfg,&DefaultStaCfg);
		if(isOk == TRUE){		
			if(!wifi_station_dhcpc_status()){
        		os_printf("DHCP is not started. Starting it...\n");
        		if(!wifi_station_dhcpc_start()){
            		os_printf("DHCP start failed!\n");
            		return false;
        		}
    		}
		}else{
			isOk = wifi_station_set_config(&currentStaCfg);
			if(isOk == FALSE){
				printf("current ssid %s, current password %s",
					currentStaCfg.ssid,
					currentStaCfg.password
					);
			}else{
				wifi_station_get_config_default(&DefaultStaCfg);
				printf("default ssid %s, default password %s",
					DefaultStaCfg.ssid,
					DefaultStaCfg.password
					);
			}
			if(!wifi_station_dhcpc_status()){
        		os_printf("DHCP is not started. Starting it...\n");
        		if(!wifi_station_dhcpc_start()){
            		os_printf("DHCP start failed!\n");
            		return false;
        		}
    		}
		}
		if(wifi_station_connect() < 0){
			printf("\r\n=====sta connect failure======\r\n");
		}
		
	}else{
		printf("\r\n=============get default config failure=============\r\n");
	}
	return TRUE;
}

/*
static char ap_up()
{


}
*/
void ICACHE_FLASH_ATTR
wifi_event_handler(System_Event_t* evt)
{
	if(evt == NULL){
		return;
	}
	switch (evt->event_id){
		case EVENT_STAMODE_SCAN_DONE:
			printf("\r\n=====scan done====\r\n");
			
			Event_StaMode_ScanDone_t* info = (Event_StaMode_ScanDone_t* )&evt->event_info;
			struct bss_info* bss = info->bss;
			if(info->status != FALSE){
				while(bss != NULL){
					printf("ssid %s ,ssid len %d ,channel %d, rssi %d,",
						bss->ssid,
						bss->ssid_len,
						bss->channel,
						bss->rssi
						);
					//bss = bss->next;
				}
			}else{
				printf("\r\n=====scan failure=======\r\n");
			}
			
			break;
		case EVENT_STAMODE_CONNECTED:
			printf("\r\n=====connect======\r\n");
			break;
		case EVENT_STAMODE_DISCONNECTED:
			printf("\r\n=====disconnect======\r\n");
			break;
		case EVENT_STAMODE_GOT_IP:
			printf("\r\n=====got ip already====\r\n");
			break;
		case EVENT_STAMODE_AUTHMODE_CHANGE:
			printf("\r\n=====auto change======\r\n");
			break;
		case EVENT_STAMODE_DHCP_TIMEOUT:
			printf("\r\n dhcp timeout \r\n");
			break;
		case EVENT_SOFTAPMODE_STACONNECTED:
			printf("\r\n=====sta connect======\r\n");	
			break;
		case EVENT_SOFTAPMODE_STADISCONNECTED:
			printf("\r\n=====sta disconnect======\r\n");
			break;
		case EVENT_SOFTAPMODE_PROBEREQRECVED:
			printf("\r\n=====soft connect======\r\n");
			break;
		default:
			break;
	};
}

void ICACHE_FLASH_ATTR
user_config_load()
{
/*
	if(config_param_flash((uint32* )&defaultSdkParam,sizeof(t_test_param)) != 0){
		printf("\r\n load flash failure \r\n");
	}
*/
	if(config_param_load((uint32* )&sdkParam,sizeof(sdkParam)) < 0){
		printf("\r\n load param failure \r\n");
	}	
	printf("sdk param name %s,value %d ",
			sdkParam.test_param.name,
			sdkParam.test_param.value
			);
	//dump_config_param();
}



/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;
        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}
/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
extern void httpd_task(void *pvParameters);
void user_init(void)
{
	UART_SetBaudrate(UART0,115200);	
	printf("SDK version:%s free size %d\n", system_get_sdk_version(),system_get_free_heap_size());
	station_up();
	user_config_load();
	wifi_set_event_handler_cb(wifi_event_handler);
	xTaskCreate(&httpd_task, "HTTP Daemon", 128, NULL, 2, NULL);
}

