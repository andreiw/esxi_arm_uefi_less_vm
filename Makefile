CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

TARGET = Image
TEXT_BASE = 0x0

CFLAGS = \
	-march=armv8-a \
	-mlittle-endian \
	-fno-stack-protector \
	-mgeneral-regs-only \
	-mstrict-align \
	-fno-common \
	-fno-builtin \
	-ffreestanding \
	-std=gnu99 \
	-Werror \
	-Wall \
	-I ./

LDFLAGS = -pie

%.o: %.S
	$(CC) $(CFLAGS) $< -c -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -v -O binary $< $@

$(TARGET).elf: start.o demo.o string.o fdt.o ctype.o fdt_ro.o fdt_strerror.o vsprintf.o serial.o
	$(LD) -T boot.lds -Ttext=$(TEXT_BASE) $(LDFLAGS) $^ -o $@

clean:
	rm -f *.o $(TARGET) $(TARGET).*

cleaner: clean
	 git clean -f -d
	 git checkout demo.vmx

.PHONY: all clean cleaner
