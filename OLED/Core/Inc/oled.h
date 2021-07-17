/*
 * oled.h
 *
 *  Created on: Jul 17, 2021
 *      Author: YiBuBuHuiTou
 */

#ifndef INC_OLED_H_
#define INC_OLED_H_

#define I2C_SLAVE_OLED_ADDRESS               0x78
#define I2C_SLAVE_OLED_COMMAND               0x00
#define I2C_SLAVE_OLED_DATA                  0x40

void oled_init( void );
void oled_SetPos(unsigned char x, unsigned char y);
void oled_fill(unsigned char data);
#endif /* INC_OLED_H_ */
