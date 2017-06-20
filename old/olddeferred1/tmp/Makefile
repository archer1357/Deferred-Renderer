OUTDIR = bin
OUTFILE = $(OUTDIR)/Test
CC = g++

CPPFLAGS := -I/usr/local/include -DGLCOREARB_PROTOTYPES
LDFLAGS := -L/usr/local/lib

CPPFLAGS+= -I/usr/include/bullet -I/usr/local/include/bullet
LDFLAGS += -lBulletDynamics -lBulletCollision -lLinearMath

CPPFLAGS+= -I/usr/include/luajit-2.0 -I/usr/local/include/luajit-2.0


ifeq ($(OS),Windows_NT)
	CPPFLAGS += -DWIN32
	LDFLAGS+= -lglew32 -lglfw3 -lopengl32 -lua51
	#-lgdi32 -lcomctl32 -luser32 -lole32
else
	UNAME_S := $(shell uname -s)
	
	LDFLAGS+= -ldl -lglfw -lGL -lGLEW -lluajit-5.1

	ifeq ($(UNAME_S),Linux)
		CPPFLAGS += -DLINUX
	endif

	ifeq ($(UNAME_S),Darwin)
		CPPFLAGS += -DOSX
	endif
endif

objs := $(patsubst Source/%.cpp,$(OUTDIR)/%.o,$(wildcard Source/*.cpp))
deps := $(objs:.o=.dep)

.PHONY: all test
all: $(OUTFILE)

-include $(deps)

$(OUTDIR)/%.o: Source/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) -c $< -o $@
	@$(CC) $(CPPFLAGS) -MM $< | sed -e '1,1 s|[^:]*:|$@:|' > $(@D)/$*.dep

$(OUTFILE) : $(objs)
	$(CC) $^ $(LDFLAGS) -o $@

test: $(OUTFILE)
	@export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
	@$(OUTFILE)

clean:
	@rm -f $(deps) $(objs) $(OUTFILE)
	@rmdir --ignore-fail-on-non-empty $(OUTDIR)
