# PATH TO THE SDL2_FRAMEWORK
SDL2_PATH = $(shell brew --prefix sdl2)
EXT_PATH = $(shell brew --prefix sdl2_ttf)

# Compiler
CC = cc

# Source files in the c_files directory
SRCS = $(shell find c_files -name "*.c")

# Output executable
TARGET = a.out

# Compiler flags (Include headers from both SDL2 and Ext path)
CFLAGS = -I$(SDL2_PATH)/include/SDL2 \
         -I$(EXT_PATH)/include

DEBUG = -fsanitize=address -g

# Linker flags (Link libraries instead of frameworks)
LDFLAGS = -L$(SDL2_PATH)/lib -L$(EXT_PATH)/lib \
          -lSDL2 -lSDL2_ttf \
          -Wl,-rpath,$(SDL2_PATH)/lib -Wl,-rpath,$(EXT_PATH)/lib

# Build rule
$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

debug:
	$(CC) $(SRCS) $(CFLAGS) $(DEBUG) $(LDFLAGS) -o $(TARGET)

# Clean rule
clean:
	rm -f $(TARGET)

re: clean $(TARGET)
