OUTDIR = bin
OUTPUT=$(OUTDIR)/demo
CC=emcc
EXT = c
CPPFLAGS :=

srcs:=$(wildcard *.$(EXT))
hdrs:=$(wildcard *.h)

all: $(srcs) $(OUTPUT).html

.PHONY: all test

$(OUTPUT).html: $(srcs) $(hdrs)
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(srcs) --emrun -o $(OUTPUT).html

test:
	emrun $(OUTPUT).html

clean:
	rm $(OUTPUT).html $(OUTPUT).js $(OUTPUT).data