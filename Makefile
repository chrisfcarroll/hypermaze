BUILDDIR=build
DISTDIR=dist

all: hypermaze
run: run-hypermaze

hypermaze.zip: dist
	zip hypermaze.zip dist/* 
dist: dist/hypermaze dist/irrlicht dist/hypermaze.keymap.conf

dist/%:%
	cp -r $^ $@

CPP=g++
CPPOPTS=-ggdb
#CPPOPTS+= -rdynamic
CPPLIBS=

IRRLIBS=-lIrrlicht -isystem/usr/include/irrlicht/

test: CPPOPTS+= -DIOSTREAM
test: test.o maze.o hypio.o
test.o: test.cc maze.hh dirns.hh vector.hh string.hh mazegen.hh hypio.hh
run-test: test
	./test

scriptedit: CPPOPTS+= -DIOSTREAM
scriptedit: scriptedit.o script.o hypio.o maze.o

run-scriptedit: scriptedit
	./scriptedit

scriptedit.o: scriptedit.cc scriptimpl.hh script.hh hypio.hh maze.hh

hypermaze: CPPOPTS+= -DIRRLICHT -DOPENAL -DIOSTREAM
hypermaze: CPPLIBS+= $(IRRLIBS) -lopenal -lalut -lcurl
hypermaze: hypermaze.o iMyCamera.o controller.o irrdisp.o hypio.o maze.o keymap.o GUIFormattedText.o sound.o script.o gui.o fonts.o irrcurl.o

run-hypermaze: hypermaze
	./hypermaze

hypermaze.o: hypermaze.cc irrdisp.hh maze.hh dirns.hh vector.hh string.hh \
 iMyCamera.hh keymap.hh controller.hh script.hh scriptimpl.hh SmartPointer.hh hypio.hh fonts.hh

controller.o: controller.cc controller.hh string.hh maze.hh dirns.hh \
 vector.hh keymap.hh irrdisp.hh gui.hh mazegen.hh helpgui.hh keymapgui.hh GUIFormattedText.hh SmartPointer.hh hypio.hh hypioimp.hh

irrdisp.o: irrdisp.cc irrdisp.hh maze.hh dirns.hh vector.hh string.hh \
 controller.hh keymap.hh SmartPointer.hh hypio.hh

maze.o: maze.cc maze.hh dirns.hh vector.hh SmartPointer.hh hypio.hh

gui.o: gui.cc gui.hh irrdisp.hh maze.hh hypioimp.hh mazegen.hh

keymap.o: keymap.cc keymap.hh dirns.hh vector.hh 

iMyCamera.o: iMyCamera.cpp iMyCamera.hh

GUIFormatedText.o: GUIFormattedText.cc GUIFormatedText.hh

hypio.o: hypio.cc hypio.hh hypioimp.hh

irrcurl.o: irrcurl.cc irrcurl.hh

sound.o: sound.cc sound.hh

script.o: script.cc script.hh scriptimpl.hh SmartPointer.hh hypio.hh

fonts.o: fonts.cc fonts.hh
clean:
	rm -f *.o
	rm -f test
	rm -f hypermaze
	rm -rf dist/*

%: %.o
	$(CPP) $(CPPOPTS) -o $@ $^ $(CPPLIBS) 
%.o: %.cc
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
%.o: %.cpp
	$(CPP) $(CPPOPTS) $(CPPLIBS) -c $<
