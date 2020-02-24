
NAME := greencloud.0.5
SLAVE := slave.out

SRC_DIR = src
OBJ_DIR = obj
HED_DIR = include
LIB_DIR = lib
SO_DIR = /usr/lib
RELEASE_DIR = release
TEST_DIR = test

VPATH = $(TEST_DIR):$(OBJ_DIR):$(SRC_DIR):$(RELEASE_DIR)

GLOBAL_LIB := /usr/lib/libglobals.so
GLOBAL_SRC := ./lib/libglobals.cpp
GLOBAL_OBJ := ./libglobals.o

LIB := /usr/lib/libgreencloud.so

SRC := $(wildcard $(SRC_DIR)/*.cpp)
HEDS := $(wildcard $(HED_DIR)/*.hpp)
EXE := $(NAME).out
NAME_SRC := $(NAME).cpp
OBJ := $(notdir $(SRC:.cpp=.o))
OBJS :=$(addprefix $(OBJ_DIR)/,$(OBJ))
TESTS := $(wildcard $(TEST_DIR)/*.cpp)

CC = g++
# CC = arm-linux-gnueabihf-g++
CFLAGS = -c -std=c++11 -pedantic-errors -Wall -Wextra -g
LFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -g -pthread
DLFLAGS = -lglobals -ldl

.SECONDARY: $(wildcard *.o)
.SUFFIXES:
.PHONY: all clean re

vpath %.cpp $(SRC_DIR)
vpath %.o $(OBJ_DIR)
vpath %.hpp $(HED_DIR)
vpath %.so $(SO_DIR)

all: $(EXE)

%.o: %.cpp %.hpp
	$(CC) $(CFLAGS) -fPIC $< -I $(HED_DIR)
	mv *.o $(OBJ_DIR)

$(LIB): $(OBJ) $(SRC) $(GLOBAL_LIB) $(HEDS)
	$(CC) $(LFLAGS) -shared $(OBJS) -o $(notdir $@) $(DLFLAGS) -I $(HED_DIR)
	sudo mv $(notdir $@) /usr/lib/

$(EXE): $(NAME_SRC) $(GLOBAL_LIB) $(LIB)
	$(CC) $(LFLAGS) $< -o $@ $(DLFLAGS) -lgreencloud -I $(HED_DIR)

$(GLOBAL_LIB): $(GLOBAL_SRC)
	$(CC) $(CFLAGS) -fPIC $^ -I $(HED_DIR)
	$(CC) $(LFLAGS) -shared $(GLOBAL_OBJ) -o $(notdir $@) -I $(HED_DIR)
	rm $(GLOBAL_OBJ)
	sudo mv $(notdir $@) /usr/lib/

slave: $(SLAVE:.out=.cpp) $(OBJ) $(SRC) $(GLOBAL_LIB)
	$(CC) $(LFLAGS) $< $(OBJS) -o $(SLAVE) $(DLFLAGS) -I $(HED_DIR)



ton: $(GLOBAL_LIB)
	$(CC) $(CFLAGS) -fPIC ./src/logger.cpp ./test/foo_lib.cpp -I $(HED_DIR)
	mv *.o $(OBJ_DIR)
	$(CC) $(LFLAGS) -shared ./obj/foo_lib.o ./obj/logger.o -o libFoo.so -lglobals
	sudo mv libFoo.so /usr/lib/
	$(CC) $(LFLAGS) ./src/logger.cpp ./test/handleton_test.cpp -o $@.out -lglobals -ldl -I $(HED_DIR)

logger:
	$(CC) $(LFLAGS) ./src/logger.cpp ./test/logger_test.cpp -o $@.out -ldl -I $(HED_DIR)

fac:
	$(CC) $(LFLAGS) ./test/factory_test.cpp -o $@.out -I $(HED_DIR)

pq:
	$(CC) $(LFLAGS) ./test/wpq.cpp ./src/semaphore.cpp -o $@.out -I $(HED_DIR)

tp: $(EXE)
	$(CC) $(LFLAGS) ./test/thread_pool_test.cpp ./obj/*.o -o $@.out -lglobals -I $(HED_DIR)

dis: $(EXE)
	$(CC) $(LFLAGS) ./test/dispatcher_callback_test.cpp ./obj/*.o -o $@.out $(DLFLAGS) -I $(HED_DIR)


clean:
	rm -f *.out
	rm -f $(OBJ_DIR)/*.o
	sudo rm -f $(LIB)
	sudo rm -f $(GLOBAL_LIB)

re: clean all
