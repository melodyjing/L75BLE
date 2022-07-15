/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	hid_key.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	  2020/8/31

	Description :   Key µÄ´¦Àí
	Changed Log	:

		Liujian  2020/8/31		- Creation
*****************************************************************************/

#include "hid_key.h"
#include "app_button.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"
#include "app_hid.h"
#if (BLE_APP_HID_ENABLE)

static struct Button cobra_key[8];

/**
 * Get the Key code for specified character
 * @param character ASCII character code
 * @return key code of US Keyboard
 */
uint8_t cobra_get_key_code(uint8_t character)
{
    switch (character)
    {
    case 'A':
    case 'a':
        return 0x04;
    case 'B':
    case 'b':
        return 0x05;
    case 'C':
    case 'c':
        return 0x06;
    case 'D':
    case 'd':
        return 0x07;
    case 'E':
    case 'e':
        return 0x08;
    case 'F':
    case 'f':
        return 0x09;
    case 'G':
    case 'g':
        return 0x0a;
    case 'H':
    case 'h':
        return 0x0b;
    case 'I':
    case 'i':
        return 0x0c;
    case 'J':
    case 'j':
        return 0x0d;
    case 'K':
    case 'k':
        return 0x0e;
    case 'L':
    case 'l':
        return 0x0f;
    case 'M':
    case 'm':
        return 0x10;
    case 'N':
    case 'n':
        return 0x11;
    case 'O':
    case 'o':
        return 0x12;
    case 'P':
    case 'p':
        return 0x13;
    case 'Q':
    case 'q':
        return 0x14;
    case 'R':
    case 'r':
        return 0x15;
    case 'S':
    case 's':
        return 0x16;
    case 'T':
    case 't':
        return 0x17;
    case 'U':
    case 'u':
        return 0x18;
    case 'V':
    case 'v':
        return 0x19;
    case 'W':
    case 'w':
        return 0x1a;
    case 'X':
    case 'x':
        return 0x1b;
    case 'Y':
    case 'y':
        return 0x1c;
    case 'Z':
    case 'z':
        return 0x1d;
    case '!':
    case '1':
        return 0x1e;
    case '@':
    case '2':
        return 0x1f;
    case '#':
    case '3':
        return 0x20;
    case '$':
    case '4':
        return 0x21;
    case '%':
    case '5':
        return 0x22;
    case '^':
    case '6':
        return 0x23;
    case '&':
    case '7':
        return 0x24;
    case '*':
    case '8':
        return 0x25;
    case '(':
    case '9':
        return 0x26;
    case ')':
    case '0':
        return 0x27;
    case '\n': // LF
        return 0x28;
    case '\b': // BS
        return 0x2a;
    case '\t': // TAB
        return 0x2b;
    case ' ':
        return 0x2c;
    case '_':
    case '-':
        return 0x2d;
    case '+':
    case '=':
        return 0x2e;
    case '{':
    case '[':
        return 0x2f;
    case '}':
    case ']':
        return 0x30;
    case '|':
    case '\\':
        return 0x31;
    case ':':
    case ';':
        return 0x33;
    case '"':
    case '\'':
        return 0x34;
    case '~':
    case '`':
        return 0x35;
    case '<':
    case ',':
        return 0x36;
    case '>':
    case '.':
        return 0x37;
    case '?':
    case '/':
        return 0x38;
    default:
        return 0;
    }
}

/**
 * Get the modifier key combination for specified character
 * @param character ASCII character code
 * @return modifier key combination
 */
Modifier cobra_get_modifier(uint8_t character)
{
    switch (character)
    {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '!':
    case '@':
    case '#':
    case '$':
    case '%':
    case '^':
    case '&':
    case '*':
    case '(':
    case ')':
    case '_':
    case '+':
    case '{':
    case '}':
    case '|':
    case ':':
    case '"':
    case '~':
    case '<':
    case '>':
    case '?':
        return MODIFIER_LEFT_SHIFT;
    default:
        return MODIFIER_NONE;
    }
}


unsigned char read_cobra_key_0() 
{
	unsigned char key_value;
	key_value = app_m3_gpio_read_pin(GPIO_PIN_27);
	return key_value;
}

unsigned char read_cobra_key_1() 
{
	unsigned char key_value;
	key_value = app_m3_gpio_read_pin(GPIO_PIN_28);
	return key_value;
}

unsigned char read_cobra_key_2() 
{
	unsigned char key_value;
	key_value = app_m3_gpio_read_pin(GPIO_PIN_29);
	return key_value;
}

unsigned char read_cobra_key_3() 
{
	unsigned char key_value;
	key_value = app_m3_gpio_read_pin(GPIO_PIN_4);
	return key_value;
}

unsigned char read_cobra_key_4() 
{
	unsigned char key_value;
	key_value = app_m3_gpio_read_pin(GPIO_PIN_5);
	return key_value;
}

unsigned char read_cobra_key_5() 
{
	unsigned char key_value;
	key_value = app_m3_gpio_read_pin(GPIO_PIN_6);
	return key_value;
}

unsigned char read_cobra_key_6() 
{
	unsigned char key_value;
	key_value = app_m3_gpio_read_pin(GPIO_PIN_7);
	return key_value;
}

unsigned char read_cobra_key_7() 
{
	unsigned char key_value;
	key_value = app_m3_gpio_read_pin(GPIO_PIN_8);
	return key_value;
}


