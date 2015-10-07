OBJS=fp_test.o fp_arithmetic.o fp_conversion.o fp_conversion_pc.o

.PHONY: clean stubbed stubbedrun uploadrun test

clean:
	rm -f test $(OBJS) $(OBJS:.o=.d)

test: fp_test.c fp_arithmetic.c fp_conversion.c fp_conversion_pc.c
	gcc -Wall fp_test.c fp_arithmetic.c fp_conversion.c fp_conversion_pc.c -o test

%.o : %.c
	@$(COMPILE.c) -M -o $@ $< > $*.d
	$(COMPILE.c) -o $@ $<
	@cp $*.d $*.P; \
		sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
			-e '/^$$/ d' -e 's/$$/ :/' < $*.P >> $*.d; \
		rm -f $*.P

-include $(OBJS:.o=.d)
