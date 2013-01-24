echo "Compiling binaries."
g++ -c -fPIC ImageProcessing.cpp -o ImageProcessing.o
g++ -shared -Wl,-soname,libimgproc.so -o libimgproc.so  ImageProcessing.o

g++ -c -fPIC ArduinoController.cpp -o ArduinoController.o
g++ -shared -Wl,-soname,libarduino.so -o libarduino.so  ArduinoController.o
