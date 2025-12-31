PONGO_SRC ?= ../PongoOS
CC ?= clang -target aarch64-darwin-none
CFLAGS +=  -I$(PONGO_SRC)/include -I$(PONGO_SRC)/src/kernel -I$(PONGO_SRC)/newlib/aarch64-none-darwin/include -Iinclude -I$(PONGO_SRC)/apple-include
CFLAGS += -I$(PONGO_SRC)/src/drivers -Oz -ffreestanding -nostdlib -U__nonnull -DTARGET_OS_OSX=0 -DTARGET_OS_MACCATALYST=0 -D_GNU_SOURCE -DDER_TAG_SIZE=8
CFLAGS += -fvisibility=hidden -Wno-macro-redefined
LDFLAGS += -Wl,-kext -Wl,-dead_strip -flto=full -fno-stack-protector

all: cpufreq

obj/%.o: src/%.c
	$(CC) -c -nostdlibinc $(CFLAGS) $< -o $@

cpufreq: $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o cpufreq

format:
	clang-format -i src/*.c src/*.h

clean:
	rm -f cpufreq obj/*.o

