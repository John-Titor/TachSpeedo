CHIP		 = LPC810
PORT		 = /dev/cu.SLAB_USBtoUART

CLASSLIB	 = $(firstword \
			$(wildcard ../../../NXP/lpc8xx_classlib) \
			$(abspath ./lpc8xx_classlib))
BIN		 = obj/TachSpeedo.bin
SRCS		:= $(abspath $(wildcard *.cpp))
EXTRA_FMT_SRCS	:= $(abspath $(wildcard *.h))
LIBS		:= $(CLASSLIB)/obj/lpc8xx_classlib.a

include $(CLASSLIB)/make.inc
