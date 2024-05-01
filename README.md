## Prerequisites

Windows: 

You have to install Visual Studio Build Tools, MSVC, Cmake, MSbuild.

Ubuntu:

You have to run this command:
```
sudo apt install -y gcc cmake build-essential libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev mesa-common-dev libasound2-dev
```

## Compiling with Python GUI (MSVC or GCC)

```
python ./run.py
```

You can change various settings of the demo game of the game engine on this menu and compile with a button.

You have to set the path to vcvarsall.bat in the GUI to be able to compile on Windows.

## Compiling without Python GUI on x64 Windows (MSVC)

Build project files:

```
mkdir build
cd build
cmake ..
```

Make sure that you use x64 native tools command prompt for vs to run msbuild command.

```
msbuild voxel_engine.sln /p:Configuration=Release /p:Platform=x64
```

## Compiling without Python GUI on x64 Ubuntu (GCC)

Compiling:

```
mkdir build
cd build
cmake ..
make
```

## Debugging on x64 Ubuntu (GCC)

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
