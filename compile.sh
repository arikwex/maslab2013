echo "Compiling binaries."
g++ -c -fPIC src/ExploreState.cpp -o obj/ExploreState.o
g++ -c -fPIC src/BallCollectState.cpp -o obj/BallCollectState.o
g++ -c -fPIC src/DeployState.cpp -o obj/DeployState.o
g++ -c -fPIC src/RepositionState.cpp -o obj/RepositionState.o

g++ -c -fPIC src/ImageProcessing.cpp -o obj/ImageProcessing.o
g++ -c -fPIC src/ArduinoController.cpp -o obj/ArduinoController.o

echo "Generating shared libraries."
g++ -shared -Wl,-soname,libimgproc.so -o shared/libimgproc.so  obj/*.o 
g++ -shared -Wl,-soname,libarduino.so -o shared/libarduino.so  obj/*.o

