
$PBDIR=/usr/local
$RJDIR=/usr/local

CXXFLAGS=-I../include -I$(PBDIR)/include  -I$(RJDIR)/include
PROTOC=$(PBDIR)/bin/protoc
LDFLAGS=-L$(PBDIR)/lib -lprotobuf

all: test

test.pb.h: test.pb.cc

test.cpp: test.pb.h	

test.pb.cc: test.proto
	$(PROTOC) --cpp_out=. test.proto

test: test.o test.pb.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o *.so test *.pb.cc *.pb.h