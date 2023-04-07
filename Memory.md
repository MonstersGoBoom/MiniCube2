
$FF00 - Video Display Page.

	$0-$F directs the display controller to point to $n000 in memory 

$FF01 - Video Display Colors.

	$00-$FF directs the color ram to $nn00 in memory. 
	768 bytes of color data ( subject to change ) 
$FF02 - Controller input 

$FF03-$FF10 Blitter Control.

	$FF03 - Xposition ( signed int 8 ) 0 is left edge.
	$FF04 - Yposition ( signed int 8 ) 0 is top edge 
	$FF05 - Width ( signed int 8 ) negative values create X flipped data of abs(width) 
	$FF06 - Height ( signed int 8 ) negative values create Y flipped data of abs(height)
	$FF07 - Data stride, the data increment per Y scanline of data.
	$FF08 - X Scale 2:6 bit fixed point. $40 = 1 pixel per pixel $20 is half size
	$FF09 - Y Scale 2:6 bit fixed point. $80 = 2 pixel per output pixel 
	$FF0A - Data page, output ram page, $0-$F writes to $n000 page 
	$FF0B - Source of data LOW BYTE
	$FF0C - Source of data HI BYTE
	$FF0D - Trigger and control. 
			- write to this AFTER setting above registers. writing to this byte triggers the Blitter CHIP
			- %x------- skip colorkey value, don't write if source contains Key 
			- %_x------ use X & Y bytes as ADDRESS to write, vs X&Y screen position. ( ignores Page )
			- %--xx---- bits per pixel of source data. 
			     00     1 bit per pixel data ( Useful for fonts ) 
			     01     2 bit per pixel data 
			     10     4 bit per pixel data 
			     11     8 bit per pixel data 
			- %----xxxx color palette control, depends on BPP source data 
			       BPP1 xxxx is literal offset into palette 
			       BPP2 xxxx is xxxx * 4 e.g. 1 adds 4 to input color index, 2 adds 8 etc. 
			       BPP4 xxxx is xxxx * 16 e.g. 1 adds 16 to input color index , 2 adds 32 etc. 
			       BPP8 serves no purpose currently

$FF0E - Color Key value
	
	if Trigger has %10000000 set blitter will ignore this value 

$FF0F - experimental bits.

	- 0 draw normally 
	- 1 draw transparent. 
		to be explained 

TODO
---- 
clarify blitter cycle speeds. 

Audio 
-----
6 channel Namco WSG style chip.

Extended to add 3 additional channels

Extended to support 16 waveforms vs 8 

Samples are encoded as 32 nibbles vs 32 bytes ( 16 bytes per sample )

Stored in per channel consecutive bytes.

	$FF20-25 - Memory Page for Audio Channel, $nn00 source ram for audio sample

	$FF26-2B - ControlBits 	%xxxx---- volume 
							%----xxxx waveform select
	$FF2C-31 - Frequency low msb
	$FF32-37 - Frequency hi msb

