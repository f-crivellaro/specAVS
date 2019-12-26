IDIR=../headers
CC=gcc
CFLAGS=-iquote$(IDIR)

ODIR=obj
LIBS=-lavs -lpaho-mqtt3c

_DEPS = *.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = log.o utils.o mqtt_client.o avs_driver.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c $< -o $@ $(CFLAGS) -v -std=c99

spectral: $(OBJ)
	$(CC) -o ../$@ $^ $(CFLAGS) $(LIBS) -v -Wall -Xlinker --allow-multiple-definition

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
	rm -f ../spectral
	rm -f ../log/*
