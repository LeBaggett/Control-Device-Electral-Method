#include "oled_probe.h"
#include "pinout.h"
#include <stdbool.h>
#include <stddef.h>
I2C_HandleTypeDef hi2c1;
volatile oled_test_debug_t g_oled_test={.magic=0x4F4C4544U,.version=1};
static bool scl_high(void){return HAL_GPIO_ReadPin(OLED_SCL_PORT,OLED_SCL_PIN)==GPIO_PIN_SET;}
static bool sda_high(void){return HAL_GPIO_ReadPin(OLED_SDA_PORT,OLED_SDA_PIN)==GPIO_PIN_SET;}
static void snapshot(void){
 g_oled_test.scl=scl_high();g_oled_test.sda=sda_high();
 g_oled_test.hal_state=hi2c1.State;g_oled_test.hal_error=hi2c1.ErrorCode;
 g_oled_test.sr1=I2C1->SR1;g_oled_test.sr2=I2C1->SR2;
 g_oled_test.gpio_moder=GPIOB->MODER;g_oled_test.gpio_afrh=GPIOB->AFR[1];
}
static bool wait_scl(void){
 for(unsigned i=0;i<5;i++){if(scl_high())return true;HAL_Delay(1);}
 return scl_high();
}
static bool bus_release(void){
 OLED_GPIO_CLOCK_ENABLE();__HAL_RCC_I2C1_CLK_ENABLE();
 __HAL_RCC_I2C1_FORCE_RESET();__HAL_RCC_I2C1_RELEASE_RESET();
 GPIO_InitTypeDef p={0};p.Mode=GPIO_MODE_OUTPUT_OD;p.Pull=OLED_GPIO_PULL;
 p.Speed=GPIO_SPEED_FREQ_LOW;
 /* SET releases the open-drain output; never drive a bus line high. */
 HAL_GPIO_WritePin(OLED_SCL_PORT,OLED_SCL_PIN,GPIO_PIN_SET);
 HAL_GPIO_WritePin(OLED_SDA_PORT,OLED_SDA_PIN,GPIO_PIN_SET);
 p.Pin=OLED_SCL_PIN;HAL_GPIO_Init(OLED_SCL_PORT,&p);
 p.Pin=OLED_SDA_PIN;HAL_GPIO_Init(OLED_SDA_PORT,&p);HAL_Delay(2);
 g_oled_test.scl_before=scl_high();g_oled_test.sda_before=sda_high();
 if(!wait_scl()){g_oled_test.failure=2;snapshot();return false;}
 for(unsigned i=0;i<9 && !sda_high();i++){
  HAL_GPIO_WritePin(OLED_SCL_PORT,OLED_SCL_PIN,GPIO_PIN_RESET);HAL_Delay(1);
  HAL_GPIO_WritePin(OLED_SCL_PORT,OLED_SCL_PIN,GPIO_PIN_SET);
  g_oled_test.pulses++;
  if(!wait_scl()){g_oled_test.failure=2;snapshot();return false;}
  HAL_Delay(1);
 }
 if(!sda_high()){g_oled_test.failure=3;snapshot();return false;}
 /* Generate STOP: SDA rises while released SCL is high. */
 HAL_GPIO_WritePin(OLED_SCL_PORT,OLED_SCL_PIN,GPIO_PIN_RESET);
 HAL_GPIO_WritePin(OLED_SDA_PORT,OLED_SDA_PIN,GPIO_PIN_RESET);HAL_Delay(1);
 HAL_GPIO_WritePin(OLED_SCL_PORT,OLED_SCL_PIN,GPIO_PIN_SET);
 if(!wait_scl()){
  HAL_GPIO_WritePin(OLED_SDA_PORT,OLED_SDA_PIN,GPIO_PIN_SET);
  g_oled_test.failure=2;snapshot();return false;
 }
 HAL_Delay(1);HAL_GPIO_WritePin(OLED_SDA_PORT,OLED_SDA_PIN,GPIO_PIN_SET);HAL_Delay(1);
 if(!sda_high()){g_oled_test.failure=3;snapshot();return false;}
 return true;
}
static bool send_bytes(uint8_t *bytes,uint16_t n){
 HAL_StatusTypeDef s=HAL_I2C_Master_Transmit(&hi2c1,(uint16_t)(g_oled_test.address_7bit<<1),bytes,n,50);
 g_oled_test.hal_status=s;g_oled_test.transactions++;snapshot();return s==HAL_OK;
}
void oled_test_run(void){
 g_oled_test.attempts++;g_oled_test.phase=1;g_oled_test.failure=0;
 g_oled_test.ready=0;g_oled_test.address_7bit=0;g_oled_test.ack_3c=g_oled_test.ack_3d=0;
 g_oled_test.pulses=0;g_oled_test.transactions=0;
 g_oled_test.core_clock=SystemCoreClock;g_oled_test.pclk1=HAL_RCC_GetPCLK1Freq();
 if(!bus_release())return;
 g_oled_test.phase=4;
 /* Reset before AF pin assignment; HSI=16MHz, PCLK1=16MHz. */
 __HAL_RCC_I2C1_FORCE_RESET();__HAL_RCC_I2C1_RELEASE_RESET();
 GPIO_InitTypeDef p={0};p.Mode=GPIO_MODE_AF_OD;p.Pull=OLED_GPIO_PULL;
 p.Speed=GPIO_SPEED_FREQ_LOW;p.Alternate=OLED_GPIO_AF;
 p.Pin=OLED_SCL_PIN;HAL_GPIO_Init(OLED_SCL_PORT,&p);
 p.Pin=OLED_SDA_PIN;HAL_GPIO_Init(OLED_SDA_PORT,&p);
 hi2c1=(I2C_HandleTypeDef){0};hi2c1.Instance=I2C1;
 hi2c1.Init.ClockSpeed=OLED_BUS_HZ;hi2c1.Init.DutyCycle=I2C_DUTYCYCLE_2;
 hi2c1.Init.AddressingMode=I2C_ADDRESSINGMODE_7BIT;
 hi2c1.Init.DualAddressMode=I2C_DUALADDRESS_DISABLE;
 hi2c1.Init.GeneralCallMode=I2C_GENERALCALL_DISABLE;hi2c1.Init.NoStretchMode=I2C_NOSTRETCH_DISABLE;
 HAL_StatusTypeDef s=HAL_I2C_Init(&hi2c1);g_oled_test.hal_status=s;snapshot();
 if(s!=HAL_OK){g_oled_test.failure=4;return;}
 g_oled_test.phase=5;
 s=HAL_I2C_IsDeviceReady(&hi2c1,0x3C<<1,2,20);g_oled_test.ack_3c=(s==HAL_OK);
 if(s==HAL_OK)g_oled_test.address_7bit=0x3C;
 else{
  s=HAL_I2C_IsDeviceReady(&hi2c1,0x3D<<1,2,20);g_oled_test.ack_3d=(s==HAL_OK);
  if(s==HAL_OK)g_oled_test.address_7bit=0x3D;
 }
 g_oled_test.hal_status=s;snapshot();
 if(!g_oled_test.address_7bit){g_oled_test.failure=5;return;}
 g_oled_test.phase=6;
 uint8_t init[]={0,0xAE,0xD5,0x80,0xA8,0x3F,0xD3,0,0x40,
#if OLED_CONTROLLER == OLED_CONTROLLER_SSD1306
 0x8D,0x14,0x20,0x02,
#else
 0xAD,0x8B,
#endif
 0xA1,0xC8,0xDA,0x12,0x81,0x7F,0xD9,0xF1,0xDB,0x40,0xA6,0xA5,0xAF};
 if(!send_bytes(init,sizeof(init))){g_oled_test.failure=6;return;}
 /* Entire display ON test bypasses framebuffer/addressing for two seconds. */
 HAL_Delay(2000);g_oled_test.phase=7;
 for(unsigned page=0;page<8;page++){
  const uint8_t off=OLED_CONTROLLER==OLED_CONTROLLER_SH1106?2:0;
  uint8_t pos[]={0,(uint8_t)(0xB0+page),(uint8_t)(off&15),(uint8_t)(0x10|(off>>4))};
  if(!send_bytes(pos,sizeof(pos))){g_oled_test.failure=7;return;}
  for(unsigned block=0;block<8;block++){
   uint8_t data[17];data[0]=0x40;
   for(unsigned i=1;i<17;i++){
    unsigned x=block*16+i-1;
    data[i]=(x==0||x==127)?0xFF:(page==0?0x01:(page==7?0x80:((x/8+page)%2?0xAA:0x55)));
   }
   if(!send_bytes(data,sizeof(data))){g_oled_test.failure=7;return;}
  }
 }
 uint8_t ram[]={0,0xA4};if(!send_bytes(ram,sizeof(ram))){g_oled_test.failure=7;return;}
 g_oled_test.ready=1;g_oled_test.phase=8;snapshot();
}
