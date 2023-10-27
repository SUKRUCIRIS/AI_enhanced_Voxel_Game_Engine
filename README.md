# ŞÜKRÜ ÇİRİŞ 18401785

build project files:

```
mkdir build
cd build
cmake .. -DCMAKE_VERBOSE_MAKEFILE=1
```

compile gnu:

```
make -j`nproc`
```

compile msvc:

```
msbuild opengl_voxel_gsu.sln /p:Configuration=Release /p:Platform=x64
```
