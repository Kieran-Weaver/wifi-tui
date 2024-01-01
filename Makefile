CC ?= gcc
TARGET := intf
CPPFLAGS = -I /usr/include/libnl3/ -MT $@ -MMD -MP -MF build/$*.d
CFLAGS := -Og -g
LDFLAGS := -lnl-3

SRCS := $(shell find . -path "*.c")
OBJS := $(patsubst %.c, ./build/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

./build/%.o: ./%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

intf: $(OBJS)
	$(CC) $^ -o $(TARGET) $(LDFLAGS)

all: intf

clean:
	rm -fr intf build

-include $(DEPS)
