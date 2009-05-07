CILDIR := /home/nox/Desktop/cil-1.3.7/
COMP := ocamlopt -ccopt -L$(CILDIR)/obj/x86_LINUX/ -I $(CILDIR)/obj/x86_LINUX unix.cmxa str.cmxa cil.cmxa
ccomp: main.ml printer.ml
	$(COMP) -o ccomp printer.ml main.ml
