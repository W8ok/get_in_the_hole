CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -O0 -g -Isrc/include
LIBS = -lpthread -ldl -lm -lSDL3 -lSDL3_image

SRCS := $(shell find src -name "*.c")
OBJS := $(patsubst src/%.c,obj/%.o,$(SRCS))
TARGET = get-in-the-hole

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET)
	rm -rf obj/

run: $(TARGET)
	./$(TARGET)

debug: clean $(TARGET)
	gdb ./$(TARGET)

.PHONY: clean run debug
