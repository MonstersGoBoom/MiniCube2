	.import source "mcube.s"

	.segment CODE [start=$0200,outBin="audio_demo.cb2"]
	.segment ZP [start=$0]

vbl:		.byte 0 
ticker:	.byte 0 
color:	.byte 0

	.segment CODE "Main"
	* = $200 "Code"
	sei

	ldx #$ff
	txs 

	ldx #$5 
setPages:
	lda #>samples
	sta APU.Page,x 
	dex 
	bpl setPages

	lda #>colors
	sta VDC.CLUT

	lda #<on_vblank 
	sta IRQ
	lda #>on_vblank 
	sta IRQ+1

	lda #$00 
	sta BLT.Key
	sta color

	lda #$10 
	sta BLT.Width
	sta BLT.Height
	lda #$40 
	sta BLT.Stride
	jsr VGM.Reset

	cli 
wait:
	lda vbl 
	beq wait
	lda #$00 
	sta vbl 
	lda #$ff 
	sta colors+1 
	jsr VGM.Update 
	lda #$80 
	sta colors+1 
	inc ticker 
	bne ncolor 
	inc color
ncolor:
	lda ticker 
	and #1 
	clc 
	adc #$d 
	sta BLT.Page 
	sta VDC.Page 

	lda #$40 
	sta BLT.XScale
	sta BLT.YScale	
	sta BLT.Width
	sta BLT.Height
	sta BLT.Stride
	lda #<sprite 
	sta BLT.Source
	lda #>sprite 
	sta BLT.Source+1 
	lda #SOLID | BPP4 
	sta BLT.Trigger 

	lda #$c0 
	sta colors+1 


	lda ticker 
	sta BLT.XScale
	sta BLT.YScale	
	lda #$40
	sta BLT.Width
	sta BLT.Height
	lda color 
	and #$f
	ora #KEYED | BPP4
	sta BLT.Trigger 

	lda #$0 
	sta colors+1 

	jmp wait

	.segment CODE "IRQ"

on_vblank:
	pha 
	txa 
	pha 
	tya 
	pha 
	inc vbl 
	pla
	tay
	pla
	tax
	pla
	rti


VGM:
{
.const PATLEN = 32

	.segment ZP "VGM"
	snd_tune: 			.word 0
	snd_pat_ptr:		.word 0
	snd_step: 			.byte 0
	snd_pat_l: 			.fill MAX_CHANNELS,0 
	snd_pat_h: 			.fill MAX_CHANNELS,0 
	snd_pat_rle: 		.fill MAX_CHANNELS,0 
	.segment CODE "VGM Player"
Init:
nextPattern:
	//	set up pattern 
	//	reset pattern counter
	lda #PATLEN
	sta snd_step
	//	check for end byte marker
	ldy #$00
	lda (snd_tune),y 
	cmp #$ff	//	set repeat 
	bne noend 
	//	restart tune 
Reset:		
	lda #<Track 
	sta snd_tune
	lda #>Track 
	sta snd_tune+1
	jmp nextPattern
noend:
	//	copy pattern channel 0 into pointers
	tax 
	lda patternoff_l,x 
	sta snd_pat_l
	lda patternoff_h,x 
	sta snd_pat_h
	iny 
	//	copy pattern channel 1 into pointers
	lda (snd_tune),y 
	tax 
	lda patternoff_l,x 
	sta snd_pat_l+1
	lda patternoff_h,x 
	sta snd_pat_h+1
	iny 
	//	copy pattern channel 2 into pointers
	lda (snd_tune),y 
	tax 
	lda patternoff_l,x 
	sta snd_pat_l+2
	lda patternoff_h,x 
	sta snd_pat_h+2
	//	reset per channel clock
	lda #$01
	sta snd_pat_rle
	sta snd_pat_rle+1
	sta snd_pat_rle+2
	//	add 3 to the pointer ( 3 channels music )
	clc 
	lda snd_tune
	adc #3
	sta snd_tune
	lda snd_tune+1
	adc #0 
	sta snd_tune+1
	rts

Update:
	ldx #$02
chLoop:	 
	// copy this channels pattern into tmp pointer
	lda snd_pat_l,x 
	sta snd_pat_ptr
	lda snd_pat_h,x 
	sta snd_pat_ptr+1
	//	decrement clock and check if we should play yet
	dec snd_pat_rle,x 
	lda snd_pat_rle,x 
	bne skiptrigger 
trigger:
	//	reset this channel's clock
	lda #1 
	sta snd_pat_rle,x
	//	get Wave and Volume
	ldy #$00 
	lda (snd_pat_ptr),y 
	//	check clock set control byte 
	cmp #$01	//	set repeat 
	bne notControlByte01 
	//	data is $ff $xx xx = clock 
	iny 
	lda (snd_pat_ptr),y 
	sta snd_pat_rle,x 
	iny 
	//	now get the wave / volume
	lda (snd_pat_ptr),y 
notControlByte01:	
	//	put to audio channel ,X
	sta APU.Control,x
	//	next byte is frequency 
	iny 
	lda (snd_pat_ptr),y
	sty ty 
	tay

	lda freq_h,y
	sta APU.Freq_H,x 

	lda freq_l,y
	sta APU.Freq_L,x 

	ldy ty:#$00
	iny 

	//	step x bytes for this pattern 
	//	can include control bytes 
	clc 
	tya 
	adc snd_pat_l,x 
	sta snd_pat_l,x 
	lda snd_pat_h,x 
	adc #0 
	sta snd_pat_h,x 
skiptrigger:
	dex 
	bpl chLoop
	//	count the pattern length
	dec snd_step
	bne vgm_exit
	//	start next pattern
	jmp nextPattern
vgm_exit: 
	rts
}

	.segment CODE "Tune"
	.import source "tune.s"
	.align 256
samples:
	.import binary  "../resources/furnace.bin.wc"

	.segment CODE "Art"
sprite:
	.import binary "../resources/ball4.raw"

	.align 256
colors:
	.import binary "../resources/ball4.pal"

