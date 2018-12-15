/*
 * File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015-07-29     Arda.Fu      first implementation
 */
#include <rtthread.h>
#include <board.h>

/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#include <dfs_posix.h>

#include <easyflash.h>
#include "drivers/usb_device.h"
#include "drv_usbd.h"


#define USER_APP_ADDRESS		0x08020000

void iap_load_app(uint32_t appxaddr);
static uint16_t LocateString(const char * src, const char * str, uint16_t src_len);
void bootloader(void);

int main(void)
{
	bootloader();
}


int old_bootloader(void)
{
    /* user app entry */
	int new_v = 0;
	int old_v = 0;
	char *old_verison;
	char new_version[16] = {0};
	char bin_file_name[32] = {0};
	int new_bin_file_size = 0;
	int file_v = 0;
	int file_bin = 0;
	uint32_t user_app_addr = 0;
	uint32_t cur_size;
	uint32_t app_cur_addr;
	int result = 0;
	uint32_t buff[32];
		
		rt_pin_mode(BOOTLOADER_CHECK_KEY, PIN_MODE_INPUT_PULLUP);
	//	while();
		easyflash_init();
	
		if (rt_pin_read(BOOTLOADER_CHECK_KEY) == 0) {	//�������¹̼�����bootloader�еȴ���������
			goto to_open_usb;
		}

		/* mount sd card fat partition 1 as root directory */  
		if (dfs_mount("flash0", "/", "elm", 0, 0) == 0) {
			rt_kprintf("flash0 mount to /.\n");  
		} else {
			return 0;
		}

		//1�����spi flash�̼��汾�ţ��ȶԵ�ǰ�̼��汾��
		//2������Ҫ�������ҵ���Ӧ�汾�Ź̼�����ȡ�̼���С
		//3������flash�����¹̼�
		//4�����¹̼��汾��Ϊ����
		//5��������APP
		
		//1
		file_v = open("version.txt", O_RDONLY, 0);
		if (file_v == -1) {
			rt_kprintf("Error:version.txt not found!");
			goto to_application;	//to to_open_usb;	//����ת��app����bootloader�еȴ�����ָ����û����µ���version.txt�ļ�
		}
		if (-1 == read(file_v, new_version, 16)) {
			rt_kprintf("Error:version.txt not found!");
			goto to_application;
		}
		//old_verison = ef_get_env("version");
		new_v = atol(new_version);
		old_v = atol(old_verison);
		
		if (new_v <= old_v) {
			goto to_application;
		}
		//2
		
		sprintf(bin_file_name, "%s.bin", new_version);
		file_bin = open(bin_file_name, O_RDONLY | O_BINARY, 0);
		if (file_bin == -1) {
			rt_kprintf("Error:%s.bin not found!", bin_file_name);
			goto to_application;
		}
		
		new_bin_file_size = lseek(file_bin, 0, SEEK_END);
		if (-1 == new_bin_file_size) {
			rt_kprintf("Error:%s.bin is null!", bin_file_name);
			goto to_application;
		}

		user_app_addr = USER_APP_ADDRESS;
		if (ef_erase_user_app(user_app_addr, new_bin_file_size)) {
				rt_kprintf("Erase user app fail.\n");
				goto to_application;
		} else {
				rt_kprintf("Erase user app success.\n");
		}
		
		lseek(file_bin, 0, SEEK_SET);
    for (cur_size = 0; cur_size < new_bin_file_size; cur_size += sizeof(buff)) {
        app_cur_addr = user_app_addr + cur_size;

        read(file_bin, buff, sizeof(buff));
        result = ef_port_write(app_cur_addr, buff, sizeof(buff));
        if (result != EF_NO_ERR) {
            break;
        }
    }

		//ef_set_env("version", new_version);
		//ef_save_env();
		
		goto to_application;
		
to_application:			
		if (file_v != -1) {
			close(file_v);
		}
		if (file_bin != -1) {
			close(file_bin);
		}
		
		iap_load_app(USER_APP_ADDRESS);
		goto to_open_usb;
		
to_open_usb:
		
		dfs_unmount("/");
			
		rt_usbd_msc_class_register();
		stm_usbd_register();
		
    return 0;
}


