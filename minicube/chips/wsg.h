

#define WSG_SAMPLE_RATE 96000
#define MAX_CHANNELS 6

#pragma pack(1)
typedef struct wsg {
  uint32_t accumulator[MAX_CHANNELS]; // 20 bit value
  int gain;
} wsg;

#pragma pack(1)
typedef struct 
{
	uint8_t page[MAX_CHANNELS];
	uint8_t control[MAX_CHANNELS];	
	uint8_t freq_l[MAX_CHANNELS];	
	uint8_t freq_m[MAX_CHANNELS];	
} WSG_chip;

extern wsg *w;

void wsg_init();
void wsg_play(int16_t* const buffer, int buffer_len);
int16_t wsg_tick();
