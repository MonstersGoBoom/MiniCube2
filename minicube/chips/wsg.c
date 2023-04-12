#include <stdio.h>
#include <stdint.h>
#include "machine.h"
#include "fake6502.h"
#include "wsg.h"

wsg _wsg;
wsg *w = &_wsg;



//	reset "chip"
void wsg_init()
{
	WSG_chip *chip=(WSG_chip*)&memory[IO_AUDIO_REGS];
  for (int voice_no = 0; voice_no < MAX_CHANNELS; voice_no++)
	{
    w->accumulator[voice_no] = 0;
		chip->page[voice_no]=0;
		chip->control[voice_no]=0;
		chip->freq_m[voice_no]=0;
		chip->freq_l[voice_no]=0;
  }
  w->gain = 25;

}

//	called 735 times per frame. ish :P
int16_t wsg_tick()
{
	WSG_chip *chip=(WSG_chip*)&memory[IO_AUDIO_REGS];
	int16_t sample = 0;
	//	for each channel
	for (int v = 0; v < MAX_CHANNELS; v++)
	{
		uint16_t sound_ram = chip->page[v]<<8;
		//	current frequency
		uint32_t freq = (chip->freq_m[v]<<8) | chip->freq_l[v];
		//	check we're playing
		if (((chip->control[v]&0xf0)!=0) && (freq!=0))
		{
			int8_t sv;
			uint8_t volume,wave;
			volume=(chip->control[v]>>4)&0xf;
			wave=chip->control[v]&0xf;

			//	add freq 
			w->accumulator[v]+=freq;
//			w->accumulator[v]+=freq>>1;
			//	get sample offset
			int offset = ((w->accumulator[v]>>16)&0x1f);	//	sample offset
			//	get waveform offset 
			int sample_pos = (wave<<4) + offset;
			sv = read6502(sound_ram+(sample_pos>>1));
			if ((sample_pos&1)==0)
				sv>>=4;
				
			sv&=0xf;
			int16_t voice_sample = (sv-8) * volume;

//			sv = read6502(sound_ram+(sample_pos>>1));
//			if ((sample_pos&1)==0)
//				sv>>=4;
//			sv&=0xf;
//			int16_t voice_sample = (sv-8) * ((chip->control[v]>>4)&0xf);


			//	add sound 
			sample+=voice_sample;			
		}
	}
	return sample*w->gain;
}

//	playback
void wsg_play(int16_t* const buffer, int buffer_len)
{
	WSG_chip *chip=(WSG_chip*)&memory[IO_AUDIO_REGS];
	//	do sample length
  for (int i = 0; i < buffer_len; i++)
	{
		buffer[i]=wsg_tick();
	}
}

