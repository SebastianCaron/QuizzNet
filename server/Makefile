COMPILER := gcc
SRC_DIR ?= src
BIN_DIR ?= bin

DEBUG ?= 1
ifeq '$(DEBUG)' '1'
CFLAGS ?= -Wall -MMD -g
else
CFLAGS ?= -Wall -MMD -03 -DNDEBUG
endif

LDFLAGS = -ldl -lpthread

OBJS := $(shell find $(SRC_DIR) -name "*.c" | sed 's/.c$$/.o/g' | sed 's/$(SRC_DIR)/$(BIN_DIR)/g')
DEPS := $(OBJS:.o=.d)

TARGET ?= server

all: $(TARGET)
	@echo "BUILD OK"

$(TARGET): $(OBJS)
	$(COMPILER) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(COMPILER) $(CFLAGS) -c $< -o $@

.PHONY: clean mrpropre fr0
clean:
	rm -rf $(BIN_DIR)

mrpropre: clean
	clear
	rm -f $(TARGET)
	rm -rf val.txt
	rm -f test.db
	rm -f *.log

aspi:
	clear
	rm -f val.txt
	rm -f test.db
	rm -f *.log


fr0: mrpropre all
	clear
	./server 'les_12_coups' 9504

start: all aspi
	./server 'les_12_coups' 9504 -log

debug: all
	clear
	valgrind --leak-check=full --log-file=val.txt ./server 'les_12_coups' 9504

-include $(DEPS)