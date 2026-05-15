out = build
CFLAGS += -g -lraylib -lm

.PHONY: all
all: main

%: %.c
	@mkdir -p $(out)/
	cc $(CFLAGS) -o $(out)/$@ $^

.PHONY: clean
clean:
	rm -rf $(out)/
