#
TARGET = HPS_FPGA_LED

#
ALT_DEVICE_FAMILY ?= soc_cv_av
SOCEDS_ROOT ?= $(SOCEDS_DEST_ROOT)
HWLIBS_ROOT = /home/root/hwlib

CFLAGS = -ggdb -Wall -std=c++11 `pkg-config  --cflags opencv` -D$(ALT_DEVICE_FAMILY) -I$(HWLIBS_ROOT)/include/$(ALT_DEVICE_FAMILY)   -I$(HWLIBS_ROOT)/include/

LDFLAGS =  -ggdb -Wall -std=c++11 `pkg-config  --libs opencv`
CC = g++
ARCH= arm

build: $(TARGET)
$(TARGET): main.o 
	$(CC) $(LDFLAGS) -o $@ $^ `pkg-config  --libs opencv` -lpthread
%.o : %.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 

.PHONY: clean
clean:
	rm -f $(TARGET) *.a *.o *~ 
