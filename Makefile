# Araç Zinciri (Toolchain)
CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-as
LD      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# İşlemci Ayarları
TARGET_CPU = cortex-m3
CFLAGS     = -mcpu=$(TARGET_CPU) -mthumb -mfloat-abi=soft -std=gnu11 -Wall -O0 -g3
ASFLAGS    = -mcpu=$(TARGET_CPU) -mthumb
LDFLAGS    = -mcpu=$(TARGET_CPU) -mthumb --specs=nosys.specs -T$(LDSCRIPT) -Wl,-Map=$(BUILD_DIR)/final.map

# Klasör Yapısı
PROJECT_DIR    = .
APP_DIR        = $(PROJECT_DIR)/app
MIDDLEWARE_DIR = $(PROJECT_DIR)/middleware
PORT_DIR       = $(MIDDLEWARE_DIR)/portable/arm_cm3
BSP_DIR        = $(PROJECT_DIR)/bsp/stm32f103c8t6
DRIVERS_DIR    = $(PROJECT_DIR)/drivers
BUILD_DIR      = $(PROJECT_DIR)/build

LDSCRIPT = $(BSP_DIR)/stm32f03c8t6_linker_script.ld

# Include Yolları
INCLUDES = -I$(APP_DIR) \
           -I$(MIDDLEWARE_DIR) \
           -I$(PORT_DIR) \
           -I$(DRIVERS_DIR)

# Kaynak Dosyaları Otomatik Bulma
SRCS = $(wildcard $(APP_DIR)/*.c) \
       $(wildcard $(MIDDLEWARE_DIR)/*.c) \
       $(wildcard $(PORT_DIR)/*.c) \
       $(wildcard $(DRIVERS_DIR)/*.c) \
       $(wildcard $(BSP_DIR)/*.c)

ASMS = $(wildcard $(PORT_DIR)/*.s)

# Nesne (.o) Dosyaları Listesi (Build klasörü içinde oluşturulacak)
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
OBJS += $(ASMS:%.s=$(BUILD_DIR)/%.o)

# Dosyaları bulabilmesi için VPATH tanımlıyoruz
VPATH = $(APP_DIR):$(MIDDLEWARE_DIR):$(PORT_DIR):$(DRIVERS_DIR):$(BSP_DIR)

# Varsayılan Hedef (Tüm projeyi derle)
all: $(BUILD_DIR)/final.elf $(BUILD_DIR)/final.bin

# Linkleme Aşaması
$(BUILD_DIR)/final.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/final.bin: $(BUILD_DIR)/final.elf
	$(OBJCOPY) -O binary $< $@

# C Dosyalarını Derleme
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Assembly Dosyalarını Derleme
$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# Temizlik
clean:
	rm -rf $(BUILD_DIR)

# Cppcheck & MISRA Analizi
check:
	cppcheck --enable=all $(INCLUDES) --addon=misra ./middleware/*.c ./app/*.c

load:
	openocd -f interface/stlink.cfg -f board/stm32f103c8_blue_pill.cfg