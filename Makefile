
#
# Compiler flags
#
CC     = g++
CFLAGS = -Wall

#
# Project files
#
SRCS = Hue.cpp CReadFile.cpp CJson.cpp CHue.cpp
OBJS = $(SRCS:.cpp=.o)
EXE  = Hue

#
# Debug build settings
#
DBGDIR = debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCFLAGS = -g -O0 -DDEBUG

#
# Release build settings
#
RELDIR = release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCFLAGS = -O3 -DNDEBUG 

# Default build
all: prep release

#
# Debug rules
#
debug: prep $(DBGEXE)
$(DBGEXE): $(DBGOBJS)
	@mkdir -p $(DBGDIR)
	$(CC) $(CFLAGS) $(DBGCLAGS) -o $(EXE) $^ `pkg-config --libs libcurl`

$(DBGDIR)/%.o: %.cpp
	@mkdir -p $(DBGDIR)
	$(CC) -c $(CFLAGS) `pkg-config --cflags libcurl` $(DBGCFLAGS) -o $@ $<

#
# Release rules
#
release: prep $(RELEXE)
$(RELEXE): $(RELOBJS)
	@mkdir -p $(RELDIR)
	$(CC) $(CFLAGS) $(RELCFLAGS) -o $(EXE) $^ `pkg-config --libs libcurl`

$(RELDIR)/%.o: %.cpp
	@mkdir -p $(RELDIR)
	$(CC) -c $(CFLAGS) `pkg-config --cflags libcurl` $(RELCFLAGS) -o $@ $<

#
# Other rules
#
prep:
	@mkdir -p $(DBGDIR)
	@mkdir -p $(RELDIR)

clean:
#rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS)
	rm -f -r $(DBGDIR) $(RELDIR)