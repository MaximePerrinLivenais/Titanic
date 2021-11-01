CXX = mpic++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -g
CPPFLAGS = -Isrc -Ilib

TARGET = titanic

CLIENT = \
         src/client/client.cc \
         src/client/client-message.cc \
         src/client/client-request.cc \
         src/client/client-response.cc \
         $(NULL)

MESSAGE = \
         src/message/message.cc \
         $(NULL)

RAFT = \
		src/process/process.cc \
        src/raft/server.cc \
        $(NULL)

REPL = \
        src/repl/repl.cc \
        src/repl/repl-message.cc \
        src/repl/request-crash-repl.cc \
        src/repl/request-start-repl.cc \
        src/repl/request-speed-repl.cc \
        $(NULL)

RPC = \
        src/rpc/append-entries.cc \
        src/rpc/append-entries-response.cc \
        src/rpc/log-entry.cc \
        src/rpc/request-vote.cc \
        src/rpc/request-vote-response.cc \
        src/rpc/rpc.cc \
        $(NULL)

UTILS = \
        src/utils/chrono/chrono.cc \
        src/utils/openmpi/mpi-wrapper.cc \
        $(NULL)

SRC =  \
        src/main.cc \
        $(CLIENT) \
        $(MESSAGE) \
        $(RAFT) \
        $(REPL) \
        $(RPC) \
        $(UTILS) \
        $(NULL)

OBJS = $(SRC:.cc=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^

clean:
	$(RM) $(TARGET) $(OBJS)
