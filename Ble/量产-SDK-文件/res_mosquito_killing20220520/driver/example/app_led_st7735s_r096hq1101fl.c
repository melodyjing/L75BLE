
#include "app_led_st7735s_r096hq1101fl.h"

#if APP_LCD_ST7735S_R096HQ1101FL_ENABLE
#include "cobra_pinmux.h"
#include "cobra_umcu.h"
#include "cobra_spi.h"
#include "cobra_m4_gpio.h"

#include "img_bike_80_80.h"


#define  USE_GPIO_EMULATE_SPI  0

#define  TEST_DELAY_TIME  300

#define LCD_ROW  160
#define LCD_COL 80

#define WHITE_COLOR   0xFFFF
#define BLACK_COLOR   0x0000
#define RED_COLOR     0xF800
#define GREEN_COLOR   0x07E0
#define BLUE_COLOR    0x001F

#define TRANSPARENT_COLOR    0x0001

#define YELLOW_COLOR  0xFFE00

#define GRAY_COLOR   0xEF5D
#define GRAY75_COLOR 0x39E7
#define GRAY50_COLOR 0x7BEF
#define GRAY25_COLOR 0xADB5

#define RGB(r,g,b)  (unsigned int)((r<<11)|(g<<6)|(b))

#define PIC_WIDTH   LCD_COL	 //Ԥ????LCD??ʾ??????????ͼƬ?Ĵ?С
#define PIC_HEIGHT   LCD_ROW

#define  FONT_W_8  8
#define  FONT_H_16  16

#define  FONT_W_16  16
#define  FONT_H_32  32

#define  FONT_W_24  24
#define  FONT_H_48  48


#define  FONT_CH_W  16
#define  FONT_CH_H  16


#define  DC_ONE_HIGH   app_m3_gpio_write_pin(GPIO_PIN_0, GPIO_HIGH)
#define  DC_ZERO_LOW   app_m3_gpio_write_pin(GPIO_PIN_0, GPIO_LOW)

#define  CS_ONE_HIGH   app_m3_gpio_write_pin(GPIO_PIN_29, GPIO_HIGH)
#define  CS_ZERO_LOW   app_m3_gpio_write_pin(GPIO_PIN_29, GPIO_LOW)

#define  RESET_ONE_HIGH   app_m3_gpio_write_pin(GPIO_PIN_27, GPIO_HIGH)
#define  RESET_ZERO_LOW   app_m3_gpio_write_pin(GPIO_PIN_27, GPIO_LOW)

#define  LEDK_ONE_HIGH   app_m3_gpio_write_pin(GPIO_PIN_5, GPIO_HIGH)
#define  LEDK_ZERO_LOW   app_m3_gpio_write_pin(GPIO_PIN_5, GPIO_LOW)


uint16_t POINT_COLOR=0x0000;	/*??????ɫ*/
uint16_t BACK_COLOR=0xFFFF;     /*??????ɫ*/

unsigned char FONT_W = FONT_W_8;
unsigned char FONT_H = FONT_H_16;


