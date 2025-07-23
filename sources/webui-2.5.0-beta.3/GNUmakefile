# WebUI Library

# == 1. VARIABLES =============================================================

WEBUI_OUT_LIB_NAME = webui-2

# Detect the OS once
ifeq ($(OS),Windows_NT)
DETECTED_OS := Windows
else
DETECTED_OS := $(shell uname -s)
endif

# TLS
WEBUI_USE_TLS =
WEBUI_TLS_INCLUDE ?= .
WEBUI_TLS_LIB ?= .
TLS_CFLAG = -DNO_SSL
TLS_LDFLAG_DYNAMIC =
ifeq ($(WEBUI_USE_TLS), 1)
WEBUI_OUT_LIB_NAME = webui-2-secure
TLS_CFLAG = -DWEBUI_TLS -DNO_SSL_DL -DOPENSSL_API_1_1
ifeq ($(DETECTED_OS),Windows)
TLS_LDFLAG_DYNAMIC = -lssl -lcrypto -lbcrypt
else
TLS_LDFLAG_DYNAMIC = -lssl -lcrypto
endif
endif

MAKEFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MAKEFILE_DIR := $(dir $(MAKEFILE_PATH))
BUILD_DIR := $(MAKEFILE_DIR)/dist

# ARGS
CC ?= gcc
ifeq ($(CC), cc)
	ifeq ($(DETECTED_OS),Darwin)
		CC = clang
	else
		CC = gcc
	endif
endif

# Allow to add arch-target for macOS CI cross compilation
ARCH_TARGET ?=

# BUILD FLAGS
CIVETWEB_BUILD_FLAGS := -o civetweb.o -I"$(MAKEFILE_DIR)/include/" -c "$(MAKEFILE_DIR)/src/civetweb/civetweb.c" -I"$(WEBUI_TLS_INCLUDE)" $(TLS_CFLAG) -w
CIVETWEB_DEFINE_FLAGS = -DNDEBUG -DNO_CACHING -DNO_CGI -DUSE_WEBSOCKET $(TLS_CFLAG)
WEBUI_BUILD_FLAGS := -o webui.o -I"$(MAKEFILE_DIR)/include/" -c "$(MAKEFILE_DIR)/src/webui.c" -I"$(WEBUI_TLS_INCLUDE)" $(TLS_CFLAG)
WARNING_RELEASE := -w
WARNING_LOG := -Wall -Wno-unused

# OUTPUT FILES
# The static output is the same for all platforms
# The dynamic output is platform dependent
LIB_STATIC_OUT := lib$(WEBUI_OUT_LIB_NAME)-static.a

# Platform defaults and dynamic library outputs
ifeq ($(DETECTED_OS),Windows)
	# Windows
	SHELL := CMD
	PLATFORM := windows
	LIB_DYN_OUT := $(WEBUI_OUT_LIB_NAME).dll
	LWS2_OPT := -lws2_32 -lole32
	CIVETWEB_DEFINE_FLAGS += -DMUST_IMPLEMENT_CLOCK_GETTIME
else ifeq ($(DETECTED_OS),Darwin)
	# MacOS
	PLATFORM := macos
	CC = clang
	LIB_DYN_OUT := $(WEBUI_OUT_LIB_NAME).dylib
	WEBKIT_OBJ := wkwebview.o
	WKWEBKIT_BUILD_FLAGS := -o wkwebview.o -c "$(MAKEFILE_DIR)/src/webview/wkwebview.m"
	WKWEBKIT_LINK_FLAGS := -framework Cocoa -framework WebKit
else
	# Linux
	PLATFORM := linux
	LIB_DYN_OUT := $(WEBUI_OUT_LIB_NAME).so
	ifeq ($(CC),clang)
		LLVM_OPT := llvm-
	endif
endif

# macOS can set `ARCH_TARGET=arm64` for cross-compilation.
ifneq ($(ARCH_TARGET),)
	ifneq ($(PLATFORM),macos)
		$(error ARCH_TARGET is only available on macOS)
	endif
	ifeq ($(ARCH_TARGET),arm64)
		TARGET := -target arm64-apple-darwin -arch arm64
	else ifeq ($(ARCH_TARGET),x64)
		TARGET := -target x86_64-apple-darwin -arch x86_64
	else
		$(error Unsupported ARCH_TARGET: $(ARCH_TARGET). Use 'arm64' or 'x64'.)
	endif
endif

# == 2.TARGETS ================================================================

all: release

debug: --debug

release: --release

clean: --clean-$(PLATFORM)

# == 2.1 INTERNAL TARGETS =====================================================

--debug:
# Create build directory
ifeq ($(PLATFORM),windows)
	@mkdir "$(BUILD_DIR)/debug" >nul 2>&1 ||:
else
	@mkdir -p "$(BUILD_DIR)/debug"
