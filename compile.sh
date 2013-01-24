echo "Compiling binaries."
g++ -c -fPIC src/ExploreState.cpp -o obj/ExploreState.o
g++ -c -fPIC src/BallCollectState.cpp -o obj/BallCollectState.o

#g++ -c -fPIC src/ExploreState.cpp -o ExploreState
#g++ -c -fPIC src/ImageProcessing.cpp -o ImageProcessing.o
#g++ -c -fPIC src/ArduinoController.cpp -o ArduinoController.o

#echo "Generating shared libraries."
#g++ -shared -Wl,-soname,libimgproc.so -o libimgproc.so  ImageProcessing.o
#g++ -shared -Wl,-soname,libarduino.so -o libarduino.so  ArduinoController.o
