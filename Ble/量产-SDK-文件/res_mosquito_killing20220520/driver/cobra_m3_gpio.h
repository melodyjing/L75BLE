/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_m3_gpio.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/2

	Description :   
	
	Changed Log	:

		Liujian  2019/4/2		- Creation



*****************************************************************************/

#ifndef __COBRA_M3_GPIO__
#define __COBRA_M3_GPIO__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_M3_GPIO_EN

typedef union {
//		uint32_t val;
    uint8_t val;
    struct {
        uint8_t data0:1;
        uint8_t data1:1;
        uint8_t data2:1;
        uint8_t data3:1;
        uint8_t data4:1;
        uint8_t data5:1;
        uint8_t data6:1;
        uint8_t data7:1;
//        uint32_t :24;
    } BITS;
} RDW_TGPIO_DATA;
typedef union {
    uint32_t val;
    struct {
        uint32_t dir0:1;		//1:output 0:input
        uint32_t dir1:1;		//1:output 0:input
        uint32_t dir2:1;		//1:output 0:input
        uint32_t dir3:1;		//1:output 0:input
        uint32_t dir4:1;		//1:output 0:input
        uint32_t dir5:1;		//1:output 0:input
        uint32_t dir6:1;		//1:output 0:input
        uint32_t dir7:1;		//1:output 0:input
        uint32_t :24;
    } BITS;
} RDW_TGPIO_DIR;
typedef union {
    uint32_t val;
    struct {
        uint32_t detect0:1;		//interrupt detect 1:level 0:edge
        uint32_t detect1:1;		//interrupt detect 1:level 0:edge
        uint32_t detect2:1;		//interrupt detect 1:level 0:edge
        uint32_t detect3:1;		//interrupt detect 1:level 0:edge
        uint32_t detect4:1;		//interrupt detect 1:level 0:edge
        uint32_t detect5:1;		//interrupt detect 1:level 0:edge
        uint32_t detect6:1;		//interrupt detect 1:level 0:edge
        uint32_t detect7:1;		//interrupt detect 1:level 0:edge
        uint32_t :24;
    } BITS;
} RDW_TGPIO_IS;
typedef union {
    uint32_t val;
    struct {
        uint32_t bothedge0:1;		//interrupt both edge 1:edge 0:level
        uint32_t bothedge1:1;		//interrupt both edge 1:edge 0:level
        uint32_t bothedge2:1;		//interrupt both edge 1:edge 0:level
        uint32_t bothedge3:1;		//interrupt both edge 1:edge 0:level
        uint32_t bothedge4:1;		//interrupt both edge 1:edge 0:level
        uint32_t bothedge5:1;		//interrupt both edge 1:edge 0:level
        uint32_t bothedge6:1;		//interrupt both edge 1:edge 0:level
        uint32_t bothedge7:1;		//interrupt both edge 1:edge 0:level
        uint32_t :24;
    } BITS;
} RDW_TGPIO_IBE;
typedef union {
    uint32_t val;
    struct {
        uint32_t even0:1;		//1:edge rising/high level 0:edge falling/low level
        uint32_t even1:1;		//1:edge rising/high level 0:edge falling/low level
        uint32_t even2:1;		//1:edge rising/high level 0:edge falling/low level
        uint32_t even3:1;		//1:edge rising/high level 0:edge falling/low level
        uint32_t even4:1;		//1:edge rising/high level 0:edge falling/low level
        uint32_t even5:1;		//1:edge rising/high level 0:edge falling/low level
        uint32_t even6:1;		//1:edge rising/high level 0:edge falling/low level
        uint32_t even7:1;		//1:edge rising/high level 0:edge falling/low level
        uint32_t :24;
    } BITS;
} RDW_TGPIO_IEV;
typedef union {
    uint32_t val;
    struct {
        uint32_t mask0:1;	//trigger 1:no mask  0:mask
        uint32_t mask1:1;	//trigger 1:no mask  0:mask
        uint32_t mask2:1;	//trigger 1:no mask  0:mask
        uint32_t mask3:1;	//trigger 1:no mask  0:mask
        uint32_t mask4:1;	//trigger 1:no mask  0:mask
        uint32_t mask5:1;	//trigger 1:no mask  0:mask
        uint32_t mask6:1;	//trigger 1:no mask  0:mask
        uint32_t mask7:1;	//trigger 1:no mask  0:mask
        uint32_t :24;
    } BITS;
} RDW_TGPIO_IE;
typedef union {
    uint32_t val;
    struct {
        uint32_t raw0 :1;	//raw status read
        uint32_t raw1 :1;	//raw status read
        uint32_t raw2 :1;	//raw status read
        uint32_t raw3 :1;	//raw status read
        uint32_t raw4 :1;	//raw status read
        uint32_t raw5 :1;	//raw status read
        uint32_t raw6 :1;	//raw status read
        uint32_t raw7 :1;	//raw status read
        uint32_t :24;
    } BITS;
} RDW_TGPIO_RIS;
typedef union {
    uint32_t val;
    struct {
        uint32_t mask0:1;	//read mask
        uint32_t mask1:1;	//read mask
        uint32_t mask2:1;	//read mask
        uint32_t mask3:1;	//read mask
        uint32_t mask4:1;	//read mask
        uint32_t mask5:1;	//read mask
        uint32_t mask6:1;	//read mask
        uint32_t mask7:1;	//read mask
        uint32_t :24;
    } BITS;
} RDW_TGPIO_MIS;
typedef union {
    uint32_t val;
    struct {
        uint32_t clear0:1;		//clear interrupt flag write 1:clear 0:no effect
        uint32_t clear1:1;		//clear interrupt flag write 1:clear 0:no effect
        uint32_t clear2:1;		//clear interrupt flag write 1:clear 0:no effect
        uint32_t clear3:1;		//clear interrupt flag write 1:clear 0:no effect
        uint32_t clear4:1;		//clear interrupt flag write 1:clear 0:no effect
        uint32_t clear5:1;		//clear interrupt flag write 1:clear 0:no effect
        uint32_t clear6:1;		//clear interrupt flag write 1:clear 0:no effect
        uint32_t clear7:1;		//clear interrupt flag write 1:clear 0:no effect
        uint32_t :24;
    } BITS;
} RDW_TGPI_IC;
typedef union {
    uint32_t val;
    struct {
        uint32_t hwsw0:1;		//1:hw control 0:sw control
        uint32_t hwsw1:1;		//1:hw control 0:sw control
        uint32_t hwsw2:1;		//1:hw control 0:sw control
        uint32_t hwsw3:1;		//1:hw control 0:sw control
        uint32_t hwsw4:1;		//1:hw control 0:sw control
        uint32_t hwsw5:1;		//1:hw control 0:sw control
        uint32_t hwsw6:1;		//1:hw control 0:sw control
        uint32_t hwsw7:1;		//1:hw control 0:sw control
        uint32_t :24;
    } BITS;
} RDW_TGPIO_AFSEL;

