export WATCOM  := C:\work\WATCOM
export PATH    := $(WATCOM)\binnt;$(WATCOM)\binw;$(PATH)
export INCLUDE := $(WATCOM)\h

CC     := wcl386
CFLAGS := -i=$(WATCOM)\h -l=causeway
TARGET := main.exe

SRCS   := main.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -fe=$(TARGET)

clean:
	@if exist *.obj del /q *.obj
	@if exist *.err del /q *.err
	@if exist $(TARGET) del /q $(TARGET)