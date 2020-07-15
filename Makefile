CC=g++  -std=c++11
CFLAGS=-c -Wpedantic #-Ofast
LDFLAGS=
SOURCES=diseaseAggregator.cpp worker.cpp utils.cpp boss.cpp record.cpp record_HT.cpp cdHashTable.cpp bbst.cpp topk.cpp
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
	./$(EXECUTABLE) -i ../inputs/ass2/input_dir -w 4 -b 128

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(TESTEXEC)


#GIA TO BASH
#./create_infiles.sh ../inputs/ass2/disfile ../inputs/ass2/coufile ../inputs/ass2/input_dir 4 10
#../inputs/ass2/input_dir

#/home/demetres/Desktop/bashforLarge/input_dir
#valgrind
#valgrind --leak-check=full ./diseaseAggregator -i ../inputs/ass2/input_dir -w 3 -b 128
