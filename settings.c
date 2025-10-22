//settings.c
#include "settings.h"
#include "lcd_defines.h"
#include "lcd.h"
#include "delay.h"
#include "types.h"
#include "kpm.h"
#include <lpc21xx.h>
#include <stdlib.h>

u32 PSW=818;
s32 UserPSW;
u8 choice;
extern u32 msg_flag;


typedef struct
{
				u8 hour;
				u8 minute;
				char text[80];
				u8 enabled; // 1 = show, 0 = skip (user controlled)
} Message;

extern Message messageList[10];
extern s8 hour,min,sec,dom,month,year,dow;

void settings()
{
				u8 flag=1;
				CmdLCD(CLEAR_LCD);
				StrLCD("  ADMIN ");
				CmdLCD(GOTO_LINE2_POS0);
				StrLCD("        MODE");
				delay_s(1);
	b:CmdLCD(CLEAR_LCD);
				StrLCD("ENTER PASSWORD");
				UserPSW = ReadNum2(10);
				if(UserPSW==-1)
				{
						CmdLCD(CLEAR_LCD);
						StrLCD("INVALID");
						delay_s(1);
						goto b;
				}
				if(UserPSW==PSW)
				{
								while(1)
								{
										a: CmdLCD(CLEAR_LCD);
												CmdLCD(GOTO_LINE1_POS0);
												StrLCD("1.Time 2.Date");
												CmdLCD(GOTO_LINE2_POS0);
												StrLCD("3.day 4.Msg 5.Ex");
												choice = Keyscan();
												switch(choice)
												{
																case '1':Edit_time();
																				 break;
																case '2':Edit_date();
																				 break;
																case '3':DOW = Edit_day();
																				 break;
																case '4':MsgDisp();
																				 break;
																case '5':flag=0;
																				 break;
																default :CmdLCD(CLEAR_LCD);
																StrLCD("Invalid input");
																delay_s(2);
																goto a;
												}
												if(flag==0) break;
								}
				}
				else
				{
								CmdLCD(CLEAR_LCD);
								StrLCD("INVALID PASSWORD");
								delay_s(2);
								goto b;
				}
}

void Edit_time()
{
				while(1)
				{
								u8 ch,flag=1;
								CmdLCD(CLEAR_LCD);
								StrLCD("1.Hr 2.Min 3.Sec");
								CmdLCD(GOTO_LINE2_POS0);
								StrLCD("4.Back");
								ch=Keyscan();
								switch(ch)
								{
												case '1': HOUR = Edit_hour();
																	break;
												case '2': MIN  = Edit_min();
																	break;
												case '3': SEC  = Edit_sec();
																	break;
												case '4': flag=0;
																	break;
												default : StrLCD("  Invalid");
																	delay_s(2);
								}
								if(flag==0)
								{
												break;
								}
				}
}

s8 Edit_hour()
{
				s32 h;
				while(1)
				{
								CmdLCD(CLEAR_LCD);
								StrLCD("ENTER HOUR(0-23)");
								CmdLCD(GOTO_LINE2_POS0);
								h=ReadNum2(2);
								if(h==-1)
								{
												StrLCD("  Invalid");
												delay_s(2);
								}
								else if(h>23)
								{
												StrLCD("  Invalid");
												delay_s(2);
								}
								else
								{
												return h;
								}
				}
}
s8 Edit_min()
{
				s32 m;
				while(1)
				{
								CmdLCD(CLEAR_LCD);
								StrLCD("ENTER Mins(0-59)");
								CmdLCD(GOTO_LINE2_POS0);
								m=ReadNum2(2);
								if(m == -1)
								{
												StrLCD(" Invalid");
												delay_s(2);
								}
								else if(m>59)
								{
												StrLCD(" Invalid");
												delay_s(2);
								}
								else
								{
												return m;
								}
				}
}
s8 Edit_sec()
{
        s32 s;
        while(1)
        {
								CmdLCD(CLEAR_LCD);
								StrLCD("Enter secs(0-59)");
								CmdLCD(GOTO_LINE2_POS0);
								s=ReadNum2(2);
								if(s==-1)
								{
												StrLCD("  Invalid");
												delay_s(2);
								}
								else if(s>59)
								{
												StrLCD("   Invalid");
												delay_s(1);
								}
								else
								{
												return s;
								}
        }
}

