# ŞÜKRÜ ÇİRİŞ 18401785

# HOW TO BUILD?

## Build project files:

```
mkdir build
cd build
cmake .. -DCMAKE_VERBOSE_MAKEFILE=1
```

## Compile with gnu on linux:

```
make -j`nproc`
```

You need to install pthread if you don't have. I run this command on ubuntu:

```
sudo apt-get install libpthread-stubs0-dev
```

You need to install your platform's windowing system dev package to compile glfw if you don't have.
For example I run this command on ubuntu x11:

```
sudo apt-get install xorg-dev
```

## Compile with msvc on windows:

Make sure that you use native tools command prompt for vs.

```
msbuild opengl_voxel_gsu.sln /p:Configuration=Release /p:Platform=x64
```
