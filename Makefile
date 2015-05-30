CPPFLAGS = -Wall -g -MMD -fno-stack-protector
LDFLAGS = -g
CXX = g++

OBJS = main.o sim.o report.o user.o topo.o binsearch.o \
	trafficgen.o userVisitor.o coopTable.o ctableEntry.o \
	packetfactory.o packet.o agent.o packethandler.o 

SOURCES = main.cpp user.cpp trafficgen.cpp sim.cpp topo.cpp \
	binsearch.cpp userVisitor.cpp coopTable.cpp ctableEntry.cpp \
	packetfactory.cpp packet.cpp agent.cpp packethandler.cpp \
	report.cpp 

all: coopRoute

coopRoute: $(OBJS)
	$(CXX) -o $@ $^


-include $(subst .cpp,.d,$(SOURCES))

%.d: %.cpp
	$(CXX) -M $(CPPFLAGS) -MF $@ $< -MT "$*.o $@"

clean:
	rm -f coopRoute $(OBJS) *.d *.d.*