endif
#	Build macOS WKWebView
ifeq ($(DETECTED_OS),Darwin)
	@cd "$(BUILD_DIR)/debug" \
	&& echo "Build WebUI Objective-C WKWebKit ($(CC) $(TARGET) debug)..." \
	&& $(CC) $(TARGET) $(WKWEBKIT_BUILD_FLAGS) -g -DWEBUI_LOG
endif
#	Static with Debug info
	@cd "$(BUILD_DIR)/debug" \
	&& echo "Build WebUI library ($(CC) $(TARGET) debug static)..." \
	&& $(CC) $(TARGET) $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS) -g \
	&& $(CC) $(TARGET) $(WEBUI_BUILD_FLAGS) $(WARNING_LOG) -g -DWEBUI_LOG \
	&& $(LLVM_OPT)ar rc $(LIB_STATIC_OUT) webui.o civetweb.o $(WEBKIT_OBJ) \
	&& $(LLVM_OPT)ranlib $(LIB_STATIC_OUT)
#	Dynamic with Debug info
	@cd "$(BUILD_DIR)/debug" \
	&& echo "Build WebUI library ($(CC) $(TARGET) debug dynamic)..." \
	&& $(CC) $(TARGET) $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS) -g -fPIC \
	&& $(CC) $(TARGET) $(WEBUI_BUILD_FLAGS) $(WARNING_LOG) -g -fPIC -DWEBUI_LOG -DWEBUI_DYNAMIC \
	&& $(CC) $(TARGET) -shared -o $(LIB_DYN_OUT) webui.o civetweb.o $(WEBKIT_OBJ) -g -L"$(WEBUI_TLS_LIB)" $(TLS_LDFLAG_DYNAMIC) $(LWS2_OPT) $(WKWEBKIT_LINK_FLAGS)
ifeq ($(PLATFORM),windows)
	@cd "$(BUILD_DIR)/debug" && del *.o >nul 2>&1
else
	@rm -f $(BUILD_DIR)/debug/*.o
endif
	@echo "Done."

--release:
# Create build directory
ifeq ($(PLATFORM),windows)
	@mkdir "$(BUILD_DIR)" >nul 2>&1 ||:
else
	@mkdir -p "$(BUILD_DIR)"
endif
#	Build macOS WKWebView
ifeq ($(DETECTED_OS),Darwin)
	@cd "$(BUILD_DIR)" \
	&& echo "Build WebUI Objective-C WKWebKit ($(CC) $(TARGET) release)..." \
	&& $(CC) $(TARGET) $(WKWEBKIT_BUILD_FLAGS) -Os
endif
#	Static Release
	@cd "$(BUILD_DIR)" \
	&& echo "Build WebUI library ($(CC) $(TARGET) release static)..." \
	&& $(CC) $(TARGET) $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS) -Os \
	&& $(CC) $(TARGET) $(WEBUI_BUILD_FLAGS) $(WARNING_RELEASE) -Os \
	&& $(LLVM_OPT)ar rc $(LIB_STATIC_OUT) webui.o civetweb.o $(WEBKIT_OBJ) \
	&& $(LLVM_OPT)ranlib $(LIB_STATIC_OUT)
#	Dynamic Release
	@cd "$(BUILD_DIR)" \
	&& echo "Build WebUI library ($(CC) $(TARGET) release dynamic)..." \
	&& $(CC) $(TARGET) $(CIVETWEB_BUILD_FLAGS) $(CIVETWEB_DEFINE_FLAGS) -Os -fPIC \
	&& $(CC) $(TARGET) $(WEBUI_BUILD_FLAGS) $(WARNING_RELEASE) -O3 -fPIC -DWEBUI_DYNAMIC \
	&& $(CC) $(TARGET) -shared -o $(LIB_DYN_OUT) webui.o civetweb.o $(WEBKIT_OBJ) -L"$(WEBUI_TLS_LIB)" $(TLS_LDFLAG_DYNAMIC) $(LWS2_OPT) $(WKWEBKIT_LINK_FLAGS)
#	Clean
ifeq ($(PLATFORM),windows)
	@strip --strip-unneeded $(BUILD_DIR)/$(LIB_DYN_OUT)
	@cd "$(BUILD_DIR)" && del *.o >nul 2>&1
else
	@rm -f $(BUILD_DIR)/*.o
endif
	@echo "Done."

# PLATFROM CLEAN TARGETS

--clean-linux: --clean-unix

--clean-macos: --clean-unix

--clean-unix:
	@- cd "$(BUILD_DIR)" \
	&& rm -f *.o \
	&& rm -f *.so \
	&& rm -f *.dylib \
	&& rm -f *.a

--clean-windows:
	@- cd "$(BUILD_DIR)" \
	&& del *.a >nul 2>&1 \
	&& del *.o >nul 2>&1 \
	&& del *.dll >nul 2>&1 \
	&& del *.obj >nul 2>&1 \
	&& del *.ilk >nul 2>&1 \
	&& del *.pdb >nul 2>&1 \
	&& del *.lib >nul 2>&1 \
	&& del *.exp >nul 2>&1
