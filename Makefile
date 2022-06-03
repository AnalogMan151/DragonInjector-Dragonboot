ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/base_rules

TARGET 					:= dragonboot
BUILD 					:= build
OUTPUT 					:= output
SOURCEDIR 			:= src
DATA						:= data
SOURCES		      := src \
										src/core \
										src/ianos \
										src/gfx \
										src/libs/fatfs src/libs/elfload src/libs/compr \
										src/mem \
										src/panic \
										src/power \
										src/sec \
										src/soc \
										src/storage \
										src/utils

INCLUDES				:= include
VPATH = $(dir $(wildcard ./$(SOURCEDIR)/*/)) $(dir $(wildcard ./$(SOURCEDIR)/*/*/))
CFILES			:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
SFILES			:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES		:=  $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

OFILES_BIN		:= $(addsuffix .o,$(BINFILES))
OFILES_SRC		:= $(SFILES:.s=.o) $(CFILES:.c=.o)
HFILES_BIN		:= $(addsuffix .h,$(subst .,_,$(BINFILES)))

OBJS 					= $(addprefix $(BUILD)/$(TARGET)/, $(OFILES_BIN) $(OFILES_SRC))


INCLUDE				:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
										$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
										-I$(BUILD)/$(TARGET)

ARCH := -march=armv4t -mtune=arm7tdmi -mthumb -mthumb-interwork
CFLAGS = $(INCLUDE) $(ARCH) -Os -nostdlib -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-inline -std=gnu11 -Wall
LDFLAGS = $(ARCH) -nostartfiles -lgcc -Wl,--nmagic,--gc-sections


.PHONY: all clean

all: directories $(TARGET).lz4 $(TARGET).bin
	@echo $(HFILES_BIN)

directories:
	@mkdir -p "$(BUILD)"
	@mkdir -p "$(BUILD)/$(TARGET)"
	@mkdir -p "$(OUTPUT)"
	
clean:
	@rm -rf $(OBJS)
	@rm -rf $(BUILD)
	@rm -rf $(OUTPUT)

$(MODULEDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

$(TARGET).lz4: $(BUILD)/$(TARGET)/$(TARGET).bin
	@lz4 -c -f -l -9 $(BUILD)/$(TARGET)/$(TARGET).bin | dd of=$(OUTPUT)/$@ bs=1 skip=8

$(BUILD)/$(TARGET)/$(TARGET).bin: $(BUILD)/$(TARGET)/$(TARGET).elf $(MODULEDIRS)
	$(OBJCOPY) -S -O binary $< $@

$(BUILD)/$(TARGET)/$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -T $(SOURCEDIR)/link.ld $^ -o $@

$(BUILD)/$(TARGET)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/$(TARGET)/%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

$(OFILES_SRC)	: $(HFILES_BIN)