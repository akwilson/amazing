SRCS = graphics.c amazing.c

BIN1 = amazing
BIN1_SRCS = $(SRCS)
LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf

include lib/simplified-make/simplified.mk
