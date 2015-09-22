CC=lcc-x32
CFLAGS=-I$(X32INC)
OBJS=protocol.o rs232.o consoleio.o joystickio.o keyboardio.o input.o

.PHONY: clean stubbed stubbedrun uploadrun test

qrtest.ce: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -lib $(X32INC) -o qrtest.ce

stubbed: CFLAGS+=-DSENSOR_STUB
stubbed: qrtest.ce

stubbedrun: stubbed uploadrun

clean:
	rm -f term $(OBJS) $(OBJS:.o=.d)

term: term.c protocol.c rs232.c consoleio.c joystickio.c keyboardio.c input.c protocol.h
	gcc -Wall term.c protocol.c rs232.c consoleio.c joystickio.c keyboardio.c input.c -o term -lrt

%.o : %.c
	@$(COMPILE.c) -M -o $@ $< > $*.d
	$(COMPILE.c) -o $@ $<
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $*.P >> $*.d; \
		rm -f $*.P

-include $(OBJS:.o=.d)
