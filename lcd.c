#include "avr.h"
#include "lcd.h"

#define DDR    DDRB
#define PORT   PORTB
#define RS_PIN 0
#define RW_PIN 1
#define EN_PIN 2

// Set 1s for output in DDRD, Set the port used (D) to the character and its' Registers on the microcontroller's port D
static inline void
set_data(unsigned char x)
{
	PORTD = x;
	DDRD = 0xff;
}

//Need to set to 0s to get data for the DDRD, then return data
static inline unsigned char
get_data(void)
{
	DDRD = 0x00;
	return PIND;
}

//Sleep - simple function
static inline void
sleep_700ns(void)
{
	NOP();
	NOP();
	NOP();
}

//Input as programming instructions or input as # computer in programming instructions
static unsigned char
input(unsigned char rs)
{
	unsigned char d;
	if (rs) SET_BIT(PORT, RS_PIN); else CLR_BIT(PORT, RS_PIN);
	SET_BIT(PORT, RW_PIN);
	get_data();
	SET_BIT(PORT, EN_PIN);
	sleep_700ns();
	d = get_data();
	CLR_BIT(PORT, EN_PIN);
	return d;
}

//Output, can be just output
static void
output(unsigned char d, unsigned char rs)
{
	if (rs) SET_BIT(PORT, RS_PIN); else CLR_BIT(PORT, RS_PIN);
	CLR_BIT(PORT, RW_PIN);
	set_data(d);
	SET_BIT(PORT, EN_PIN);
	sleep_700ns();
	CLR_BIT(PORT, EN_PIN);
}

static void
write(unsigned char c, unsigned char rs)
{
	while (input(0) & 0x80);
	output(c, rs);
}

//Initialize with the avrs, make sure everything is working, initialize the data bus for the LCD
void
lcd_init(void)
{
	SET_BIT(DDR, RS_PIN);
	SET_BIT(DDR, RW_PIN);
	SET_BIT(DDR, EN_PIN);
	avr_wait(16);
	output(0x30, 0);
	avr_wait(5);
	output(0x30, 0);
	avr_wait(1);
	write(0x3c, 0);
	write(0x0c, 0);
	write(0x06, 0);
	write(0x01, 0);
}

void
lcd_clr(void)
{
	write(0x01, 0);
}

void
lcd_pos(unsigned char r, unsigned char c)
{
	unsigned char n = r * 40 + c;
	write(0x02, 0);
	while (n--) {
		write(0x14, 0);
	}
}

void
lcd_put(char c)
{
	write(c, 1);
}

void
lcd_puts1(const char *s)
{
	char c;
	while ((c = pgm_read_byte(s++)) != 0) {
		write(c, 1);
	}
}

//Use this
void
lcd_puts2(const char *s)
{
	char c;
	while ((c = *(s++)) != 0) {
		write(c, 1);
	}
}