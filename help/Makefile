HTML = $(wildcard *.html)
HTMLOUT = $(patsubst %.html,obj/%.html,$(HTML))

HHC = /opt/htmlhelp/hhc.exe

all: help.chm

help.chm: obj/help.hhp $(HTMLOUT) obj/styles.css
	cd obj; $(HHC) help.hhp; true

obj/help.hhp: help.hhp
	@mkdir -p obj
	cp $< $@

obj/%.html: %.html
	cat template_head.html $< template_foot.html > $@

obj/styles.css: styles.css
	cp $< $@

clean:
	rm -rf obj

.PHONY: all clean
