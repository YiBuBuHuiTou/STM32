/*
 * oled.c
 *
 *  Created on: Jul 17, 2021
 *      Author: YiBuBuHuiTou
 */
#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "oled.h"

void oled_init(void) {
	uint8_t init_cmd[] = {
			0xAE, //display off
			0x20, //Set Memory Addressing Mode
			0x10, //00,Horizontal Addressing Mode,01,Vertical Addressing Mode,10,Page Addressing Mode (RESET),11,Invalid
			0xb0, //Set Page Start Address for Page Addressing Mode,0-7
			0xc8, //Set COM Output Scan Direction
			0x00, //---set low column address
			0x10, //---set high column address
			0x40, //--set start line address
			0x81, //--set contrast control register
			0xFF, //亮度调节 0x00~0xff
			0xa1, //--set segment re-map 0 to 127
			0xa6, //--set normal display
			0xa8, //--set multiplex ratio(1 to 64)
			0x3F, //
			0xa4, //0xa4,Output follows RAM content,0xa5,Output ignores RAM content
			0xd3, //-set display offset
			0x00, //-not offset
			0xd5, //--set display clock divide ratio/oscillator frequency
			0xf0, //--set divide ratio
			0xd9, //--set pre-charge period
			0x22, //
			0xda, //--set com pins hardware configuration
			0x12,
			0xdb, //--set vcomh
			0x20, //0x20,0.77xVcc
			0x8d, //--set DC-DC enable
			0x14, //
			0xaf, //--turn on oled panel

	};
	HAL_Delay(100);
	if (HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_READY) {
		HAL_I2C_Mem_Write(&hi2c1, I2C_SLAVE_OLED_ADDRESS, I2C_SLAVE_OLED_COMMAND, I2C_MEMADD_SIZE_8BIT, init_cmd, sizeof(init_cmd), 1000);
	}

}

/**
 * x 列
 * y 页
 *  *************************************************************************************
 *        | seg0| seg1| seg2| ........................|seg126| seg127|
 *  page0 |     |     |     |.........................|      |       |
 *  page1 |     |     |     |.........................|      |       |
 *  page2 |     |     |     |.........................|      |       |
 *  page3 |     |     |     |.........................|      |       |
 *  page4 |     |     |     |.........................|      |       |
 *  page5 |     |     |     |.........................|      |       |
 *  page6 |     |     |     |.........................|      |       |
 *  page7 |     |     |     |.........................|      |       |
 *
 */
void oled_SetPos(unsigned char seg, unsigned char page) //设置起始点坐标
{
	uint8_t page1 = 0xB0 + page;
	uint8_t seg_lsb = (seg&0x0F)|0x01;
	uint8_t seg_msb = ((seg&0xF0)>>4)|0x10;
	HAL_I2C_Mem_Write(&hi2c1, I2C_SLAVE_OLED_ADDRESS, I2C_SLAVE_OLED_COMMAND, I2C_MEMADD_SIZE_8BIT, &page1, sizeof(page1), 1000 );
	// 设置列 把列拆成高低各四位分别 或上0x10 和0x00
	HAL_I2C_Mem_Write(&hi2c1, I2C_SLAVE_OLED_ADDRESS, I2C_SLAVE_OLED_COMMAND, I2C_MEMADD_SIZE_8BIT, &seg_lsb, sizeof(seg_lsb), 1000 );
	HAL_I2C_Mem_Write(&hi2c1, I2C_SLAVE_OLED_ADDRESS, I2C_SLAVE_OLED_COMMAND, I2C_MEMADD_SIZE_8BIT, &seg_msb, sizeof(seg_msb), 1000 );

}

void oled_fill(unsigned char data) {
	uint8_t temp[128];
	for (uint8_t i=0; i < 128; i++){
		temp[i] = data;
	}
	for(uint8_t page = 0; page < 8; page++) {
		oled_SetPos(0, page);
		HAL_I2C_Mem_Write(&hi2c1, I2C_SLAVE_OLED_ADDRESS, I2C_SLAVE_OLED_DATA, I2C_MEMADD_SIZE_8BIT, &temp[0], sizeof(temp), 1000 );
	}
}
