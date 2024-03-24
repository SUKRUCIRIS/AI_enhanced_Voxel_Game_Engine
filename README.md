## Compiling with Python GUI on Windows
```
python ./run.py
```
You can change various settings of the demo game of the game engine on this menu and compile with a button.
You have to install Visual Studio Build Tools and set the path to vcvarsall.bat in the GUI to be able to compile.
You have to create a folder named ./aimodels and put an AI model named hm.safetensors in it to be able to generate heightmaps with AI. I used [this model](https://civitai.com/models/16826/game-landscape-heightmap-genrator) on my computer.

## Compiling without Python GUI on Windows

### Build project files:

```
mkdir build
cd build
cmake .. -DCMAKE_VERBOSE_MAKEFILE=1
```

### Compile with msvc on x64 windows:

Make sure that you use x64 native tools command prompt for vs.

```
msbuild voxel_engine.sln /p:Configuration=Release /p:Platform=x64
```

## Şükrü Çiriş

All rights reserved.

[My Website](https://sukruciris.github.io)
