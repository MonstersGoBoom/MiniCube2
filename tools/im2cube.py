# hacky tool 
# python im2cube.py <filename> <bitdepth>
# acceptable depths 1,2,4,8 

import math
import sys
import os

from PIL import Image

def main():
	basename = sys.argv[1]
	if len(sys.argv)==3:
		bpp = int(sys.argv[2])
	else:
		bpp = 8
 
	splitname = basename.split(".")
	outname = splitname[0] + ".raw"
	outclutname = splitname[0] + ".pal"

	image = Image.open(basename)
	(w, h) = image.size

	original_palette = image.getpalette()

	print()
	print(outname)
	print(outclutname)
	print()

	outraw = open(outname, "wb")
	outclut = open(outclutname, "wb")

	for ty in range(0,h):
		if bpp==1:
			for tx in range(0,w,8):
				p =   (image.getpixel((tx,ty))&0x1)<<7
				p |= (image.getpixel((tx+1,ty))&0x1)<<6
				p |= (image.getpixel((tx+2,ty))&0x1)<<5
				p |= (image.getpixel((tx+3,ty))&0x1)<<4
				p |= (image.getpixel((tx+4,ty))&0x1)<<3
				p |= (image.getpixel((tx+5,ty))&0x1)<<2
				p |= (image.getpixel((tx+6,ty))&0x1)<<1
				p |= (image.getpixel((tx+7,ty))&0x1)<<0
				outraw.write(p.to_bytes(1,byteorder='big'))

		if bpp==2:
			for tx in range(0,w,4):
				p =   image.getpixel((tx,ty))&0x3
				p |= (image.getpixel((tx+1,ty))&0x3)<<2
				p |= (image.getpixel((tx+2,ty))&0x3)<<4
				p |= (image.getpixel((tx+3,ty))&0x3)<<6
				outraw.write(p.to_bytes(1,byteorder='big'))
		if bpp==4:
			for tx in range(0,w,2):
				p = image.getpixel((tx,ty))&0xf
				p |= (image.getpixel((tx+1,ty))&0xf)<<4
				outraw.write(p.to_bytes(1,byteorder='big'))
		if bpp==8:
			for tx in range(0,w):
				p = image.getpixel((tx,ty))
				outraw.write(p.to_bytes(1,byteorder='big'))

	print('Palette:')

	for i in range(0,len(original_palette),3):
			(r,g,b) = original_palette[i],original_palette[i+1],original_palette[i+2]
#			print('hex #%02x%02x%02x ;%02x' % (r, g, b, i))
			outclut.write(r.to_bytes(1,byteorder='big'))
			outclut.write(g.to_bytes(1,byteorder='big'))
			outclut.write(b.to_bytes(1,byteorder='big'))

	outraw.close()
	outclut.close()
	print()

if __name__ == '__main__':
   main()
