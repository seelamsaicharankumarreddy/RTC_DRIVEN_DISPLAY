#include <lpc21xx.h>
#include <string.h>
#include "delay.h"
#include "types.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "kpm.h"
#include "rtc.h"
#include "pin_connect_block.h"
#include "interrupts_defines.h"
#include "settings.h"
#include "adc.h"

#define TOTAL_MESSAGES 10
#define LED_RED 25
#define LED_GREEN 26
#define LCD_WIDTH 16

s32 hour, min, sec, date, month, year, day;
u32 msg_flag = 1;
u8 len;
s32 i = 0, startHour, startMin;

typedef struct
{
	u8 hour;
	u8 minute;
	char text[80];
	u8 enabled; // 1 = show, 0 = skip (user controlled)
}Message;

// function declarations
void msg_scroll(const char *, u32, u32);
int find_strlen(const char *);
void eint0_isr(void) __irq;

Message messageList[TOTAL_MESSAGES] = {
				{7, 45,  "               Good Morning! Classes Start Soon ", 1},
				{13, 45, "               C Programming Session in Class room number 2 ", 1},
				{10, 15, "               C module theory exam in 4th floor lab1 ", 1},
				{10, 15, "               C module lab exam in 4th floor lab2 and middle lab at 10:30AM ", 1},
				{12, 45, "               Lunch Break from 1PM - 2PM ", 1},
				{9, 45,  "               ARM Workshop on external interrupts in LAB1 at 10AM ", 1},
				{9, 45,  "               ARM kit issue time from 10AM - 10:30AM in middle lab ", 1},
				{15, 15, "               Only 15 mins break time for next ARM session ", 1},
				{17, 00, "               Revise today's Class programs at home! ", 1},
				{17, 45, "               End of Day - See You Tomorrow! ", 1}
};

int main()
{
	//s32 i = 0, startHour, startMin;

	InitLCD();
	InitKPM();
	RTC_Init();
	Init_ADC();

	IODIR1 |= 3 << LED_RED;  // Configure LED pins as output

	// Set initial RTC values
	SetRTCTimeInfo(07, 44, 55);
	SetRTCDateInfo(22, 10, 2025);
	SetRTCDay(2);

	// Configure external interrupt on P0.1
	cfgPortPin(0, 1, EINT0_PIN_FUNC);
	VICIntSelect = 0;
	VICIntEnable = 1 << EINT0_VIC_CHNO;
	VICVectCntl0 = (1 << 5) | EINT0_VIC_CHNO;
	VICVectAddr0 = (u32)eint0_isr;
	EXTMODE = 1 << 0;

	while (1)
	{
		msg_flag = 1;
		IOSET1 = 1 << LED_RED;
		IOCLR1 = 1 << LED_GREEN;

		CmdLCD(GOTO_LINE1_POS0);
		GetRTCTimeInfo(&hour, &min, &sec);
		DisplayRTCTime(hour, min, sec);
		GetRTCDateInfo(&date, &month, &year);
		DisplayRTCDate(date, month, year);
		GetRTCDay(&day);
		DisplayRTCDay(day);

		CmdLCD(GOTO_LINE2_POS0 + 11);
		U32LCD(Read_LM35());
		CharLCD(223);
		CharLCD('C');

		// Check if a message is scheduled for current time
		for (i = 0; i < TOTAL_MESSAGES; i++)
		{
			if (messageList[i].hour == hour && messageList[i].minute == min && messageList[i].enabled == 1)
			{
				startHour = hour;
				startMin = min;

				// Start scrolling the message
				while (1)
				{
					IOSET1 = 1 << LED_GREEN;
					IOCLR1 = 1 << LED_RED;

					GetRTCTimeInfo(&hour, &min, &sec);
					/*if (((hour * 60 + min) - (startHour * 60 + startMin)) > 0 || msg_flag == 0)
							break;*/
					if ((((hour * 60 + min) - (startHour * 60 + startMin)) > 0) || (((hour * 60 + min) - (startHour * 60 + startMin)) < 0))
									break;
					if(msg_flag==0)
									break;

					len = find_strlen(messageList[i].text);
								msg_scroll(messageList[i].text, len, 200);  // Very smooth scrolling
				}
				CmdLCD(CLEAR_LCD);
			}
		}
	}
}

void msg_scroll(const char *p, u32 size, u32 timems)
{
	char window[LCD_WIDTH + 1];
	u32 i, j;

	// Scroll entire message plus LCD width spaces at the end
	for (i = 0; i <= size; i++)
	{
		for (j = 0; j < LCD_WIDTH; j++)
		{
			GetRTCTimeInfo(&hour, &min, &sec);
			if ((i + j) < size)
					window[j] = p[i + j];
			else
					window[j] = ' ';  // pad with spaces at end
		}
		window[LCD_WIDTH] = '\0';  // null terminate

		CmdLCD(GOTO_LINE1_POS0);
		StrLCD(window);

		CmdLCD(GOTO_LINE2_POS0);
		StrLCD("  EVENT BOARD   ");

		delay_ms(timems);


		if ((((hour * 60 + min) - (startHour * 60 + startMin)) > 0) || (((hour * 60 + min) - (startHour * 60 + startMin)) < 0))
						break;
		if(msg_flag==0)
						break;
	}
}


int find_strlen(const char *p)
{
	u32 i = 0;
	while (p[i]) i++;
	return i;
}

void eint0_isr(void) __irq
{
	IOCLR1 = 1 << LED_GREEN;
	IOCLR1 = 1 << LED_RED;
	settings();
	CmdLCD(CLEAR_LCD);
	EXTINT = 1 << 0; // clear EINT0 status flag
	VICVectAddr = 0;
}

