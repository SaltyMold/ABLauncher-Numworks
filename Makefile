Q ?= @
CC = arm-none-eabi-gcc
NWLINK = npm_config_loglevel=silent npx --yes --quiet -- nwlink@1.0.0
BUILD_DIR = output

src = $(addprefix src/,\
  display.c \
  functions.c \
  main.c \
)
obj = $(addprefix $(BUILD_DIR)/,$(src:.c=.o))

CFLAGS = -std=c99 $(shell $(NWLINK) eadk-cflags-device) -Os -Wall -ggdb
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -flto -fno-fat-lto-objects -fwhole-program -fvisibility=internal

LDFLAGS = -Wl,--relocatable -nostartfiles --specs=nano.specs
LDFLAGS += -Wl,-e,main -Wl,-u,eadk_app_name -Wl,-u,eadk_app_icon -Wl,-u,eadk_api_level
LDFLAGS += -Wl,--gc-sections -flinker-output=nolto-rel

.PHONY: build
build: $(BUILD_DIR)/app.nwa

.PHONY: run
run: build
	$(Q) $(NWLINK) install-nwa $(BUILD_DIR)/app.nwa

$(BUILD_DIR)/app.nwa: $(obj) $(BUILD_DIR)/icon.o
	@echo "LD      $@"
	$(Q) $(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	@echo "CC      $<"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/icon.o: assets/icon.png
	@echo "ICON    $<"
	$(Q) mkdir -p $(dir $@)
	$(Q) $(NWLINK) png-icon-o $< $@

.PHONY: clean
clean:
	@echo "CLEAN"
	$(Q) rm -rf $(BUILD_DIR)
