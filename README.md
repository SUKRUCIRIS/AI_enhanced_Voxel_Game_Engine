# ŞÜKRÜ ÇİRİŞ 18401785

# HOW TO BUILD?

## Build project files:

```
mkdir build
cd build
cmake .. -DCMAKE_VERBOSE_MAKEFILE=1
```

## Compile with msvc on x64 windows:

Make sure that you use x64 native tools command prompt for vs.

```
msbuild opengl_voxel_gsu.sln /p:Configuration=Release /p:Platform=x64
```
