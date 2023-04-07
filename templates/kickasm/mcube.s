
	* = $ff00 "VDC" virtual
VDC:
{
	Page:			.byte 0
	CLUT:			.byte 0
	IO:				.byte 0 
}


.const SOLID 		= %00000000
.const KEYED 		= %10000000
.const ADDRESS 	= %01000000
.const BPP1 		= %00000000
.const BPP2 		= %00010000
.const BPP4 		= %00100000
.const BPP8 		= %00110000

BLT:
{
	X:				.byte 0
	Y:				.byte 0
	Width:		.byte 0
	Height:		.byte 0		//	signed
	Stride:		.byte 0		//	width of unsigned  
	XScale:		.byte 0 	//	2:6 fixed point $40 = 1 pixel
	YScale:		.byte 0 	//	2:6 fixed point	
	Page:			.byte 0
	Source:		.word 0
	Trigger:	.byte 0
	Key:			.byte 0
	Mode:			.byte 0 	//	experimental
}

	* = $ff20 "APU" virtual
APU:
{
	Page:			.fill 6,0
	Control:	.fill 6,0
	Freq_L:		.fill 6,0
	Freq_H:		.fill 6,0
} 

	* = $fffe "NMI" virtual
	IRQ:	.word 0

