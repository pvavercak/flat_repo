CC=gcc
CXX=g++

TARGET_NAME=BioMiniSDKDemo
TARGET=$(BIN_PATH)/$(TARGET_NAME)

UFSCANNERPATH=../../src/Linux/UFScanner
BIN_PATH=../../bin
INC_PATH=../../include

SRCS=BioMiniSDKDemo.cpp
OBJS=BioMiniSDKDemo.o

CFLAGS=-O2 -I $(INC_PATH)
LDFLAGS=-L$(BIN_PATH)


all: $(TARGET)

.SUFFIXES: .cpp .c .o

.c.o:
	$(CC) -c $(CFLAGS) $<

.cpp.o:
	$(CXX) -c $(CFLAGS) $<

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS) -lUFScanner -lUFMatcher -lusb-1.0 -lpthread

run: $(TARGET)
	cd $(BIN_PATH) && LD_LIBRARY_PATH=".:$${LD_LIBRARY_PATH}" ./$(TARGET_NAME)
