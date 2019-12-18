IDIR=../headers
CC=gcc
CFLAGS=-iquote$(IDIR)

ODIR=obj
LIBS=-lavs -lpaho-mqtt3c

_DEPS = *.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = mqtt_client.o avs_driver.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c $< -o $@ $(CFLAGS) -v

spectral: $(OBJ)
	$(CC) -o ../$@ $^ $(CFLAGS) $(LIBS) -v -Wall -Xlinker --allow-multiple-definition

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm -f ../spectral