typedef struct cobra_m3_gpio_type
{
		RDW_TGPIO_DATA     	DATA[1024];
		RDW_TGPIO_DIR     	DIR;		//1:output 0:input
		RDW_TGPIO_IS     		IS;			//interrupt detect 1:level 0:edge
		RDW_TGPIO_IBE     	IBE;		//interrupt both edge 1:edge 0:level
		RDW_TGPIO_IEV     	IEV;		//1:edge rising/high level 0:edge falling/low level
		RDW_TGPIO_IE     		IE;			//trigger 1:no mask  0:mask
		RDW_TGPIO_RIS     	RIS;		//raw status read
		RDW_TGPIO_MIS    		MIS;		//read mask
		RDW_TGPI_IC     		IC;			//clear interrupt flag write 1:clear 0:no effect
		RDW_TGPIO_AFSEL     AFSEL;		//1:hw control 0:sw control ;default 0
} COBRA_M3_GPIO_TypeDef; 

enum{
	PIN0 = 0,
	PIN1 = 1,
	PIN2 = 2,
	PIN3 = 3,
	PIN4 = 4,
	PIN5 = 5,
	PIN6 = 6,
	PIN7 = 7
};

/// Enumeration of GPIO pins
typedef enum
{
    GPIO_PIN_0 = 0,				//M3 GPIO 0
    GPIO_PIN_1 = 1,
    GPIO_PIN_2 = 2,
    GPIO_PIN_3 = 3,
    GPIO_PIN_4 = 4,
    GPIO_PIN_5 = 5,
    GPIO_PIN_6 = 6,
    GPIO_PIN_7 = 7,
	
		GPIO_PIN_8 = 8,               // M3 GPIO 0
    GPIO_PIN_9 = 9,
    GPIO_PIN_10 = 10,
    GPIO_PIN_11 = 11,
    GPIO_PIN_12 = 12,
    GPIO_PIN_13 = 13,
    GPIO_PIN_14 = 14,
    GPIO_PIN_15 = 15,
	

    GPIO_PIN_MAX = 16
	
}gpio_pin;

