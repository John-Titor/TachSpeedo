CHIP		 = LPC810
PORT		 = /dev/cu.SLAB_USBtoUART

CLASSLIB	 = $(abspath ./lpc8xx_classlib)
BIN		 = obj/TachSpeedo.bin
SRCS		:= $(abspath $(wildcard *.cpp))
LIBS		:= $(CLASSLIB)/obj/lpc8xx_classlib.a

include $(CLASSLIB)/make.inc
