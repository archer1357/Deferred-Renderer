OUTDIR = bin
OUTFILE = $(OUTDIR)/DeferredRenderer
CC = g++

CPPFLAGS := -DGLCOREARB_PROTOTYPES -std=c++11 -Iinclude
LDFLAGS := 

ifeq ($(OS),Windows_NT)
	CPPFLAGS += -DWIN32
	LDFLAGS += -lopengl32
	
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CCFLAGS += -DAMD64
	endif

	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CCFLAGS += -DIA32
	endif
else
	UNAME_S := $(shell uname -s)
	LDFLAGS += -ldl -lX11 -lGL

	ifeq ($(UNAME_S),Linux)
		CPPFLAGS += -DLINUX
	endif

	ifeq ($(UNAME_S),Darwin)
		CPPFLAGS += -DOSX
	endif

	UNAME_P := $(shell uname -p)

	ifeq ($(UNAME_P),x86_64)
		CCFLAGS += -DAMD64
	endif

	ifneq ($(filter %86,$(UNAME_P)),)
		CCFLAGS += -DIA32
	endif

	ifneq ($(filter arm%,$(UNAME_P)),)
		CCFLAGS += -DARM
	endif
endif

objs := $(patsubst %.cpp,$(OUTDIR)/%.o,$(wildcard *.cpp))
deps := $(objs:.o=.dep)

.PHONY: all test
all: $(OUTFILE)

-include $(deps)

$(OUTDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $< -o $@
	@$(CC) $(CPPFLAGS) -MM $< | sed -e '1,1 s|[^:]*:|$@:|' > $(@D)/$*.dep

$(OUTFILE) : $(objs)
	$(CC) $^ $(LDFLAGS) -o $@

test: $(OUTFILE)
	@$(OUTFILE)

clean:
	@rm -f $(deps) $(objs) $(OUTFILE)
	@rmdir --ignore-fail-on-non-empty $(OUTDIR)
