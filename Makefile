CC ?= gcc
TARGET := wifitui
CPPFLAGS = -I /usr/include/libnl3/ -MT $@ -MMD -MP -MF build/$*.d
CFLAGS := -Og -g 
LDFLAGS := -lnl-3 -lnl-genl-3

SRCS := $(shell find . -path "*.c")
OBJS := $(patsubst %.c, ./build/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

./build/%.o: ./%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) $^ -o $(TARGET) $(LDFLAGS)

all: $(TARGET)

clean:
	rm -fr $(TARGET) build

-include $(DEPS)
