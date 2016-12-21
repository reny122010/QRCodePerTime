RM = rm -fv
DEBUG = -g 
LFLAGS =  -pthread -lrt
CVFLAGS = -ggdb `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` 

all: syncAPP

syncAPP:
	gcc ./syncAPP.c $(CVFLAGS) $(LFLAGS) $(LIBS) $(DEBUG) -lqrencode -L/usr/local/liblibqrencode.so.3.4.4 -o syncAPP
clean:
	@echo 'Cleaning object files'
	$(RM) syncAPP