typedef  void (*iapfun)(void);
iapfun jump2app;

__asm void MSR_MSP(uint32_t addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}

void iap_load_app(uint32_t appxaddr)
{
	if(((*(volatile uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		//rt_kprintf("jump to %s", appxaddr);
		rt_hw_interrupt_disable();
		jump2app=(iapfun)*(volatile uint32_t*)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(volatile uint32_t*)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();									//��ת��APP.
	}
}	


void gotoApplication(void)
{
	iap_load_app(USER_APP_ADDRESS);
}
MSH_CMD_EXPORT(gotoApplication, goto application!)



static char file_read_buf[CONFIG_FILE_MAX_SIZE] = {0};


void bootloader(void)
{
	int file_system;
	int file_update;
	int bin_file;
	int system_version = 0;
	int update_version = 0;
	uint32_t offset = 0;
	//int system_file_size = 0;
	//int update_file_size = 0;
	uint32_t boot_addr = USER_APP_ADDRESS;
	char *p;
	uint32_t update_bin_size = 0;
	uint16_t check_sum = 0;
	uint32_t new_boot_addr = 0;
	
	uint32_t cur_size;
	uint32_t app_cur_addr;
	int result = 0;
	uint32_t buff[32];
	char bin_file_name[16] = {0};
	int bin_file_size = 0;
	
	rt_pin_mode(BOOTLOADER_CHECK_KEY, PIN_MODE_INPUT_PULLDOWN);
	
	//�������¹̼�����bootloader�еȴ���������
	if (rt_pin_read(BOOTLOADER_CHECK_KEY) == 1) {	//�˴�����Ϊ�����ӵ�
		goto to_open_usb;
	}
	
	/* mount sd card fat partition 1 as root directory */  
//	if (dfs_mount("flash0", "/", "elm", 0, 0) == 0) {
//		rt_kprintf("flash0 mount to /.\n");  
//	} else {
//		return;
//	}
	if (dfs_mount("flash0", "/", "elm", 0, 0) == 0) {
			rt_kprintf("flash0 mount to /.\n");  
	} else {
			rt_kprintf("flash0 mount to / failed!, fatmat and try again!\n");
			/* fatmat filesystem. */
			dfs_mkfs("elm", "flash0");
			/* re-try mount. */
			if (dfs_mount("flash0", "/", "elm", 0, 0) != 0) {
					rt_kprintf("sd0 mount to / failed.\n"); 
			}
	}
		
	// open system.cfg
	file_system = open("system.cfg", O_RDWR, 0);
	if (file_system == -1) {
		rt_kprintf("Error:system.cfg not found!");
		goto to_application;	//goto to_open_usb;	//����ת��app����bootloader�еȴ�����ָ����û����µ���version.txt�ļ�
	}
	if (-1 == read(file_system, file_read_buf, CONFIG_FILE_MAX_SIZE)) {
		rt_kprintf("Error:system.cfg system version not found!");
		goto to_application;
	}

	offset = LocateString(file_read_buf, "version:", 16);
	if (offset == 0) {
		rt_kprintf("Error:system.cfg error content !");
		goto to_application;
	}
	system_version = atol(&file_read_buf[offset]);
	
	p = &file_read_buf[offset];
	
	offset = LocateString(p, "boot_addr:", 50);
	if (offset == 0) {
		rt_kprintf("Error:system.cfg boot_addr not found !");
		goto to_application;
	}
	boot_addr = strtol(&p[offset],NULL,16);
	if (boot_addr > 0x08060000 || boot_addr < 0x08000000) {
		rt_kprintf("Error:system.cfg boot_addr out of range !");
		boot_addr = USER_APP_ADDRESS;
		goto to_application;
	}
	
	memset(file_read_buf, 0, CONFIG_FILE_MAX_SIZE);
	// open update.cfg
	file_update = open("update.cfg", O_RDONLY, 0);
	if (file_update == -1) {
		rt_kprintf("Error:update.cfg not found!");
		goto to_application;
	}
	
	if (-1 == read(file_update, file_read_buf, CONFIG_FILE_MAX_SIZE)) {
		rt_kprintf("Error:update.cfg update version not found!");
		goto to_application;
	}
	offset = LocateString(file_read_buf, "version:", 16);
	if (offset == 0) {
		rt_kprintf("Error:update.cfg error content !");
		goto to_application;
	}
	update_version = atol(&file_read_buf[offset]);
	
	// contrast version
	if (update_version <= system_version) {
		goto to_application;
	}
	
	
	p = &file_read_buf[offset];
	
	offset = LocateString(p, "bin_size:", 50);
	if (offset == 0) {
		rt_kprintf("Error:update.cfg bin_size not found !");
		goto to_application;
	}
	update_bin_size = atol(&p[offset]);
	if (update_bin_size == 0) {
		rt_kprintf("Error:update.cfg bin_size zero !");
		goto to_application;
	}
	
	p = &p[offset];
	offset = LocateString(p, "check_sum:", 50);
	if (offset == 0) {
		rt_kprintf("Error:update.cfg bin_size not found !");
		goto to_application;
	}
	check_sum = strtol(&p[offset],NULL,10);
	if (update_bin_size == 0) {
		rt_kprintf("Error:update.cfg bin_size zero !");
		goto to_application;
	}
	
	p = &p[offset];
	offset = LocateString(p, "boot_addr:", 50);
	if (offset == 0) {
		rt_kprintf("Error:update.cfg boot_addr not found !");
		goto to_application;
	}
	new_boot_addr = strtol(&p[offset],NULL,16);
	if (new_boot_addr > 0x08060000 || new_boot_addr < 0x08000000) {
		rt_kprintf("Error:update.cfg boot_addr zero !");
		goto to_application;
	}
	
	boot_addr = new_boot_addr;
	
	// open bin file
	sprintf(bin_file_name, "%d.bin", update_version);
	bin_file = open(bin_file_name, O_RDONLY | O_BINARY, 0);
	if (bin_file == -1) {
		rt_kprintf("Error:%s.bin not found!", bin_file_name);
		goto to_application;
	}
	
	bin_file_size = lseek(bin_file, 0, SEEK_END);
	if (-1 == bin_file_size) {
		rt_kprintf("Error:%s.bin is null!", bin_file_name);
		goto to_application;
	}

	if (update_bin_size != bin_file_size) {
		rt_kprintf("Error:%s.bin length error!", bin_file_name);
		goto to_application;
	}

	new_boot_addr = USER_APP_ADDRESS;
	if (ef_erase_user_app(new_boot_addr, update_bin_size)) {
			rt_kprintf("Erase user app fail.\n");
			goto to_application;
	} else {
			rt_kprintf("Erase user app success.\n");
	}
	
	lseek(bin_file, 0, SEEK_SET);
	for (cur_size = 0; cur_size < bin_file_size; cur_size += sizeof(buff)) {
		app_cur_addr = new_boot_addr + cur_size;

		read(bin_file, buff, sizeof(buff));
		result = ef_port_write(app_cur_addr, buff, sizeof(buff));
		if (result != EF_NO_ERR) {
				break;
		}
	}
		
	lseek(file_system, 0, SEEK_SET);
	memset(file_read_buf, 0, CONFIG_FILE_MAX_SIZE);
	sprintf(file_read_buf, "version:%d\r\nboot_addr:0x%x", update_version, new_boot_addr);
	if (-1 == write(file_system, file_read_buf, strlen(file_read_buf))) {
		rt_kprintf("Error:system.cfg system write fail!");
	}
	
	goto to_application;
	
to_application:			
		if (file_system != -1) {
			close(file_system);
		}
		if (file_update != -1) {
			close(file_update);
		}
		if (bin_file != -1) {
			close(bin_file);
		}
		
		iap_load_app(boot_addr);
		
		goto to_open_usb;
		
to_open_usb:
		
		dfs_unmount("/");
			
		rt_usbd_msc_class_register();
		stm_usbd_register();
		
}



static uint16_t LocateString(const char * src, const char * str, uint16_t src_len)
{
  uint16_t  i, off;

  i = strlen(str);
  off = 0;
  while(i <= src_len) 
  {
    if (memcmp(&src[off], &str[0], i) == 0) 
    {
      return (off+i);
    }
    off++;
    src_len--;
  }
  return 0;
}