void Edit_date()
{
        s8 choice,flag=1;
        while(1)
        {
								CmdLCD(CLEAR_LCD);
								StrLCD("1.Date 2.Month");
								CmdLCD(GOTO_LINE2_POS0);
								StrLCD("3.Year 4.Back");
								choice=Keyscan();
								switch(choice)
								{
												case '1':	DOM = Edit_Ndate();
																	break;
												case '2':	MONTH = Edit_month();
																	break;
												case '3':	YEAR = Edit_year();
																	break;
												case '4':	flag=0;
																	break;
												default :CmdLCD(CLEAR_LCD);
												StrLCD(" Invalid ");
												delay_s(2);
								}
								if(flag==0)
												break;
        }
}
s8 Edit_Ndate()
{
        s32 d;
        while(1)
        {
								CmdLCD(CLEAR_LCD);
								StrLCD("ENTER DATE(1-31)");
								d=ReadNum2(2);
								if(d==-1)
								{
												StrLCD("  Invalid");
												delay_s(2);
								}
								else if(d==0 || d>31)
								{
												StrLCD("  Invalid");
												delay_s(2);
								}
								else
								{
												return d;
								}
        }
}
s8 Edit_month()
{
        s32 m;
        while(1)
        {
								CmdLCD(CLEAR_LCD);
								StrLCD("ENTER MONTH:1-12");
								m=ReadNum2(2);
								if(m == -1)
								{
												StrLCD("  Invalid");
												delay_s(2);
								}
								else if(m==0 || m>12)
								{
												StrLCD("   Invalid");
												delay_s(2);
								}
								else
								{
												return m;
								}
        }
}
s32 Edit_year()
{
        s32 y;
        while(1)
        {
								CmdLCD(CLEAR_LCD);
								StrLCD("ENTER (2025-2999)");
								y=ReadNum2(4);
								if(y==-1)
								{
												StrLCD("  Invalid");
												delay_s(2);
								}
								else if(y<2025 || y>2999)
								{
												StrLCD("   Invalid");
												delay_s(2);
								}
								else
								{

												return y;
								}
        }
}
s8 Edit_day()
{
        s8 d;
        while(1)
        {
								CmdLCD(CLEAR_LCD);
								StrLCD("0.Su 1.M 2.T 3.W");
								CmdLCD(GOTO_LINE2_POS0);
								StrLCD("4.Th 5.Fr 6.Sa7E");
								d=Keyscan()-48;
								if(d>6)
								{
												CmdLCD(CLEAR_LCD);
												StrLCD("Invalid");
												delay_s(2);
								}
								else
								{
												return d;
								}
        }
}

void MsgDisp()
{
        s32 ch,n;
        while(1)
        {
								u8 flag=1;
				a:CmdLCD(CLEAR_LCD);
								StrLCD("Enter Num:1-10");
								ch=ReadNum2(2);
								if(ch<=0 || ch>10)
								{
												StrLCD(" Invalid");
												delay_s(2);
												goto a;
								}
								//delay_ms(500);
								CmdLCD(CLEAR_LCD);
								StrLCD("1.Enable 2.Disable");
								n=Keyscan();
								switch(n)
								{
												case '1': messageList[ch-1].enabled=1;
																	msg_flag=1;
																	flag=0;
																	CmdLCD(GOTO_LINE2_POS0);
																	StrLCD("Enabled");
																	delay_ms(500);
																	break;
												case '2': messageList[ch-1].enabled=0;
																	msg_flag=0;
																	flag=0;
																	CmdLCD(GOTO_LINE2_POS0);
																	StrLCD("Disabled");
																	delay_ms(500);
																	break;
												default : StrLCD("Invalid");
																	delay_s(2);
								}
								if(flag==0)
												break;
				}
}

