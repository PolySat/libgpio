PLAT=$(shell uname -s)
include Make.rules.$(PLAT)

override CFLAGS+= -Wall -std=c99 -pedantic
INCLUDES=gpioapi.h
MAJOR_VERS=1
MINOR_VERS=0
LIB_NAME=libgpio.$(SO_EXT).$(MAJOR_VERS).$(MINOR_VERS)

ifeq ($(TARGET),arm)
	#CC_PATH=/home/polysat/Code/polysat_linux/build-tools/buildroot/output/staging/usr
	CC=$(ARM_TOOLCHAIN_PATH)/$(ARM_BIN_SUBPATH)cc
	STRIP=$(ARM_TOOLCHAIN_PATH)/$(ARM_BIN_SUBPATH)strip
	LIB_PATH=$(ARM_TOOLCHAIN_PATH)/lib
	INC_PATH=$(ARM_TOOLCHAIN_PATH)/include
else
	LIB_PATH=/usr/lib
	INC_PATH=/usr/include
	BIN_PATH=/usr/bin
	STRIP=strip
	SBIN_PATH=/usr/sbin
endif

all: gpiotest $(LIB_NAME)

arm: TARGET=arm
arm: all

gpiotest: gpiotest.o $(LIB_NAME)
	$(CC) $(LDFLAGS) -L. -o $@ gpiotest.o -lgpio
	$(STRIP) $@

gpioapi.o: override CFLAGS += $(SO_CFLAGS)
$(LIB_NAME): gpioapi.o
	$(CC) $(SO_LDFLAGS) $(LDFLAGS) $< -o $@
	$(STRIP) $@
	ln -sf $(LIB_NAME) libgpio.$(SO_EXT)
	ln -sf $(LIB_NAME) libgpio.$(SO_EXT).$(MINOR_VERS)

install: gpiotest $(LIB_NAME)
	cp $(LIB_NAME) $(LIB_PATH)
	ln -sf $(LIB_NAME) $(LIB_PATH)/libgpio.$(SO_EXT)
	ln -sf $(LIB_NAME) $(LIB_PATH)/libgpio.$(SO_EXT).$(MAJOR_VERS)
	install -d $(INC_PATH)
	cp $(INCLUDES) $(INC_PATH)
	cp gpiotest $(BIN_PATH)

.PHONY: clean

clean:
	rm -rf core* *.o gpiotest libgpio.*
