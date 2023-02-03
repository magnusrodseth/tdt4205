VSLC := ../src/vslc

PS2_EXAMPLES := $(patsubst ps2-parser/%.vsl, ps2-parser/%.ast, $(wildcard ps2-parser/*.vsl))

.PHONY: all check clean

all: $(PS2_EXAMPLES)
	@echo $(PS2_EXAMPLES)

%.ast: %.vsl
	$(VSLC) -t < $^ > $@

clean:
	-rm -r */*.ast

check: all
	cd ps2-parser; \
	find * -name "*.suggested" | awk -F. '{print $$1"."$$2 " " $$0}' | xargs -L 1 diff -s --unified=0
	@echo "No differences found!"
