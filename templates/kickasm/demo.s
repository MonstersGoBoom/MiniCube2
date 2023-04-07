	.import source "mcube.s"

	.segment CODE [start=$0200,outBin="demo.cb2"]
	.segment ZP [start=$0]

vbl:		.byte 0 
ticker:	.byte 0 
color:	.byte 0


	.segment CODE "Main"
	* = $200 "Code"
	sei

	ldx #$ff
	txs 

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
	cli 
wait:
	lda vbl 
	beq wait
	lda #$00 
	sta vbl 
	lda #$ff 
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

	.segment CODE "Data"

sprite:
	.import binary "../resources/ball4.raw"

	.align 256
colors:
	.import binary "../resources/ball4.pal"

