gcc -Os -s -m64 -shared -FPIC cube_libretro.c ..\minicube\machine.c ..\minicube\chips\fake6502.c ..\minicube\chips\wsg.c ..\minicube\chips\blitter.c -I..\minicube\chips\ -I..\minicube -o mcube_libretro.so
