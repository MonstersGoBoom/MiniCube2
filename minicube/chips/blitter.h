
//	blitter chip 
#pragma pack(1)
typedef struct 
{
	int8_t x,y;
	int8_t w,h;
	uint8_t	stride;
	uint8_t xscale;
	uint8_t yscale;
	uint8_t page;
	uint16_t source;
	uint8_t	shade:4;
	uint8_t	data_mode:2;
	uint8_t	address:1;
	uint8_t	fg_mode:1;
	uint8_t colorKey;
	uint8_t drawMode;
} _blitter_t_;


extern _blitter_t_ *blit;
extern uint32_t bpixels;
extern uint32_t cycles_cost;

void blitData();
uint8_t getPixel1(uint16_t source,uint8_t x);
uint8_t getPixel2(uint16_t source,uint8_t x);
uint8_t getPixel4(uint16_t source,uint8_t x);
uint8_t getPixel8(uint16_t source,uint8_t x);