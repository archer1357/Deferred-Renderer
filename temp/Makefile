OUTDIR = bin
OUTFILE = $(OUTDIR)/demo
CC = gcc
EXT = c
CPPFLAGS := 
CXXFLAGS :=
LDFLAGS :=
#DEBUG_BUILD = -g
#DEBUG_TEST = gdb --eval-command=run

ifeq ($(OS),Windows_NT)
	CPPFLAGS += -DWIN32
	LDFLAGS += -lgdi32 -luser32 -lpthread -lkernel32 -lopengl32 -lglu32 -static
else
	UNAME_S := $(shell uname -s)

	ifeq ($(UNAME_S),Linux)
		CPPFLAGS += -DX11
		LDFLAGS += -ldl -lm -lX11 -lXi -lpthread -lGL
	endif

	ifeq ($(UNAME_S),Darwin)
		CPPFLAGS += -DOSX
	endif
endif


objs := $(patsubst %.$(EXT),$(OUTDIR)/%.o,$(wildcard *.$(EXT)))
deps := $(objs:.o=.dep)

.PHONY: all test
all: $(OUTFILE)

-include $(deps)

$(OUTDIR)/%.o: %.$(EXT)
	@mkdir -p $(@D)
	$(CC) $(DEBUG_BUILD) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
	@$(CC) $(CPPFLAGS) $(CXXFLAGS) -MM $< | sed -e '1,1 s|[^:]*:|$@:|' > $(OUTDIR)/$*.dep

$(OUTFILE) : $(objs)
	$(CC) $(DEBUG_BUILD) $^ $(LDFLAGS) -o $@

test: $(OUTFILE)
	@$(DEBUG_TEST) $(OUTFILE)

clean:
	@rm -f $(deps) $(objs) $(OUTFILE) $(OUTFILE).exe
	@rmdir --ignore-fail-on-non-empty $(OUTDIR)
