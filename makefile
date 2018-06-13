CXXFLAGS += -O2 -Wall -g -pthread

all: test

test: RCSwitch.o RcOok.o Sensor.o test.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi -lmosquitto

clean:
	$(RM) *.o test
