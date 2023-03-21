/*************************************************
 	lcd.h: Header file for 16x2 LCD interfacing  
**************************************************/

//Functions contained in this header file																	//fn takes integer value as an input and generates corresponding delay in milli seconds
void lcd_init(void);																				//Initialize LCD
void lcd_cmd(unsigned int i);																//Sends commands to lcd
void lcd_char(unsigned char ch);														//display character on a lcd corresponding to input ascii
void lcd_write_string(unsigned char *s);										//takes pointer of a string which ends with null and display on a lcd  
void lcd_write_char(unsigned char ch);
void int_to_string(unsigned int val,unsigned char *temp_str_data); // utility to convert int to string.


//Function definitions
/************************************************
   lcd_init():
	Initializes LCD port and 
	LCD display parameters
************************************************/
void lcd_init(void)
{
	PORTC_DIRSET = 0xFF;
	PORTD_DIRSET = 0xFF;

	PORTC_OUT = 0x00;
	PORTD_OUT = 0x00;
	
	lcd_cmd(0x38);	// Function set: 2 Line, 8-bit, 5x7 dots
	_delay_ms(4);
	lcd_cmd(0x06);	// Entry mode, auto increment with no shift
	_delay_ms(4);
	lcd_cmd(0x0E);	// Display on, Cursor on
	_delay_ms(4);
	lcd_cmd(0x01);	// LCD clear
	_delay_ms(4);
	lcd_cmd(0x80);	//Move cursor to Row 1 column 0
}

/**********************************************************
   msdelay(<time_val>): 
	Delay function for delay value <time_val>ms
***********************************************************/	


/**********************************************************
    lcd_cmd(<char command>):
	Sends 8 bit command
	LCD display parameters
***********************************************************/	
void lcd_cmd(unsigned int i)
{
	PORTD_OUT = 0x04;
	PORTC_OUT = i;
	_delay_ms(10);
	PORTD_OUT = 0x00;	
}


/*********************************************************
   lcd_write_char(<char data>):
	Sends 8 bit character(ASCII)
	to be printed on LCD
**********************************************************/
void lcd_write_char(unsigned char ch)
{
	PORTD_OUT = 0x05;
	PORTC_OUT = ch;
	_delay_ms(10);
	PORTD_OUT = 0x01;
}


/***********************************************************
  lcd_write_string(<string pointer>):
	Prints string on LCD. Requires string pointer 
	as input argument.
**********************************************************/
void lcd_write_string(unsigned char *s)
{
	while(*s!='\0')
	{
		lcd_write_char(*s++);
	}
}

void int_to_string(unsigned int val,unsigned char *temp_str_data)
{
	// char str_data[4]=0;
	temp_str_data[0]=48+(val/10000);
	temp_str_data[1]=48+(val%10000/1000);
	temp_str_data[2]=48+((val%1000)/100);
	temp_str_data[3]=48+((val%100)/10);
	temp_str_data[4]=48+(val%10);
	// return str_data;
}
