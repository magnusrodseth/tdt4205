VSLC := ../src/vslc

PS2_EXAMPLES := $(patsubst %.vsl, %.ast, $(wildcard ps2-parser/*.vsl))
PS2_GRAPHVIZ := $(patsubst %.vsl, %.svg, $(wildcard ps2-parser/*.vsl))
PS3_EXAMPLES := $(patsubst %.vsl, %.ast, $(wildcard ps3-simplify/*.vsl))
PS3_GRAPHVIZ := $(patsubst %.vsl, %.svg, $(wildcard ps3-simplify/*.vsl))
PS4_EXAMPLES := $(patsubst %.vsl, %.symbols, $(wildcard ps4-symbols/*.vsl))
PS5_EXAMPLES := $(patsubst %.vsl, %.S, $(wildcard ps5-codegen1/*.vsl))
PS5_ASSEMBLED := $(patsubst %.vsl, %.out, $(wildcard ps5-codegen1/*.vsl))

.PHONY: all ps2 ps2-graphviz ps3 ps3-graphviz ps4 ps5 ps5-assemble clean ps2-check

all: ps2 ps3 ps4 ps5

ps2: $(PS2_EXAMPLES)
ps2-graphviz: $(PS2_GRAPHVIZ)

ps3: $(PS3_EXAMPLES)
ps3-graphviz: $(PS3_GRAPHVIZ)

ps4: $(PS4_EXAMPLES)
ps4-graphviz: $(PS4_GRAPHVIZ)
ps4-check: ps4
	cd ps4-symbols; \
	for file in *.symbols; do \
    diff -s $$file suggested/$$file; \
    if [ $$? -ne 0 ]; then \
    		echo "Files $$file and suggested/$$file are NOT identical"; \
        exit 1; \
		fi; \
	done; \
	if [ $$? -eq 0 ]; then \
		echo "No differences found in PS4!"; \
	fi \

ps5: $(PS5_EXAMPLES)
ps5-assemble: $(PS5_ASSEMBLED)

ps2-parser/%.ast: ps2-parser/%.vsl $(VSLC)
	$(VSLC) -t < $< > $@

ps3-simplify/%.ast: ps3-simplify/%.vsl $(VSLC)
	$(VSLC) -T < $< > $@

%.svg: %.vsl $(VSLC)
	GRAPHVIZ_OUTPUT=1 $(VSLC) -T < $< | dot -Tsvg -o $@

%.symbols: %.vsl $(VSLC)
	$(VSLC) -s < $< > $@

%.S: %.vsl $(VSLC)
	$(VSLC) -c < $< > $@

%.out: %.S
	gcc -no-pie $< -o $@

clean:
	-rm -rf */*.ast */*.svg */*.symbols */*.S */*.out

ps2-check: ps2
	cd ps2-parser; \
	find * -name "*.suggested" | awk -F. '{print $$1"."$$2 " " $$0}' | xargs -L 1 diff -s --unified=0
	@echo "No differences found in PS2!"
