
BIN=bin/
SRC=src/
CFLAGS=-Wall
CC=g++
STD11FLAG=-std=gnu++11
cc_begin=$(CC) $(1) $(CFLAGS)

INCLUDE=include/

UTIL_DIR=util/
UTIL_BIN=$(BIN)$(UTIL_DIR)
UTIL_SRC=$(SRC)$(UTIL_DIR)

SIGNAL_DIR=signal/
SIGNAL_BIN=$(BIN)$(SIGNAL_DIR)
SIGNAL_SRC=$(SRC)$(SIGNAL_DIR)

HOTKEY_DIR=hotkey/
HOTKEY_BIN=$(BIN)$(HOTKEY_DIR)
HOTKEY_SRC=$(SRC)$(HOTKEY_DIR)

TEST_DIR=tests/
TEST_SRC=$(SRC)$(TEST_DIR)
TEST_BIN=$(BIN)$(TEST_DIR)

MKDIR_P=mkdir -p

# Output filenames
UTIL_PROG=event.o util.o
SIGNAL_PROG=isignal.o hardsignal.o isoftsignal.o mods.o keysignal.o spacesignal.o scriptsignal.o comsignal.o noop.o
HOTKEY_PROG=hotkey.o aintercept.o keyintercept.o keyeventspecified.o
TEST_PROG=signaltest hotkeytestlnx

# All modules, not including tests
LIST=directories \
$(addprefix $(UTIL_BIN), $(UTIL_PROG)) \
$(addprefix $(SIGNAL_BIN), $(SIGNAL_PROG)) \
$(addprefix $(HOTKEY_BIN), $(HOTKEY_PROG))

all: $(LIST)

# Ease of use definitions
# Util
event: directories $(UTIL_BIN)event.o

util: $(addprefix $(UTIL_BIN), $(UTIL_PROG))
# Signal
mods: directories $(SIGNAL_BIN)mods.o

keysignal: directories $(SIGNAL_BIN)keysignal.o

spacesignal: directories $(SIGNAL_BIN)spacesignal.o

scriptsignal: directories $(SIGNAL_BIN)scriptsignal.o

comsignal: directories $(SIGNAL_BIN)comsignal.o
	
signal: util $(addprefix $(SIGNAL_BIN), $(SIGNAL_PROG))
# Hotkey
keyeventspecified: hotkey

hotkey: signal $(addprefix $(HOTKEY_BIN), $(HOTKEY_PROG))

# And here come the tests
signaltest: signal $(SIGNAL_BIN)signaltest

hotkeytestlnx: hotkey $(TEST_BIN)hotkeytestlnx

hotkeytest: hotkey $(TEST_BIN)hotkeytest

tests=signaltest hotkeytest

# Util
INCY=/usr/include/ $(UTIL_BIN) $(SIGNAL_BIN) $(HOTKEY_BIN) $(UTIL_SRC) $(SIGNAL_SRC) $(HOTKEY_SRC) $(INCLUDE)
INC=$(addprefix -I, $(INCY))
cc-command=$(call cc_begin, $(INC)) $< -o $@ $(STD11FLAG)
$(UTIL_BIN)%.o: $(UTIL_SRC)%.cpp
	$(cc-command) -c

$(UTIL_BIN)event.o: $(UTIL_SRC)event.h
	$(cc-command) -c
	
# Signal
# INCY=usr/include/ $(UTIL_SRC) $(UTIL_BIN) $(SIGNAL_SRC) $(SIGNAL_BIN)
# INC=$(addprefix -I/, $(INCY))
# cc-command=$(call cc_begin, $(INC)) $< -o $@ $(STD11FLAG)
$(SIGNAL_BIN)%.o: $(SIGNAL_SRC)%.cpp
	$(cc-command) -c

$(TEST_BIN)signaltest: $(addprefix $(SIGNAL_BIN), $(SIGNAL_PROG)) $(SIGNAL_SRC)main.cpp
	$(cc-command)
#	$(CC) $(INC) $(CFLAGS) \
#	$(addprefix $(SIGNAL_BIN), $(SIGNAL_PROG)) \
#	$< \
#	-o $@ -std=gnu++11
	
# Hotkey
# INCY=usr/include/ $(UTIL_SRC) $(UTIL_BIN) $(SIGNAL_SRC) $(SIGNAL_BIN) $(HOTKEY_SRC) $(HOTKEY_BIN)
# INC=$(addprefix -I/, $(INCY))
# cc-command=$(call cc_begin, $(INC)) $< -o $@ $(STD11FLAG)
$(HOTKEY_BIN)%.o: $(HOTKEY_SRC)%.cpp
	$(cc-command) -c

$(TEST_BIN)hotkeytestlnx: $(addprefix $(SIGNAL_BIN), $(SIGNAL_PROG)) $(addprefix $(HOTKEY_BIN), $(HOTKEY_PROG)) $(HOTKEY_SRC)hotkeytestlnx.cpp
	$(cc-command)
	# $(CC) $(INC) $(CFLAGS) \
	# $(addprefix $(SIGNAL_BIN), $(SIGNAL_PROG)) \
	# $(addprefix $(HOTKEY_BIN), $(HOTKEY_PROG)) \
	# $< \
	# -o $@ -lX11 -std=gnu++11

#.PHONY: directories
directories: ${UTIL_BIN} ${SIGNAL_BIN} ${HOTKEY_BIN} ${TEST_BIN}
	
${UTIL_BIN}:
	${MKDIR_P} ${UTIL_BIN}
	
${SIGNAL_BIN}:
	${MKDIR_P} ${SIGNAL_BIN}
	
${HOTKEY_BIN}:
	${MKDIR_P} ${HOTKEY_BIN}

${TEST_BIN}:
	${MKDIR_P} ${TEST_BIN}
	
.PHONY: clean
clean:
	rm -rf bin
	echo Entire build directory has been removed.

.PHONY: clean-temp
clean-temp:
	rm -f *~
	rm -rf $(SRC)*~
	echo Temp files ending with ~ have been removed from here and everything in $(SRC)