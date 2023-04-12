
//#define TRACK_MEMORY
//#define DEBUG_OUTPUT 


#define IO_VIDEO 0x0100 
#define IO_COLORS IO_VIDEO+1
#define IO_INPUT IO_VIDEO+2
#define IO_BLOCK IO_VIDEO+3
#define BLITTER_LOC (IO_VIDEO+3)
#define BLITTER_TRIGGER (IO_VIDEO+0xd)

#define IO_VIDEO_CTRL IO_VIDEO+0x1f 
#define IO_AUDIO_REGS IO_VIDEO+0x20

void machine_boot(const unsigned char *ptr,int len);
void machine_run(uint32_t *pixels,int pitch);
void machine_setpad(uint8_t pad);
void machine_dispose();

#define consoleWidth 64
#define consoleHeight 64

#define AUDIO_SAMPLERATE 48000
#define AUDIO_CHANNELS 1
#define AUDIO_BITS 16 
#define MAX_SAMPLES_PER_UPDATE 128 

extern uint8_t memory[1<<16];
#ifdef TRACK_MEMORY
extern uint8_t read_memory[1<<16];
extern uint8_t write_memory[1<<16];
#endif 


