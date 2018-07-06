/**********************************************************************************************************
                                天穹飞控 —— 致力于打造中国最好的多旋翼开源飞控
                                Github: github.com/loveuav/BlueSkyFlightControl
                                技术讨论：bbs.loveuav.com/forum-68-1.html
 * @文件     board.c
 * @说明     硬件初始化
 * @版本  	 V1.0
 * @作者     BlueSky
 * @网站     bbs.loveuav.com
 * @日期     2018.05 
**********************************************************************************************************/
#include "board.h"

#include "FreeRTOS.h"
#include "task.h"

#include "drv_spi.h"
#include "drv_usart.h"
#include "drv_i2c_soft.h"
#include "drv_pwm.h"
#include "drv_sbus.h"
#include "drv_usbhid.h"
#include "drv_adc.h"
#include "drv_ppm.h"

static void SysPeriphClockInit(void);

/**********************************************************************************************************
*函 数 名: BoardInit
*功能说明: 飞控硬件初始化
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
void BoardInit(void)
{
	//中断优先级组别设置
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	//外设时钟使能
	SysPeriphClockInit();
	
	//SPI初始化
    Spi_GPIO_Init();
	#if (configUSE_SPI1 == 1)
		Spi_Open(1);
	#endif
	#if (configUSE_SPI2 == 1)
		Spi_Open(2);
	#endif
    
    //串口初始化
	Usart_Open(GPS_UART, GPS_BAUDRATE);
	Usart_Open(DATA_UART, DATA_BAUDRATE);
	Usart_Open(TOF_UART, TOF_BAUDRATE);
	Usart_Open(SBUS_UART, SBUS_BAUDRATE);
    
    //软件I2C初始化
    #if (configUSE_SOFT_I2C1 == 1)
		Soft_I2c_Open(1);
	#endif    
    #if (configUSE_SOFT_I2C2 == 1)
		Soft_I2c_Open(2);
	#endif 

    //定时器PWM输出初始化
    PWM_Init();
	
	//遥控器接收机数据协议解析初始化
	if(RC_PROTOCOL == SBUS)
		Sbus_Init();
	else if(RC_PROTOCOL == PPM)
        PPM_Init();

	//USB HID初始化
	UsbHid_Init();
	
	//ADC初始化
	Adc_Init();
}

/**********************************************************************************************************
*函 数 名: SysPeriphClockInit
*功能说明: 单片机外设时钟使能
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
static void SysPeriphClockInit(void)
{
	//GPIO
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  
	//串口
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
    
	//SPI
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	//定时器
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    //ADC
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3,ENABLE);
}


/**********************************************************************************************************
*函 数 名: SoftDelayMs
*功能说明: 毫秒级软件延时，基于STM32F4 168M主频的调试值，不同单片机有所区别
*形    参: 延时时间
*返 回 值: 无
**********************************************************************************************************/
void SoftDelayMs(uint32_t ms)
{
	uint32_t us_cnt; 
	for(; ms!= 0;ms--)
    {
        us_cnt = 42000;
		while(us_cnt)
        {
			us_cnt--;
		}	
    }		
}

/**********************************************************************************************************
*函 数 名: SoftDelayUs
*功能说明: 微秒级软件延时，基于STM32F4 168M主频的调试值，不同单片机有所区别
*形    参: 延时时间
*返 回 值: 无
**********************************************************************************************************/
void SoftDelayUs(uint32_t us)
{
	uint32_t us_cnt; 
	for(; us!= 0;us--)
    {
        us_cnt = 35;
		while(us_cnt)
        {
			us_cnt--;
		}
    }        
}

/**********************************************************************************************************
*函 数 名: GetSysTimeUs
*功能说明: 获取当前系统运行时间，单位为微秒
*形    参: 无
*返 回 值: 系统时间
**********************************************************************************************************/
uint32_t GetSysTimeUs(void) 
{
  register uint32_t ms;
	uint32_t value;
	ms = xTaskGetTickCount();
	value = ms * 1000 + (SysTick->LOAD - SysTick->VAL) * 1000 / SysTick->LOAD;
	return value;
}

/**********************************************************************************************************
*函 数 名: GetSysTimeMs
*功能说明: 获取当前系统运行时间，单位为毫秒
*形    参: 无
*返 回 值: 系统时间
**********************************************************************************************************/
uint32_t GetSysTimeMs(void) 
{
	return xTaskGetTickCount();
}


