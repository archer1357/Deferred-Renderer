OUTDIR = bin
OUTFILE = $(OUTDIR)/DeferredRenderer
CC = g++ 

CPPFLAGS := -Iinclude
CXXFLAGS := -std=gnu++11
LDFLAGS := 

ifeq ($(OS),Windows_NT)
	CPPFLAGS += -D_WIN32  -DWIN32 -DUNICODE
	LDFLAGS += -lgdi32 -lopengl32 -luser32 -static  
	
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CPPFLAGS += -DAMD64
	endif

	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CPPFLAGS += -DIA32
	endif
else
	UNAME_S := $(shell uname -s)

	ifeq ($(UNAME_S),Linux)
		CPPFLAGS += -DLINUX -D_X11
		LDFLAGS += -ldl -lX11 -lXi -lGL
	endif

	ifeq ($(UNAME_S),Darwin)
		CPPFLAGS += -DOSX
	endif

	UNAME_P := $(shell uname -p)

	ifeq ($(UNAME_P),x86_64)
		CPPFLAGS += -DAMD64
	endif

	ifneq ($(filter %86,$(UNAME_P)),)
		CPPFLAGS += -DIA32
	endif

	ifneq ($(filter arm%,$(UNAME_P)),)
		CPPFLAGS += -DARM
	endif
endif

objs := $(patsubst %.cpp,$(OUTDIR)/%.o,$(wildcard *.cpp))
deps := $(objs:.o=.dep)

.PHONY: all test
all: $(OUTFILE)

-include $(deps)

$(OUTDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
	@$(CC) $(CPPFLAGS) $(CXXFLAGS) -MM $< | sed -e '1,1 s|[^:]*:|$@:|' > $(@D)/$*.dep

$(OUTFILE) : $(objs)
	$(CC) $^ $(LDFLAGS) -o $@

test: $(OUTFILE)
	@$(OUTFILE)

clean:
	@rm -f $(deps) $(objs) $(OUTFILE)
	@rmdir --ignore-fail-on-non-empty $(OUTDIR)
