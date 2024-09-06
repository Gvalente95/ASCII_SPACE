# Base path to frameworks
FRAMEWORK_PATH = /Library/Frameworks

# SDL2 Framework path
SDL2_FRAMEWORK_PATH = $(FRAMEWORK_PATH)/SDL2.framework

# Compiler
CC = cc

# Source files in the c_files directory
SRCS = $(wildcard c_files/*.c)

# Output executable
TARGET = a.out

# Compiler flags
CFLAGS = -I$(SDL2_FRAMEWORK_PATH)/Headers

# Linker flags
LDFLAGS = -F$(FRAMEWORK_PATH) -framework SDL2 \
          -Wl,-rpath,$(FRAMEWORK_PATH)

# Build rule
$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LDFLAGS) -o $(TARGET)

# Clean rule
clean:
	rm -f $(TARGET)
