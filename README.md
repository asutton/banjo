# Banjo

The Banjo Programming Language.

## Building Banjo

After cloning the repository, go into the directory and run:

```shell
git submodule update --init --recursive
```

This will pull the required versions of Lingo into your working directory.

Note that if you have a fork of Banjo, you will need to update the 'lingo'
submodule.

```shell
git submodule update --remote --merge
```

The build is configured by [CMake](https://cmake.org).

```shell
mkdir build
cd build
cmake ..
make
```
