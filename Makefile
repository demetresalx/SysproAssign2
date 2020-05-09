CC=g++  -std=c++11
CFLAGS=-c -Wpedantic #-Ofast
LDFLAGS=
SOURCES=diseaseAggregator.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=diseaseAggregator



#syspro
#kuriws
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

run:
	./$(EXECUTABLE) -i ../inputs/ass2/_medium.txt -w 20 -b 128

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(TESTEXEC)


#GIA TO BASH 
#./create_infiles.sh ../inputs/ass2/disfile ../inputs/ass2/coufile ../inputs/ass2/input_dir 4 10