/*8*16*/
const unsigned char asc2_1608[95][FONT_W_8*FONT_H_16/8]=
{
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",0*/
    {0x00,0x00,0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x18,0x18,0x00,0x00},/*"!",1*/
    {0x00,0x12,0x36,0x24,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*""",2*/
    {0x00,0x00,0x00,0x24,0x24,0x24,0xFE,0x48,0x48,0x48,0xFE,0x48,0x48,0x48,0x00,0x00},/*"#",3*/
    {0x00,0x00,0x10,0x38,0x54,0x54,0x50,0x30,0x18,0x14,0x14,0x54,0x54,0x38,0x10,0x10},/*"$",4*/
    {0x00,0x00,0x00,0x44,0xA4,0xA8,0xA8,0xA8,0x54,0x1A,0x2A,0x2A,0x2A,0x44,0x00,0x00},/*"%",5*/
    {0x00,0x00,0x00,0x30,0x48,0x48,0x48,0x50,0x6E,0xA4,0x94,0x88,0x89,0x76,0x00,0x00},/*"&",6*/
    {0x00,0x60,0x60,0x20,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"'",7*/
    {0x00,0x02,0x04,0x08,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x08,0x08,0x04,0x02,0x00},/*"(",8*/
    {0x00,0x40,0x20,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x10,0x20,0x40,0x00},/*")",9*/
    {0x00,0x00,0x00,0x00,0x10,0x10,0xD6,0x38,0x38,0xD6,0x10,0x10,0x00,0x00,0x00,0x00},/*"*",10*/
    {0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0xFE,0x10,0x10,0x10,0x10,0x00,0x00,0x00},/*"+",11*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x20,0xC0},/*",",12*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"-",13*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x00,0x00},/*".",14*/
    {0x00,0x00,0x01,0x02,0x02,0x04,0x04,0x08,0x08,0x10,0x10,0x20,0x20,0x40,0x40,0x00},/*"/",15*/
    {0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x24,0x18,0x00,0x00},/*"0",16*/
    {0x00,0x00,0x00,0x10,0x70,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00},/*"1",17*/
    {0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x04,0x04,0x08,0x10,0x20,0x42,0x7E,0x00,0x00},/*"2",18*/
    {0x00,0x00,0x00,0x3C,0x42,0x42,0x04,0x18,0x04,0x02,0x02,0x42,0x44,0x38,0x00,0x00},/*"3",19*/
    {0x00,0x00,0x00,0x04,0x0C,0x14,0x24,0x24,0x44,0x44,0x7E,0x04,0x04,0x1E,0x00,0x00},/*"4",20*/
    {0x00,0x00,0x00,0x7E,0x40,0x40,0x40,0x58,0x64,0x02,0x02,0x42,0x44,0x38,0x00,0x00},/*"5",21*/
    {0x00,0x00,0x00,0x1C,0x24,0x40,0x40,0x58,0x64,0x42,0x42,0x42,0x24,0x18,0x00,0x00},/*"6",22*/
    {0x00,0x00,0x00,0x7E,0x44,0x44,0x08,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00},/*"7",23*/
    {0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x24,0x18,0x24,0x42,0x42,0x42,0x3C,0x00,0x00},/*"8",24*/
    {0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x26,0x1A,0x02,0x02,0x24,0x38,0x00,0x00},/*"9",25*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00},/*":",26*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x20},/*";",27*/
    {0x00,0x00,0x00,0x02,0x04,0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x04,0x02,0x00,0x00},/*"<",28*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00},/*"=",29*/
    {0x00,0x00,0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x04,0x08,0x10,0x20,0x40,0x00,0x00},/*">",30*/
    {0x00,0x00,0x00,0x3C,0x42,0x42,0x62,0x02,0x04,0x08,0x08,0x00,0x18,0x18,0x00,0x00},/*"?",31*/
    {0x00,0x00,0x00,0x38,0x44,0x5A,0xAA,0xAA,0xAA,0xAA,0xB4,0x42,0x44,0x38,0x00,0x00},/*"@",32*/
    {0x00,0x00,0x00,0x10,0x10,0x18,0x28,0x28,0x24,0x3C,0x44,0x42,0x42,0xE7,0x00,0x00},/*"A",33*/
    {0x00,0x00,0x00,0xF8,0x44,0x44,0x44,0x78,0x44,0x42,0x42,0x42,0x44,0xF8,0x00,0x00},/*"B",34*/
    {0x00,0x00,0x00,0x3E,0x42,0x42,0x80,0x80,0x80,0x80,0x80,0x42,0x44,0x38,0x00,0x00},/*"C",35*/
    {0x00,0x00,0x00,0xF8,0x44,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x44,0xF8,0x00,0x00},/*"D",36*/
    {0x00,0x00,0x00,0xFC,0x42,0x48,0x48,0x78,0x48,0x48,0x40,0x42,0x42,0xFC,0x00,0x00},/*"E",37*/
    {0x00,0x00,0x00,0xFC,0x42,0x48,0x48,0x78,0x48,0x48,0x40,0x40,0x40,0xE0,0x00,0x00},/*"F",38*/
    {0x00,0x00,0x00,0x3C,0x44,0x44,0x80,0x80,0x80,0x8E,0x84,0x44,0x44,0x38,0x00,0x00},/*"G",39*/
    {0x00,0x00,0x00,0xE7,0x42,0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x42,0xE7,0x00,0x00},/*"H",40*/
    {0x00,0x00,0x00,0x7C,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00},/*"I",41*/
    {0x00,0x00,0x00,0x3E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x88,0xF0},/*"J",42*/
    {0x00,0x00,0x00,0xEE,0x44,0x48,0x50,0x70,0x50,0x48,0x48,0x44,0x44,0xEE,0x00,0x00},/*"K",43*/
    {0x00,0x00,0x00,0xE0,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x42,0xFE,0x00,0x00},/*"L",44*/
    {0x00,0x00,0x00,0xEE,0x6C,0x6C,0x6C,0x6C,0x54,0x54,0x54,0x54,0x54,0xD6,0x00,0x00},/*"M",45*/
    {0x00,0x00,0x00,0xC7,0x62,0x62,0x52,0x52,0x4A,0x4A,0x4A,0x46,0x46,0xE2,0x00,0x00},/*"N",46*/
    {0x00,0x00,0x00,0x38,0x44,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x44,0x38,0x00,0x00},/*"O",47*/
    {0x00,0x00,0x00,0xFC,0x42,0x42,0x42,0x42,0x7C,0x40,0x40,0x40,0x40,0xE0,0x00,0x00},/*"P",48*/
    {0x00,0x00,0x00,0x38,0x44,0x82,0x82,0x82,0x82,0x82,0xB2,0xCA,0x4C,0x38,0x06,0x00},/*"Q",49*/
    {0x00,0x00,0x00,0xFC,0x42,0x42,0x42,0x7C,0x48,0x48,0x44,0x44,0x42,0xE3,0x00,0x00},/*"R",50*/
    {0x00,0x00,0x00,0x3E,0x42,0x42,0x40,0x20,0x18,0x04,0x02,0x42,0x42,0x7C,0x00,0x00},/*"S",51*/
    {0x00,0x00,0x00,0xFE,0x92,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x38,0x00,0x00},/*"T",52*/
    {0x00,0x00,0x00,0xE7,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00},/*"U",53*/
    {0x00,0x00,0x00,0xE7,0x42,0x42,0x44,0x24,0x24,0x28,0x28,0x18,0x10,0x10,0x00,0x00},/*"V",54*/
    {0x00,0x00,0x00,0xD6,0x92,0x92,0x92,0x92,0xAA,0xAA,0x6C,0x44,0x44,0x44,0x00,0x00},/*"W",55*/
    {0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x18,0x18,0x18,0x24,0x24,0x42,0xE7,0x00,0x00},/*"X",56*/
    {0x00,0x00,0x00,0xEE,0x44,0x44,0x28,0x28,0x10,0x10,0x10,0x10,0x10,0x38,0x00,0x00},/*"Y",57*/
    {0x00,0x00,0x00,0x7E,0x84,0x04,0x08,0x08,0x10,0x20,0x20,0x42,0x42,0xFC,0x00,0x00},/*"Z",58*/
    {0x00,0x1E,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x1E,0x00},/*"[",59*/
    {0x00,0x00,0x40,0x40,0x20,0x20,0x10,0x10,0x10,0x08,0x08,0x04,0x04,0x04,0x02,0x02},/*"\",60*/
    {0x00,0x78,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x78,0x00},/*"]",61*/
    {0x00,0x1C,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"^",62*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF},/*"_",63*/
    {0x00,0x60,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"`",64*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x1E,0x22,0x42,0x42,0x3F,0x00,0x00},/*"a",65*/
    {0x00,0x00,0x00,0xC0,0x40,0x40,0x40,0x58,0x64,0x42,0x42,0x42,0x64,0x58,0x00,0x00},/*"b",66*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x22,0x40,0x40,0x40,0x22,0x1C,0x00,0x00},/*"c",67*/
    {0x00,0x00,0x00,0x06,0x02,0x02,0x02,0x1E,0x22,0x42,0x42,0x42,0x26,0x1B,0x00,0x00},/*"d",68*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x7E,0x40,0x40,0x42,0x3C,0x00,0x00},/*"e",69*/
    {0x00,0x00,0x00,0x0F,0x11,0x10,0x10,0x7E,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00},/*"f",70*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x44,0x44,0x38,0x40,0x3C,0x42,0x42,0x3C},/*"g",71*/
    {0x00,0x00,0x00,0xC0,0x40,0x40,0x40,0x5C,0x62,0x42,0x42,0x42,0x42,0xE7,0x00,0x00},/*"h",72*/
    {0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x70,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00},/*"i",73*/
    {0x00,0x00,0x00,0x0C,0x0C,0x00,0x00,0x1C,0x04,0x04,0x04,0x04,0x04,0x04,0x44,0x78},/*"j",74*/
    {0x00,0x00,0x00,0xC0,0x40,0x40,0x40,0x4E,0x48,0x50,0x68,0x48,0x44,0xEE,0x00,0x00},/*"k",75*/
    {0x00,0x00,0x00,0x70,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00},/*"l",76*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x49,0x49,0x49,0x49,0x49,0xED,0x00,0x00},/*"m",77*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDC,0x62,0x42,0x42,0x42,0x42,0xE7,0x00,0x00},/*"n",78*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00},/*"o",79*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8,0x64,0x42,0x42,0x42,0x44,0x78,0x40,0xE0},/*"p",80*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x22,0x42,0x42,0x42,0x22,0x1E,0x02,0x07},/*"q",81*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEE,0x32,0x20,0x20,0x20,0x20,0xF8,0x00,0x00},/*"r",82*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x42,0x40,0x3C,0x02,0x42,0x7C,0x00,0x00},/*"s",83*/
    {0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x7C,0x10,0x10,0x10,0x10,0x10,0x0C,0x00,0x00},/*"t",84*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6,0x42,0x42,0x42,0x42,0x46,0x3B,0x00,0x00},/*"u",85*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x28,0x10,0x10,0x00,0x00},/*"v",86*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD7,0x92,0x92,0xAA,0xAA,0x44,0x44,0x00,0x00},/*"w",87*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6E,0x24,0x18,0x18,0x18,0x24,0x76,0x00,0x00},/*"x",88*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x28,0x18,0x10,0x10,0xE0},/*"y",89*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x44,0x08,0x10,0x10,0x22,0x7E,0x00,0x00},/*"z",90*/
    {0x00,0x03,0x04,0x04,0x04,0x04,0x04,0x08,0x04,0x04,0x04,0x04,0x04,0x04,0x03,0x00},/*"{",91*/
    {0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},/*"|",92*/
    {0x00,0x60,0x10,0x10,0x10,0x10,0x10,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x60,0x00},/*"}",93*/
    {0x30,0x4C,0x43,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"~",94*/
};

static void app_cobra_lcd_st7735s_test(void);

static void app_cobra_lcd_delay(unsigned int delayMs)
{
#if 1
    while(delayMs--)
    {
        __nop();
        __nop();
    }
#else
    cobra_soft_delay_ms(delayMs);
#endif

}
static void app_cobra_lcd_init(void)
{

	/*stop motor*/
	app_m3_gpio_write_pin(GPIO_PIN_26, GPIO_LOW);

#if (USE_GPIO_EMULATE_SPI)
    /*test IO connnect*/
    cobra_map_pinmodeset(PIN31_INDEX, PIN_MODE_10); 	   /*sdi*/
    app_m3_gpio_set_direction(GPIO_PIN_31, GPIO_OUTPUT);
    app_m3_gpio_write_pin(GPIO_PIN_31, GPIO_HIGH);

    cobra_map_pinmodeset(PIN30_INDEX, PIN_MODE_10); 	   /*scl*/
    app_m3_gpio_set_direction(GPIO_PIN_30, GPIO_OUTPUT);
    app_m3_gpio_write_pin(GPIO_PIN_30, GPIO_HIGH);

    app_cobra_lcd_delay(30);
    app_m3_gpio_write_pin(GPIO_PIN_30, GPIO_LOW);
    app_m3_gpio_write_pin(GPIO_PIN_31, GPIO_LOW);

    app_cobra_lcd_delay(30);
    app_m3_gpio_write_pin(GPIO_PIN_30, GPIO_HIGH);
    app_m3_gpio_write_pin(GPIO_PIN_31, GPIO_HIGH);
#else
    cobra_pcrm_spi1_active_mode_clock_enable(1);
    cobra_map_pinmodeset(PIN30_INDEX, PIN30_SPI1_TX_CLK);

    cobra_mcu_map_aon_pinmodeset(AON_PAD4, AON_PAD4_PAD_DIO);
    cobra_map_pinmodeset(PIN31_INDEX, PIN31_SPI1_TX_DATA);

    cobra_spi_control_set(SPI1, SPI_MASTER, SAMPLE_ON_TRAILING_EDGE, IDLE_HIGH, MSB_FIRST);

    cobra_spi_set_bps(SPI1, SPI_4MHZ);
    cobra_spi_set_bits_per_word(SPI1, SPI_8bit);
    cobra_spi_disbale_tx_interrupt(SPI1);
    cobra_spi_disbale_rx_interrupt(SPI1);
    cobra_spi_enable(SPI1);
#endif

	
    cobra_map_pinmodeset(PIN0_INDEX, PIN_MODE_0);        //LCD command and data
    app_m3_gpio_set_direction(GPIO_PIN_0, GPIO_OUTPUT);
    app_m3_gpio_write_pin(GPIO_PIN_0, GPIO_HIGH);


    cobra_map_pinmodeset(PIN27_INDEX, PIN_MODE_27);      //RESET
    app_m3_gpio_set_direction(GPIO_PIN_27, GPIO_OUTPUT);
    app_m3_gpio_write_pin(GPIO_PIN_27, GPIO_HIGH);


    cobra_map_pinmodeset(PIN29_INDEX, PIN_MODE_29);      //SPI CS
    app_m3_gpio_set_direction(GPIO_PIN_29, GPIO_OUTPUT);
    app_m3_gpio_write_pin(GPIO_PIN_29, GPIO_HIGH);

    cobra_map_pinmodeset(PIN5_INDEX, PIN_MODE_5); 	 //LCD LEDK
    app_m3_gpio_set_direction(GPIO_PIN_5, GPIO_OUTPUT);
    app_m3_gpio_write_pin(GPIO_PIN_5, GPIO_LOW);


    printf("app_cobra_lcd_init ok \n");
}

#if (USE_GPIO_EMULATE_SPI)
static void app_cobra_lcd_write_command(uint8_t command)
{
    uint8_t i,value;
    value = command;
    app_m3_gpio_write_pin(GPIO_PIN_3, GPIO_LOW);   //CS low
    app_m3_gpio_write_pin(GPIO_PIN_10, GPIO_LOW);  //DC low

    for(i=0; i<8; i++)
    {
        app_m3_gpio_write_pin(GPIO_PIN_30, GPIO_LOW); /*scl*/
        __nop();
        __nop();

        if(value & 0x80)
            app_m3_gpio_write_pin(GPIO_PIN_31, GPIO_HIGH);
        else
            app_m3_gpio_write_pin(GPIO_PIN_31, GPIO_LOW);

        app_m3_gpio_write_pin(GPIO_PIN_30, GPIO_HIGH); /*scl*/
        __nop();
        __nop();

        //printf("%d[0x%x]  ", i, value);
        value =  value<<1;
    }


    app_m3_gpio_write_pin(GPIO_PIN_3, GPIO_HIGH);   //CS high

//   printf("cmd[0x%x] \n", command);
}


static void app_cobra_lcd_write_data(uint8_t data)
{
    uint8_t i,value;
    value = data;
    app_m3_gpio_write_pin(GPIO_PIN_3, GPIO_LOW);   //CS low
    app_m3_gpio_write_pin(GPIO_PIN_10, GPIO_HIGH);  //DC high

    for(i=0; i<8; i++)
    {
        app_m3_gpio_write_pin(GPIO_PIN_30, GPIO_LOW); /*scl*/
        __nop();
        __nop();

        if(value & 0x80)
            app_m3_gpio_write_pin(GPIO_PIN_31, GPIO_HIGH);
        else
            app_m3_gpio_write_pin(GPIO_PIN_31, GPIO_LOW);

        app_m3_gpio_write_pin(GPIO_PIN_30, GPIO_HIGH); /*scl*/

        __nop();
        __nop();

        value =  value<<1;
    }

    app_m3_gpio_write_pin(GPIO_PIN_3, GPIO_HIGH);   //CS high

//   printf("data[0x%x] ", data);
}

static void  SendDataSPI(unsigned char data)
{
    uint8_t i,value;

    for(i=0; i<8; i++)
    {
        app_m3_gpio_write_pin(GPIO_PIN_30, GPIO_LOW); /*scl*/
        __nop();
        __nop();

        if(value & 0x80)
            app_m3_gpio_write_pin(GPIO_PIN_31, GPIO_HIGH);
        else
            app_m3_gpio_write_pin(GPIO_PIN_31, GPIO_LOW);

        app_m3_gpio_write_pin(GPIO_PIN_30, GPIO_HIGH); /*scl*/

        __nop();
        __nop();

        value =  value<<1;
    }

}

#else
static void  SendDataSPI(unsigned char data)
{
    cobra_spi_read_write_word(SPI1, data);
}


static void app_cobra_lcd_write_command(uint8_t command)
{
    CS_ZERO_LOW;  //CS low
    DC_ZERO_LOW;  //DC low
    cobra_spi_read_write_word(SPI1, command);
    CS_ONE_HIGH;  //CS high
}

static void app_cobra_lcd_write_data(uint8_t data)
{
    CS_ZERO_LOW;  //CS low
    DC_ONE_HIGH;  //DC high
    cobra_spi_read_write_word(SPI1, data);
    CS_ONE_HIGH; //CS high
}

#endif



static void  app_cobra_ic_st7735s_init(void)
{

    /*reset pluse*/
    RESET_ONE_HIGH;
    app_cobra_lcd_delay(10);
    RESET_ZERO_LOW;
    app_cobra_lcd_delay(50);
    RESET_ONE_HIGH;
    app_cobra_lcd_delay(100);
    app_cobra_lcd_write_command(0x11);     //Sleep out

    app_cobra_lcd_delay(120);              //Delay 120ms

    app_cobra_lcd_write_command(0xB1);
    app_cobra_lcd_write_data(0x05);
    app_cobra_lcd_write_data(0x3C);
    app_cobra_lcd_write_data(0x3C);
    app_cobra_lcd_write_command(0xB2);
    app_cobra_lcd_write_data(0x05);
    app_cobra_lcd_write_data(0x3C);
    app_cobra_lcd_write_data(0x3C);

    app_cobra_lcd_write_command(0xB3);
    app_cobra_lcd_write_data(0x05);
    app_cobra_lcd_write_data(0x3C);
    app_cobra_lcd_write_data(0x3C);
    app_cobra_lcd_write_data(0x05);
    app_cobra_lcd_write_data(0x3C);
    app_cobra_lcd_write_data(0x3C);

    app_cobra_lcd_write_command(0xB4);     //Dot inversion
    app_cobra_lcd_write_data(0x03);

    app_cobra_lcd_write_command(0xC0);
    app_cobra_lcd_write_data(0x0E);
    app_cobra_lcd_write_data(0x0E);
    app_cobra_lcd_write_data(0x04);

    app_cobra_lcd_write_command(0xC1);
    app_cobra_lcd_write_data(0xC5);

    app_cobra_lcd_write_command(0xC2);
    app_cobra_lcd_write_data(0x0d);
    app_cobra_lcd_write_data(0x00);

    app_cobra_lcd_write_command(0xC3);
    app_cobra_lcd_write_data(0x8D);
    app_cobra_lcd_write_data(0x2A);

    app_cobra_lcd_write_command(0xC4);
    app_cobra_lcd_write_data(0x8D);
    app_cobra_lcd_write_data(0xEE);

    app_cobra_lcd_write_command(0xC5);     //VCOM
    app_cobra_lcd_write_data(0x06); //1D  .06

//app_cobra_lcd_write_command(0xC7);     //VCOM
//app_cobra_lcd_write_data(0x76); //1D  .06

    app_cobra_lcd_write_command(0x36);     //MX, MY, RGB mode
    app_cobra_lcd_write_data(0xc8);   	//c0

    app_cobra_lcd_write_command(0x3A);     //MX, MY, RGB mode
    app_cobra_lcd_write_data(0x55);   	//c0


    app_cobra_lcd_write_command(0xE0);
    app_cobra_lcd_write_data(0x0b);
    app_cobra_lcd_write_data(0x17);
    app_cobra_lcd_write_data(0x0a);
    app_cobra_lcd_write_data(0x0d);
    app_cobra_lcd_write_data(0x1a);
    app_cobra_lcd_write_data(0x19);
    app_cobra_lcd_write_data(0x16);
    app_cobra_lcd_write_data(0x1d);
    app_cobra_lcd_write_data(0x21);
    app_cobra_lcd_write_data(0x26);
    app_cobra_lcd_write_data(0x37);
    app_cobra_lcd_write_data(0x3c);
    app_cobra_lcd_write_data(0x00);
    app_cobra_lcd_write_data(0x09);
    app_cobra_lcd_write_data(0x05);
    app_cobra_lcd_write_data(0x10);

    app_cobra_lcd_write_command(0xE1);
    app_cobra_lcd_write_data(0x0c);
    app_cobra_lcd_write_data(0x19);
    app_cobra_lcd_write_data(0x09);
    app_cobra_lcd_write_data(0x0d);
    app_cobra_lcd_write_data(0x1b);
    app_cobra_lcd_write_data(0x19);
    app_cobra_lcd_write_data(0x15);
    app_cobra_lcd_write_data(0x1d);
    app_cobra_lcd_write_data(0x21);
    app_cobra_lcd_write_data(0x26);
    app_cobra_lcd_write_data(0x39);
    app_cobra_lcd_write_data(0x3E);
    app_cobra_lcd_write_data(0x00);
    app_cobra_lcd_write_data(0x09);
    app_cobra_lcd_write_data(0x05);
    app_cobra_lcd_write_data(0x10);

    /*************************************************/
#if 1
    app_cobra_lcd_write_command(0x36);
//	  app_cobra_lcd_write_data(0xC8);
    app_cobra_lcd_write_data(0x08);  /*??ת180*/
//	  app_cobra_lcd_write_data(0xA8);
//	  app_cobra_lcd_write_data(0x68);
#endif
    /*************************************************/

    app_cobra_lcd_write_command(0x3A);     //65k mode
    app_cobra_lcd_write_data(0x05);

    app_cobra_lcd_delay(120);
    app_cobra_lcd_write_command(0x29);     //Display on

}

void  app_cobra_lcd_st7735s_init(void)
{
    app_cobra_lcd_init();
    app_cobra_ic_st7735s_init();

    app_cobra_lcd_st7735s_test();

    LEDK_ONE_HIGH;

}

static void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend)
{

    app_cobra_lcd_write_command(0x2A); //Set Column Address
    app_cobra_lcd_write_data(0x00);
    app_cobra_lcd_write_data((Xstart+0x18)&0xFF);
//	app_cobra_lcd_write_data(0x18);
    app_cobra_lcd_write_data(0x00);
    app_cobra_lcd_write_data((Xend+0x18)&0xFF);
//	app_cobra_lcd_write_data(0x67);   //6A

    app_cobra_lcd_write_command(0x2B); //Set Page Address
    app_cobra_lcd_write_data(0x00);
    app_cobra_lcd_write_data(Ystart&0xFF);
//	app_cobra_lcd_write_data(0x00);
    app_cobra_lcd_write_data(0x00);
    app_cobra_lcd_write_data(Yend&0xFF);
//	app_cobra_lcd_write_data(0x9F);//9F

    app_cobra_lcd_write_command(0x2c);

}


static void app_cobra_lcd_st7735s_display_color(unsigned int color)
{
    unsigned int i,j;

    CS_ZERO_LOW;
    DC_ONE_HIGH;


    for(i=0; i<LCD_ROW; i++)
    {
        for(j=0; j<LCD_COL; j++)
        {
            SendDataSPI(color>>8);
            SendDataSPI(color);
        }
    }

    CS_ONE_HIGH;
}

void app_cobra_lcd_st7735s_disp_pic(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const unsigned char  *picture)
{
    const unsigned char *p;
    unsigned int  n;

    BlockWrite(x,x+w-1,y,y+h-1);

    CS_ZERO_LOW;
    DC_ONE_HIGH;


    p=picture;
    for(n=0; n<w*h*16/8; n++)       //n-row repeat count
    {
        SendDataSPI(*p);
        p++;
    }

    CS_ONE_HIGH;
}


void app_cobra_lcd_st7735s_write_one_dot(unsigned int color)
{
    CS_ZERO_LOW;
    DC_ONE_HIGH;

    SendDataSPI(color>>8);
    SendDataSPI(color);

    CS_ONE_HIGH;
}

void  app_cobra_lcd_st7735s_disp_one_char(unsigned int Xstart,unsigned int Ystart,unsigned char ord,unsigned int TextColor,unsigned int BackColor)	 // ord:0~95
{
    unsigned char i,j;
    unsigned char  *p;
    unsigned char dat;
    unsigned int index;
    unsigned int xOffset=0, yOffset=0;

    if(BackColor != TRANSPARENT_COLOR)
    {
        BlockWrite(Xstart,Xstart+(FONT_W-1),Ystart,Ystart+(FONT_H-1));
    }

    index = ord - ' ';  //?õ?ƫ?ƺ???ֵ;

//   printf("index[%d], ord[%d]\n", index, ord);

    if(index>94)
        index=94;

    for(i=0; i<(FONT_W/8*FONT_H); i++)
    {
        dat=asc2_1608[index][i];

        for(j=0; j<8; j++)
        {
            if(BackColor == TRANSPARENT_COLOR)
            {
                BlockWrite(Xstart+xOffset,Xstart+xOffset,Ystart+yOffset,Ystart+yOffset);
            }

            if((dat<<j)&0x80)
            {
                app_cobra_lcd_st7735s_write_one_dot(TextColor);
            }
            else
            {
                if(BackColor != TRANSPARENT_COLOR)
                {
                    app_cobra_lcd_st7735s_write_one_dot(BackColor);
                }
            }

            if(BackColor == TRANSPARENT_COLOR)
            {
                xOffset++;
                if(xOffset==FONT_W) /*next line*/
                {
                    xOffset = 0;
                    yOffset++;
                }
            }
        }
    }
}

void app_cobra_lcd_st7735s_disp_string(unsigned int Xstart,unsigned int Ystart,unsigned char *str,unsigned int TextColor,unsigned int BackColor)
{
    unsigned char temp_ch;

    for (; *str; str++)
    {
        temp_ch = *str;


        if((temp_ch == '\n' ) || (temp_ch == '\r'))
        {
            Xstart=0;
            Ystart=Ystart+FONT_H;

            continue;
        }

        if(temp_ch == 0) break;

        if(Xstart>((LCD_COL)-FONT_W))
        {
            Xstart=0;
            Ystart=Ystart+FONT_H;
        }

        if(Ystart>((LCD_ROW)-FONT_H))
        {
            Ystart=0;
        }

        app_cobra_lcd_st7735s_disp_one_char(Xstart,Ystart,temp_ch,TextColor,BackColor);


        if(Xstart<=((LCD_COL)-FONT_W))
        {
            Xstart=Xstart+FONT_W;
        }

    }
    BlockWrite(0,LCD_COL-1,0,LCD_ROW-1);
}

static void app_cobra_lcd_st7735s_test(void)
{
    BlockWrite(0,LCD_COL-1,0,LCD_ROW-1);
    app_cobra_lcd_st7735s_display_color(BLACK_COLOR);  /*black*/

    app_cobra_lcd_st7735s_disp_string(0, 0, "1234567890\nABCDEFGHIJKLMN\nabcdefghijklmn\n!@#$%^&*();:<>\n", RED_COLOR, WHITE_COLOR);

    app_cobra_lcd_st7735s_disp_pic(0,50,80, 80, gImage_bike_80_80);

}

#endif