static void app_key_0_key_down_press(void * param)
{
	   unsigned char test[1];
     if(app_hid_key_state_is_ready())
		 {
			    test[0] = KEY_RIGHT_ARROW;
			    app_hid_report_key(test, 1, 0);		
		 }
}
static void app_key_1_key_down_press(void * param)
{
	   unsigned char test[1];
     if(app_hid_key_state_is_ready())
		 {
			    test[0] = KEY_LEFT_ARROW;
			    app_hid_report_key(test, 1, 0);		
		 }
}
static void app_key_2_key_down_press(void * param)
{
	   unsigned char test[1];
     if(app_hid_key_state_is_ready())
		 {
			    test[0] = cobra_get_key_code('S');
			    app_hid_report_key(test, 1, 0);		
		 }
}
static void app_key_3_key_down_press(void * param)
{
	   unsigned char test[1];
     if(app_hid_key_state_is_ready())
		 {
			    test[0] = cobra_get_key_code('D');
			    app_hid_report_key(test, 1, 0);		
		 }
}
static void app_key_4_key_down_press(void * param)
{
	   unsigned char test[1];
     if(app_hid_key_state_is_ready())
		 {
			    test[0] = cobra_get_key_code('X');
			    app_hid_report_key(test, 1, 0);		
		 }
}
static void app_key_5_key_down_press(void * param)
{
	   unsigned char test[1];
     if(app_hid_key_state_is_ready())
		 {
			    test[0] = cobra_get_key_code('Z');
			    app_hid_report_key(test, 1, 0);		
		 }
}
static void app_key_6_key_down_press(void * param)
{
	   unsigned char test[1];
     if(app_hid_key_state_is_ready())
		 {
			    test[0] = KEY_ENTER;
			    app_hid_report_key(test, 1, 0);		
		 }
}

static void app_key_7_key_down_press(void * param)
{
	   unsigned char test[1];
     if(app_hid_key_state_is_ready())
		 {
			    test[0] = cobra_get_key_code('M');
			    app_hid_report_key(test, 1, 0);		
		 }
}

void app_hid_button_init(void)
{
//		app_m3_gpio_set_direction(GPIO_PIN_27, GPIO_INPUT);
	  app_m3_gpio_set_direction(GPIO_PIN_28, GPIO_INPUT);
	  app_m3_gpio_set_direction(GPIO_PIN_29, GPIO_INPUT);
	  app_m3_gpio_set_direction(GPIO_PIN_4, GPIO_INPUT);
	  app_m3_gpio_set_direction(GPIO_PIN_5, GPIO_INPUT);
	  app_m3_gpio_set_direction(GPIO_PIN_6, GPIO_INPUT);
//	  app_m3_gpio_set_direction(GPIO_PIN_7, GPIO_INPUT);
	  app_m3_gpio_set_direction(GPIO_PIN_8, GPIO_INPUT);
	
//		cobra_pin_pullup_enable(PIN27_INDEX, 1);
	  cobra_pin_pullup_enable(PIN28_INDEX, 1);
	  cobra_pin_pullup_enable(PIN29_INDEX, 1);
	  cobra_pin_pullup_enable(PIN4_INDEX, 1);
	  cobra_pin_pullup_enable(PIN5_INDEX, 1);
	  cobra_pin_pullup_enable(PIN6_INDEX, 1);
	  cobra_pin_pullup_enable(PIN7_INDEX, 1);
	  cobra_pin_pullup_enable(PIN8_INDEX, 1);
	
	
//		button_init(&cobra_key[0], read_cobra_key_0, GPIO_LOW);
	  button_init(&cobra_key[1], read_cobra_key_1, GPIO_LOW);
		button_init(&cobra_key[2], read_cobra_key_2, GPIO_LOW);
		button_init(&cobra_key[3], read_cobra_key_3, GPIO_LOW);
		button_init(&cobra_key[4], read_cobra_key_4, GPIO_LOW);
		button_init(&cobra_key[5], read_cobra_key_5, GPIO_LOW);
		button_init(&cobra_key[6], read_cobra_key_6, GPIO_LOW);
//		button_init(&cobra_key[7], read_cobra_key_7, GPIO_LOW);
		
//	  button_attach(&cobra_key[0], PRESS_DOWN, app_key_0_key_down_press);
		button_attach(&cobra_key[1], PRESS_DOWN, app_key_1_key_down_press);
		button_attach(&cobra_key[2], PRESS_DOWN, app_key_2_key_down_press);
		button_attach(&cobra_key[3], PRESS_DOWN, app_key_3_key_down_press);
		button_attach(&cobra_key[4], PRESS_DOWN, app_key_4_key_down_press);
		button_attach(&cobra_key[5], PRESS_DOWN, app_key_5_key_down_press);
		button_attach(&cobra_key[6], PRESS_DOWN, app_key_6_key_down_press);
//		button_attach(&cobra_key[7], PRESS_DOWN, app_key_7_key_down_press);
		
//  	button_start(&cobra_key[0]);
	  button_start(&cobra_key[1]);
		button_start(&cobra_key[2]);
		button_start(&cobra_key[3]);
		button_start(&cobra_key[4]);
		button_start(&cobra_key[5]);
		button_start(&cobra_key[6]);
//		button_start(&cobra_key[7]);
	
}


#endif

