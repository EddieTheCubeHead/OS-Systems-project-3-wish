# Using this tutorial (made a bit more verbose for readibility):
# https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/

# Use normal gcc for compiling
CCOMPILER = gcc

# Use include flag, enable debugging and enable all warnings as errors
CFLAGS = -I. -g -Wall -Werror

# Using a singular header file to enable easier make / imports
DEPENDS = imports.h

# The object files for final compilation
OBJECTS = wish_ui.o wish_core.o wish_utils.o wish_commands.o

%.o: %.c $(DEPENDS)
	$(CCOMPILER) -c -o $@ $< $(CFLAGS)

wish: $(OBJECTS)
	$(CCOMPILER) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJECTS)
	rm -f wish