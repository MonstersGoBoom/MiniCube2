#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "machine.h"
#include "fake6502.h"
#include "blitter.h"

_blitter_t_ *blit=NULL;
bool writePixel = true;
//	bits to make 1.0 for fixed pointer
#define SCALESHIFT 6
uint32_t bpixels = 0;
#define MAX_PIXELS (4096*4)
uint8_t getPixel1(uint16_t source,uint8_t x)
{
	uint8_t b;
	uint8_t bit = 1<<(7-(x&7));
	b=read6502(source+(x>>3))&bit;
	if (b!=0)
	{
		b = blit->shade;
	}
	else
	{
		if (blit->fg_mode==1)
			writePixel=false;
	}
	return b;
}

uint8_t getPixel2(uint16_t source,uint8_t x)
{
	uint8_t b;
	b=read6502(source+(x>>2));
	uint8_t shf = ((x)&3)<<1;
	b>>=shf;
	b&=3;
	if (((blit->fg_mode)!=0) && (b==0)) writePixel=false;
	b|=blit->shade<<2;
	return b;
}

uint8_t getPixel4(uint16_t source,uint8_t x)
{
	uint8_t b;
	b=read6502(source+(x>>1));
	if (((x)&1)!=0)				
		b>>=4;
	b&=0xf;
	if (((blit->fg_mode)!=0) && (b==0)) writePixel=false;
	b|=blit->shade<<4;
	return b;
}

uint8_t getPixel8(uint16_t source,uint8_t x)
{
	uint8_t b;
	b=read6502(source+x);
	if (((blit->fg_mode)!=0) && (b==blit->colorKey)) writePixel=false;
	return b;
}

uint32_t cycles_cost;

void blitData()
{
	uint8_t b=0;;
	uint16_t s = blit->source;
	uint16_t ss = s;
	uint16_t dest = (blit->y<<8) | blit->x;
	uint8_t (*getPixel)(uint16_t source,uint8_t x);
	uint8_t stride=0;
	int32_t xp;

	cycles_cost=0;

	if (blit->address)
	{
		blit->page = dest/4096;
		blit->x = dest & 0x3f;
		blit->y = (dest>>6) & 0x3f;
	}
	uint16_t destram = (blit->page&0xf)*4096;

	switch(blit->data_mode)
	{
		case 0:
		{
			stride = blit->stride>>3;
			getPixel = getPixel1;
			break;
		}
		case 1:
		{
			stride = blit->stride>>2;
			getPixel = getPixel2;
			break;
		}
		case 2:
		{
			stride = blit->stride>>1;
			getPixel = getPixel4;
			break;
		}
		case 3:
		{
			stride = blit->stride;
			getPixel = getPixel8; 
			break;
		}
	}
	int yp=0;
	uint8_t aheight = abs(blit->h);
	int ystep = blit->yscale;

	if (blit->h<0)
	{
		yp = (aheight*blit->yscale)-1;
		ystep = -blit->xscale;
	}

	for (int y=0;y<aheight;y++)
	{
		if (bpixels>=MAX_PIXELS) break;
		int xstep = blit->xscale;
		xp = 0;
		uint8_t awidth = abs(blit->w);

		if (blit->w<0)
		{
			xp = (awidth*blit->xscale)-1;
			xstep = -blit->xscale;
		}

		s = ss+((yp>>SCALESHIFT)*stride);
		if ((yp>>SCALESHIFT)>=aheight) break;

		int8_t oy = (y+blit->y);
		if ((oy>=0) && (oy<64))
		{
			for (int x=0;x<awidth;x++)
			{
				if (bpixels>=MAX_PIXELS) break;
				if ((xp>>SCALESHIFT)>=awidth) break;
				writePixel=true;
				int8_t ox = (x+blit->x);
				if ((ox>=0)&&(ox<64))
				{
					//	can change writePixel to false
					b = getPixel(s,xp>>SCALESHIFT);
					cycles_cost+=1;

					if (writePixel==true)
					{
						if (blit->drawMode==0)
						{
							write6502(destram+ox+(oy*consoleWidth),b);
							bpixels++;
							cycles_cost+=1;
						}
						else 
						{
							uint8_t a=read6502(destram+ox+(oy*consoleWidth));
							if (b!=blit->colorKey)
							{
								write6502(destram+ox+(oy*consoleWidth),a|(blit->shade<<4));
								bpixels++;
								cycles_cost+=1;
							}
						}
					}
				}
				xp+=xstep;
			}
		}
		yp+=ystep;
	}
}
