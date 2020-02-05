
NAME := greencloud.0.2

SRC_DIR = src
OBJ_DIR = obj
HED_DIR = include
LIB_DIR = lib
RELEASE_DIR = release
TEST_DIR = test

VPATH = $(TEST_DIR):$(OBJ_DIR):$(SRC_DIR):$(RELEASE_DIR)

GLOBAL_LIB := /usr/lib/libglobals.so
GLOBAL_SRC := ./lib/libglobals.cpp
GLOBAL_OBJ := ./libglobals.o

SRC := $(wildcard $(SRC_DIR)/*.cpp)
EXE := $(NAME).out
NAME_SRC := $(NAME).cpp
OBJ := $(notdir $(SRC:.cpp=.o))
LIB := $(LIB_DIR)/lib.a
OBJS :=$(addprefix $(OBJ_DIR)/,$(OBJ))
TESTS := $(wildcard $(TEST_DIR)/*.cpp)

CC = g++
CFLAGS = -c -std=c++11 -pedantic-errors -Wall -Wextra -g
LFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -g -pthread
DLFLAGS = -lglobals

.SECONDARY: $(wildcard *.o)
.SUFFIXES:
.PHONY: all clean re

vpath %.cpp $(SRC_DIR)
vpath %.o $(OBJ_DIR)
vpath %.hpp $(HED_DIR)

all: $(EXE)

%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) $< -I $(HED_DIR)
	mv *.o $(OBJ_DIR)

#$(LIB): $(OBJ)
#	ar rc $@ $(addprefix $(OBJ_DIR)/,$(OBJ))

$(EXE): $(NAME_SRC) $(OBJ) $(GLOBAL_LIB)
	$(CC) $(LFLAGS) $< $(OBJS) -o $@ $(DLFLAGS) -I $(HED_DIR)

$(GLOBAL_LIB): $(GLOBAL_SRC)
	$(CC) $(CFLAGS) -fPIC $^ -I $(HED_DIR)
	$(CC) $(LFLAGS) -shared $(GLOBAL_OBJ) -o $(notdir $@) -I $(HED_DIR)
	rm $(GLOBAL_OBJ)
	sudo mv $(notdir $@) /usr/lib/


ton: $(GLOBAL_LIB)
	$(CC) $(CFLAGS) -fPIC ./test/foo_lib.cpp -I $(HED_DIR)
	mv *.o $(OBJ_DIR)
	$(CC) $(LFLAGS) -shared ./obj/foo_lib.o -o libFoo.so -lglobals
	sudo mv libFoo.so /usr/lib/
	$(CC) $(LFLAGS) ./test/handleton_test.cpp -o $@.out -lglobals -ldl -I $(HED_DIR)

logger:
	$(CC) $(LFLAGS) ./src/logger.cpp ./test/logger_test.cpp -o $@.out -ldl -I $(HED_DIR)

fac:
	$(CC) $(LFLAGS) ./test/factory_test.cpp -o $@.out -I $(HED_DIR)

pq:
	$(CC) $(LFLAGS) ./test/wpq.cpp ./src/semaphore.cpp -o $@.out -I $(HED_DIR)
tp: $(EXE)
	$(CC) $(LFLAGS) ./test/thread_pool_test.cpp ./obj/*.o -o $@.out -lglobals -I $(HED_DIR)


clean:
	rm *.out
#	rm $(LIB_DIR)/*.a
	rm $(OBJ_DIR)/*.o

re: clean all
