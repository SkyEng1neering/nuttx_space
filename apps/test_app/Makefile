include $(APPDIR)/Make.defs

PROGNAME  = filexfer
PRIORITY  = 100
STACKSIZE = 2048

MAINSRC = filexfer_main.c
CSRCS += \
platform/fileXferCallbacks.c \
fileXferLib/src/fileXfer.c \
fileXferLib/src/fileXferUtils.c \

CFLAGS += \
-IfileXferLib/inc \



# Build with WebAssembly when CONFIG_INTERPRETERS_WAMR is enabled

WASM_BUILD = y

# Mode of WebAssembly Micro Runtime

WAMR_MODE  = AOT

include $(APPDIR)/Application.mk
