# Banjo

The Banjo Programming Language is a redesign of C++ that adheres to that
language's philosophy, semantics, and (largely) syntax.

## Dependencies

Banjo has several external dependencies.

- [CMake](https://cmake.org) version 3.0 or greater. This provides the
   build system for Banjo.
- [Boost](http://www.boost.org) version 1.55 or later.
- [Lingo](https://github.com/asutton/lingo) is a library that provides a
  number of utilities used by the compiler.
- [LLVM](http://llvm.org/) is a highly portable and optimizable intermediate   
  representation for programming languages.

## Forking Banjo

Use the fork feature on github to fork the repo to your page.

Add the original repo to your upstream:

```shell
git remote add upstream https://github.com/asutton/banjo.git
```

Run

```shell
git remote -v
```

to make sure the upstream has been added.

## Syncing your Fork

To get the latest updates to Banjo:

```shell
git fetch upstream
```

To merge your local changes, checkout your local master and run:

```shell
git merge upstream/master
```

## Building Banjo

After cloning the repository (or a fork of the repository), go into the
directory and run:

```shell
git submodule update --init --recursive
```

This will pull the required versions of Lingo into your working directory.

Note that if you have a fork of Banjo, you may occasionally need to update the
'lingo' submodule. That can be done using the following command:

```shell
git submodule update --remote --merge
```

The build is configured by [CMake](https://cmake.org). Within your clone
of the Banjo repository, run the following commands to establish the
build system in the `build` directory.

```shell
mkdir build
cd build
cmake ..
```

It is generally useful to build in debug mode. You can run `ccmake` to
edit the build configuration.

```shell
ccmake ..
```

Set the value of `CMAKE_BUILD_TYPE` to `Debug`. Capitalization matters.

Now you can build the compiler:

```shell
make
```


## Testing Banjo

You can run the test suite by running:

```shell
make test
```

If you want to test the parser, you can use the `test_parse` program.
It takes the name of a single input file as an argument and prints
information about the parsed program (or errors).

```shell
./banjo/test_parse input.banjo
```


## Errata

When lowering LLVM to native assembly on new versions of Mac OS X you may
need to override the system triple to avoid linker warnings. Invoke `llc`
like this:

```shell
llc -mtriple=x86_64-apple-macosx <input>
```