typedef enum {
	M3_GPIO0,
	M3_GPIO1
}GPIO_INDEX;

typedef enum
{
    GPIO_LOW = 0,
    GPIO_HIGH  = 1
}gpio_level;

typedef enum
{
    INT_LEVEL = 0,
    INT_EDGE  = 1
}GPIO_INT_TYPE;

typedef enum
{
		GPIO_OUTPUT = 0,
		GPIO_INPUT = 1
}gpio_dir;

typedef enum
{
	  GPIO_LEVEL_HIGH = 0,
	  GPIO_LEVEL_LOW  = 1,
	  GPIO_EDGE_RISE  = 2,
	  GPIO_EDGE_FALL  = 3,
	  GPIO_EDGE_BOTH  = 4
}gpio_interrupt_type;


void cobra_m3_gpio_init(void) ;
void cobra_m3_gpio_interrupt_sense_set(GPIO_INDEX io_index, gpio_pin pin, GPIO_INT_TYPE type);
void cobra_m3_gpio_interrupt_disable(GPIO_INDEX io_index, gpio_pin pin);
bool cobra_m3_gpio_get_value(GPIO_INDEX io_index, gpio_pin pin);
void cobra_m3_gpio_set_value(GPIO_INDEX io_index, gpio_pin pin,bool value);
void cobra_m3_gpio_set_output(GPIO_INDEX io_index, gpio_pin pin);
void cobra_m3_gpio_set_input(GPIO_INDEX io_index, gpio_pin pin);
void cobra_m3_gpio_interrupt_edge_set(GPIO_INDEX io_index, gpio_pin pin, bool type);
void cobra_m3_gpio_interrupt_even_set(GPIO_INDEX io_index, gpio_pin pin, bool type);
void cobra_m3_gpio_interrupt_enable(GPIO_INDEX io_index, gpio_pin pin);
void cobra_m3_gpio_interrupt_clear(GPIO_INDEX io_index,   gpio_pin pin);
uint32_t cobra_m3_gpio_interrupt_status(GPIO_INDEX io_index, gpio_pin pin, bool bMask);
uint32_t cobra_m3_gpio_get_mask_status(GPIO_INDEX io_index);
uint32_t cobra_m3_gpio_get_interrupt_status(GPIO_INDEX io_index);
void cobra_m3_gpio_set_interrupt_status_value(GPIO_INDEX io_index, uint32_t value);


#define M3_GPIO0_BASE 	(0x400C0000UL + 0xA000UL)
#define M3_GPIO1_BASE 	(0x400A0000UL + 0x8000UL)

#endif   //COBRA_M3_GPIO_EN



#endif

