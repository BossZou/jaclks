jaclks - java-based C++ lib kits
================================

`jaclks` is a modern, open source, java-based C++ library. `jaclks` supports classes with java-codestyle APIs.

[![codecov](https://codecov.io/gh/BossZou/jaclks/graph/badge.svg?token=SGY77PRJI1)](https://codecov.io/gh/BossZou/jaclks)

# To start using jaclks

This section describes how to add `jaclks` as a dependency to your C++ project.

## Supported Platforms

| Operating System               | Architectures |
|--------------------------------|---------------|
| Linux - Debian, Ubuntu, CentOS | x86, x64      |
| Windows 10+                    | x86, x64      |
| MacOS                          | x64, ARM64    |
| Linux - Others                 | x86, x64      |
| Linux                          | ARM64         |
| iOS                            | Not supported |
| Android                        | Not supported |
| AIX                            | Not supported |
| Asylo                          | Not supported |
| FreeBSD                        | Not supported |
| Fuchsia                        | Not supported |
| NaCL                           | Not supported |
| NetBSD                         | Not supported |
| OpenBSD                        | Not supported |
| Solaris                        | Not supported |

## Build from source

`jaclks` use cmake as compile toolkit. 

Follows shell commands below can build `jaclks`:
```shell
$ mkdir build
$ cd build
$ cmake .. 
$ make
```

## Develop jaclks

You can run googletest as follow:
```shell
$ cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_GTEST=ON
$ make
$ ctest --verbose --output-on-failure --build-config Debug
```

And you can enable coverage like:
```shell
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_GTEST=ON  -DENABLE_COVERAGE=ON
```

Then use `gcovr` to collect coverage information
```shell
$ gcovr -r ${WORKSPACE} --xml -o ${WORKSPACE}/coverage.xml -e '/usr/.*' -e '.*/tests/.*' -e '.*/thirdparty/.*'
```
