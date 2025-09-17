# ========= Toolchain =========
CC = riscv32-unknown-elf-gcc
OBJDUMP = riscv32-unknown-elf-objdump

# ========= Build Options =========
ARCH = rv32imc
ABI  = ilp32
CFLAGS = -march=$(ARCH) -mabi=$(ABI) -O2 -Wall -ffreestanding -nostdlib
LDFLAGS = -march=rv32imc -mabi=ilp32 -ffreestanding -nostartfiles -T link.ld

# ========= Paths =========
SRC_DIR = Kernel
BUILD_DIR = Debug

# ========= Collect all source files =========
SRCS = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/Drivers/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

TARGET = $(BUILD_DIR)/program.elf

# ========= Rules =========
all: $(TARGET)

# Compile .c → .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)      # Ensure the directory exists
	$(CC) $(CFLAGS) -c $< -o $@

# Link all .o → .elf
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# Disassemble for debugging
disasm: $(TARGET)
	$(OBJDUMP) -D $(TARGET) > $(BUILD_DIR)/program.asm

# Run in QEMU
run: $(TARGET)
	qemu-system-riscv32 -nographic -machine virt -bios none -kernel $(TARGET)

# Launch Ripes GUI
ripes-gui: $(TARGET)
	./Ripes.AppImage --mode gui --src $(TARGET) -t elf --proc "RV32_5S" --isaexts M,C

# Cleanup
clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean run disasm ripes-gui
