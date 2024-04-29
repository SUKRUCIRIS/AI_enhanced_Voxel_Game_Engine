## Compiling with Python GUI on Windows
```
python ./run.py
```
You can change various settings of the demo game of the game engine on this menu and compile with a button.

You have to install Visual Studio Build Tools and set the path to vcvarsall.bat in the GUI to be able to compile.

## Compiling without Python GUI on Windows

### Build project files:

```
mkdir build
cd build
cmake ..
```

### Compile with MSVC on x64 Windows:

Make sure that you use x64 native tools command prompt for vs.

```
msbuild voxel_engine.sln /p:Configuration=Release /p:Platform=x64
```

Then run the ./voxel_engine.exe

## Compiling with GNU on x64 Ubuntu:

Prerequisites:

```
apt install gcc
apt install cmake
apt install build-essential
apt install libx11-dev
apt install libxrandr-dev
apt install libxinerama-dev
apt install libxcursor-dev
apt install libxi-dev
apt install mesa-common-dev
apt install libasound2-dev
```

Compiling:

```
mkdir build
cd build
cmake ..
make
```

Debugging:

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
cd ..
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-out.txt \
         ./voxel_engine 
gdb ./voxel_engine 
```

## Şükrü Çiriş

All rights reserved.

[My Website](https://sukruciris.github.io)
