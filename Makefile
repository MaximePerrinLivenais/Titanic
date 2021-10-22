CXX = mpic++
CXXFLAGS = -std=c++20 -Wall -Werror -Wextra -pedantic
CPPFLAGS = -Isrc -Ilib

TARGET = titanic

RPC = \
        src/rpc/append-entries.cc \
        src/rpc/append-entries-response.cc \
        src/rpc/request-vote.cc \
        src/rpc/request-vote-response.cc \
        src/rpc/rpc.cc \
        src/rpc/log-entry.cc \
        $(NULL)

CLIENT = \
         src/client/client.cc \
         $(NULL)



UTILS = \
        src/utils/openmpi/mpi-wrapper.cc \
		src/utils/chrono/chrono.cc \
        $(NULL)

RAFT = \
		src/raft/server.cc \
		$(NULL)

SRC =  \
        src/hello-world.cc \
        $(RPC) \
        $(UTILS) \
        $(CLIENT) \
        $(RAFT) \
        $(NULL)

OBJS = $(SRC:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^

clean:
	$(RM) $(TARGET) $(OBJS)
