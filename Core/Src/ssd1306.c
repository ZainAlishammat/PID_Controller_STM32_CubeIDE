#include "stm32f3xx.h"
#include "ssd1306.h"


static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
static SSD1306_t SSD1306;

void WriteMem(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{


	I2C1->CR2 |= (DevAddress<<1);				//Slave Adress for I2C Display
	I2C1->CR2 &= ~(0xFF<<16);					//clear NBYTES
	I2C1->CR2 |= ((Size+1)<<16);				//max 255 Bytes of Data
	I2C1->CR2 |= I2C_CR2_AUTOEND;				//Stop condition automatically after transfer
	I2C1->CR2 |= I2C_CR2_START;					//Start

	while((I2C1->ISR & I2C_ISR_TXIS)==0);
	I2C1->TXDR = 0x40;

	for ( int cnt = 0; cnt < Size; cnt++)
	{
		while((I2C1->ISR & I2C_ISR_TXIS)==0);
		I2C1->TXDR = pData[cnt];


	}

}

void WriteCommand(uint16_t DevAddress, uint8_t command)
{
	I2C1->CR2 |= (DevAddress<<1);			//Slave Adress for I2C Display
	I2C1->CR2 &= ~(0xFF<<16);					//clear NBYTES
	I2C1->CR2 |= (2<<16);						//max 255 Bytes of Data
	I2C1->CR2 |= I2C_CR2_AUTOEND;				//Stop condition automatically after transfer
	I2C1->CR2 |= I2C_CR2_START;					//Start

	while((I2C1->ISR & I2C_ISR_TXIS)==0);
	I2C1->TXDR = 0x00;
	while((I2C1->ISR & I2C_ISR_TXIS)==0);
	I2C1->TXDR = command;

}



uint8_t ssd1306_Init(void)
{	
	/* Init LCD */
	WriteCommand(SSD1306_I2C_ADDR,0xAE); //display off
	WriteCommand(SSD1306_I2C_ADDR,0x20); //Set Memory Addressing Mode
	WriteCommand(SSD1306_I2C_ADDR,0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCommand(SSD1306_I2C_ADDR,0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	WriteCommand(SSD1306_I2C_ADDR,0xC8); //Set COM Output Scan Direction
	WriteCommand(SSD1306_I2C_ADDR,0x00); //---set low column address
	WriteCommand(SSD1306_I2C_ADDR,0x10); //---set high column address
	WriteCommand(SSD1306_I2C_ADDR,0x40); //--set start line address
	WriteCommand(SSD1306_I2C_ADDR,0x81); //--set contrast control register
	WriteCommand(SSD1306_I2C_ADDR,0xFF);
	WriteCommand(SSD1306_I2C_ADDR,0xA1); //--set segment re-map 0 to 127
	WriteCommand(SSD1306_I2C_ADDR,0xA6); //--set normal display
	WriteCommand(SSD1306_I2C_ADDR,0xA8); //--set multiplex ratio(1 to 64)
	WriteCommand(SSD1306_I2C_ADDR,0x3F); //
	WriteCommand(SSD1306_I2C_ADDR,0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCommand(SSD1306_I2C_ADDR,0xD3); //-set display offset
	WriteCommand(SSD1306_I2C_ADDR,0x00); //-not offset
	WriteCommand(SSD1306_I2C_ADDR,0xD5); //--set display clock divide ratio/oscillator frequency
	WriteCommand(SSD1306_I2C_ADDR,0xF0); //--set divide ratio
	WriteCommand(SSD1306_I2C_ADDR,0xD9); //--set pre-charge period
	WriteCommand(SSD1306_I2C_ADDR,0x22); //
	WriteCommand(SSD1306_I2C_ADDR,0xDA); //--set com pins hardware configuration
	WriteCommand(SSD1306_I2C_ADDR,0x12);
	WriteCommand(SSD1306_I2C_ADDR,0xDB); //--set vcomh
	WriteCommand(SSD1306_I2C_ADDR,0x20); //0x20,0.77xVcc
	WriteCommand(SSD1306_I2C_ADDR,0x8D); //--set DC-DC enable
	WriteCommand(SSD1306_I2C_ADDR,0x14); //
	WriteCommand(SSD1306_I2C_ADDR,0xAF); //--turn on SSD1306 panel
	
	ssd1306_Fill(Black);
	
	/* Update screen */
	ssd1306_UpdateScreen();
	
	/* Set default values */
	SSD1306.CurrentX = 0;
	SSD1306.CurrentY = 0;
	
	/* Initialized OK */
	SSD1306.Initialized = 1;
	
	/* Return OK */
	return 1;
}

void ssd1306_Fill(SSD1306_COLOR color) 
{
	uint32_t i;

	for(i = 0; i < sizeof(SSD1306_Buffer); i++)
	{
		SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
	}
}

void ssd1306_UpdateScreen(void) 
{
	uint8_t i;
	
	for (i = 0; i < 8; i++) {
		WriteCommand(SSD1306_I2C_ADDR,0xB0 + i);
		WriteCommand(SSD1306_I2C_ADDR,0x00);
		WriteCommand(SSD1306_I2C_ADDR,0x10);

		WriteMem(SSD1306_I2C_ADDR , 0x40, &SSD1306_Buffer[SSD1306_WIDTH * i], SSD1306_WIDTH);
	}
}

void ssd1306_DrawLogo(void)
{
	uint8_t i;

	for (i = 0; i < 8; i++) {
		WriteCommand(SSD1306_I2C_ADDR,0xB0 + i);
		WriteCommand(SSD1306_I2C_ADDR,0x00);
		WriteCommand(SSD1306_I2C_ADDR,0x10);

    	WriteMem(SSD1306_I2C_ADDR , 0x40, &beuth[SSD1306_WIDTH * i], SSD1306_WIDTH);
	}
}

void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color)
{
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) 
	{
		return;
	}
	
	if (SSD1306.Inverted) 
	{
		color = (SSD1306_COLOR)!color;
	}
	
	if (color == White)
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} 
	else 
	{
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color)
{
	uint32_t i, b, j;
	
	if (SSD1306_WIDTH <= (SSD1306.CurrentX + Font.FontWidth) ||
		SSD1306_HEIGHT <= (SSD1306.CurrentY + Font.FontHeight))
	{
		return 0;
	}
	
	for (i = 0; i < Font.FontHeight; i++)
	{
		b = Font.data[(ch - 32) * Font.FontHeight + i];
		for (j = 0; j < Font.FontWidth; j++)
		{
			if ((b << j) & 0x8000) 
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
			} 
			else 
			{
				ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
			}
		}
	}
	
	SSD1306.CurrentX += Font.FontWidth;
	
	return ch;
}

char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color)
{
	while (*str) 
	{
		if (ssd1306_WriteChar(*str, Font, color) != *str)
		{
			return *str;
		}
		
		str++;
	}
	
	return *str;
}

void ssd1306_SetCursor(uint8_t x, uint8_t y) 
{
	SSD1306.CurrentX = x;
	SSD1306.CurrentY = y;
}
