CC        = gcc
MINGW_CC  = x86_64-w64-mingw32-gcc
CFLAGS    = -O2 -Wall -Wextra -std=c99
TARGET    = crc
WIN_TARGET = crc.exe
GUI_TARGET = crc_gui.exe
SRCS      = main.c crc.c
OBJS      = $(SRCS:.c=.o)
WIN_OBJS  = $(SRCS:.c=.win.o)
GUI_OBJS  = gui_main.win.o crc.win.o

all: $(WIN_TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

windows: $(WIN_TARGET)

$(WIN_TARGET): $(WIN_OBJS)
	$(MINGW_CC) $(CFLAGS) -o $@ $^

%.win.o: %.c
	$(MINGW_CC) $(CFLAGS) -c -o $@ $<

gui: $(GUI_TARGET)

$(GUI_TARGET): $(GUI_OBJS)
	$(MINGW_CC) $(CFLAGS) -o $@ $^ -lgdi32 -lcomdlg32 -mwindows

gui_main.win.o: gui_main.c crc.h
	$(MINGW_CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(WIN_OBJS) $(GUI_OBJS) $(TARGET) $(WIN_TARGET) $(GUI_TARGET)

.PHONY: all windows gui clean
