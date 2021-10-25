CXX = mpic++
CXXFLAGS = -std=c++20 -Wall -Werror -Wextra -pedantic -g
CPPFLAGS = -Isrc -Ilib

TARGET = titanic

RPC = \
	src/message/message.cc \
        src/rpc/append-entries.cc \
        src/rpc/append-entries-response.cc \
        src/rpc/request-vote.cc \
        src/rpc/request-vote-response.cc \
        src/rpc/rpc.cc \
        src/rpc/log-entry.cc \
        $(NULL)

REPL = \
        src/repl/repl.cc \
        src/repl/repl-message.cc \
        src/repl/request-crash-repl.cc \
        src/repl/request-start-repl.cc \
        $(NULL)


CLIENT = \
         src/client/client.cc \
         src/client/client-message.cc \
         src/client/client-request.cc \
         src/client/client-response.cc \
         $(NULL)



UTILS = \
        src/utils/openmpi/mpi-wrapper.cc \
		src/utils/chrono/chrono.cc \
        $(NULL)

RAFT = \
		src/raft/server.cc \
		$(NULL)

SRC =  \
        src/main.cc \
        $(RPC) \
        $(REPL) \
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
