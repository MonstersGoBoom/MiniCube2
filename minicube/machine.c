

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "machine.h"
#include "fake6502.h"
#include "wsg.h"
#include "blitter.h"



#ifdef TRACK_MEMORY
uint8_t read_memory[1<<16];
uint8_t write_memory[1<<16];
#endif

uint8_t memory[1<<16];

uint8_t read6502(uint16_t address)
{
#ifdef TRACK_MEMORY
	read_memory[address]=memory[address];
#endif	
	return memory[address];
}

void write6502(uint16_t address, uint8_t value)
{
#ifdef TRACK_MEMORY
	write_memory[address]=value;
#endif	
	memory[address] = value;
	if (address==BLITTER_TRIGGER)
	{
		blitData();
	}
}

#ifdef DEBUG_OUTPUT
void trace()
{
	char debug[256];
	disasm6502(pc,debug,sizeof(debug));
	printf("%s\tA %02X X %02X Y %02X SP %02X\n",debug,a,x,y,sp);
}
#endif

void machine_boot(const unsigned char *ptr,int len)
{
	printf("clear mem\n");
	memset(memory,0,sizeof(memory));	
	memcpy(&memory[0x200],ptr,len);
	reset6502();
	write6502(IO_BLOCK,0);
	pc= 0x200;
	wsg_init();
}

void machine_dispose()
{
#ifdef DEBUG_OUTPUT
	Labels_Dispose();
#endif 
}

void machine_setpad(uint8_t pad)
{
	write6502(IO_INPUT,pad);
}

void machine_run(uint32_t *pixels,int pitch)
{
#ifdef TRACK_MEMORY
	for (int q=0;q<sizeof(read_memory);q++)
	{
		read_memory[q]/=3;
		if (write_memory[q]>4)
			write_memory[q]-=4;
		else 
			write_memory[q]=0;
	}
#endif
	bpixels=0;
	blit = (_blitter_t_*)&memory[BLITTER_LOC];
	uint8_t vramblock = read6502(IO_VIDEO)&0xf;
	uint16_t vram_address=vramblock*4096;
	uint8_t *vram = &memory[vram_address];
	//	draw result
	int z=0;
	uint32_t sclock = clockticks6502;
	uint32_t *output = pixels;
	uint8_t vdc_ctrl = read6502(IO_VIDEO_CTRL);

	if ((vdc_ctrl&0x80)==0)
	{
		exec6502((8*consoleWidth)*consoleHeight);
	}

	for (int y=0;y<consoleHeight;y++)
	{
		for (int x=0;x<consoleWidth;x++)
		{
			if ((vdc_ctrl&0x80)!=0)
			{
				if (cycles_cost>8)	cycles_cost-=8;
				else exec6502(8);
			}
			uint8_t paletteblock = read6502(IO_COLORS);
			uint8_t *palette = &memory[paletteblock*256];
			switch (vdc_ctrl&3)
			{
				case 0:
				{
					uint16_t index = vram[z++]*3;
					output[x]=(palette[index+2]<<16) | (palette[index+1]<<8) | (palette[index]<<0);
					break;
				}
				case 1:
				{
					uint16_t index = vram[z++]*2;
					uint8_t r=(palette[index]<<4)&0xf0;
					uint8_t g=(palette[index])&0xf0;
					uint8_t b=(palette[index+1]<<4)&0xf0;
					output[x]=(r<<16) | (g<<8) | (b<<0);
					break;
				}
//	undocumented toy modes
				case 2:
				{
					uint8_t col = palette[vram[z++]];
					uint8_t blue = col & 3;
					uint8_t green = (col >> 2) & 3;
					uint8_t red = (col >> 4) & 3;
					output[x]=(blue<<22) | (green<<14) | (red<<6);
					break;
				}
				case 3:
				{
					uint8_t col = palette[vram[z++]];
					uint8_t blue = col % 6;
					uint8_t green = (col / 6) % 6;
					uint8_t red = (col / 36) % 6;
					output[x]=((blue*42)<<16) | ((green*42)<<8) | ((red*42));
					break;
				}
			}
		}
		output+=pitch/sizeof(uint32_t);
	}

	if (irq_active==1)
	{
		irq6502();
		for (int q=0;q<8;q++)
		{
			if (cycles_cost>8)	cycles_cost-=8;
			else exec6502(8);
		}
	}
	uint32_t cycles = clockticks6502-sclock;
}
