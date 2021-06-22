CC     = gcc
C_FLAGS= -Wall -Werror -Wno-unused-but-set-variable -g
L_FLAGS= -lncurses
TARGET = five_dice


.DEFAULT: all

.PHONY: all
all: $(TARGET)


$(TARGET): *.c
	$(CC) $(C_FLAGS) $^ -o $@ $(L_FLAGS)


clean:
	$(RM) $(TARGET)
