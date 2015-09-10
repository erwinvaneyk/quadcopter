CC=lcc-x32
CFLAGS=-I$(X32INC)
OBJS=protocol.o

.PHONY: clean stubbed stubbedrun uploadrun test

qrtest.ce: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -lib $(X32INC) -o qrtest.ce

stubbed: CFLAGS+=-DSENSOR_STUB
stubbed: qrtest.ce

stubbedrun: stubbed uploadrun

clean:
	rm -f term $(OBJS) $(OBJS:.o=.d)

uploadrun: qrtest.ce
	x32-upload qrtest.ce -c $(X32_PACKAGE_SERIAL) -e

term: term.c protocol.c
	gcc -Wall term.c protocol.c -o term -lrt

test: qrtest.ce
	x32-upload test.ce -c $(X32_PACKAGE_SERIAL)

test.ce: test.c
	$(CC) $(CFLAGS) test.c -lib $(X32INC) -o test.ce

%.o : %.c
	@$(COMPILE.c) -M -o $@ $< > $*.d
	$(COMPILE.c) -o $@ $<
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $*.P >> $*.d; \
		rm -f $*.P

-include $(OBJS:.o=.d)
