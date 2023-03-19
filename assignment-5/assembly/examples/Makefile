.PHONY: all

SRCS = $(wildcard *.s)

BINS = $(patsubst %.s,%,$(SRCS))

all: $(BINS)

clean:
	rm $(BINS)

%: %.s
	$(CC) -no-pie $< -o $@
