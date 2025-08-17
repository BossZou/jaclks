jaclks - java-based C++ lib kits
================================

`jaclks` is a modern, open source, java-based C++ library. `jaclks` supports classes with java-codestyle APIs.

[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![ACI Check](https://github.com/BossZou/jaclks/actions/workflows/aci.yml/badge.svg)](https://github.com/BossZou/jaclks/actions/workflows/aci.yml)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/5d38d4632b164f1593dfb364cd0252fe)](https://app.codacy.com/gh/BossZou/jaclks/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade)
[![codecov](https://codecov.io/gh/BossZou/jaclks/graph/badge.svg?token=SGY77PRJI1)](https://codecov.io/gh/BossZou/jaclks)

# To start using jaclks

This section describes how to add `jaclks` as a dependency to your C++ project.

## Supported Platforms

| Operating System | Architectures | OS Version |
|------------------|---------------|------------|
| Ubuntu-22.04     | x64, ARM64    | &#10004;   |
| Ubuntu-24.04     | x64, ARM64    | &#10004;   |
| Windows 2025     | x64           | &#10004;   |
| MacOS            | Not supported | &#10008;   |
| Linux - Others   | Not supported | &#10008;   |
| Linux            | Not supported | &#10008;   |
| iOS              | Not supported | &#10008;   |
| Android          | Not supported | &#10008;   |
| AIX              | Not supported | &#10008;   |
| Asylo            | Not supported | &#10008;   |
| FreeBSD          | Not supported | &#10008;   |
| Fuchsia          | Not supported | &#10008;   |
| NaCL             | Not supported | &#10008;   |
| NetBSD           | Not supported | &#10008;   |
| OpenBSD          | Not supported | &#10008;   |
| Solaris          | Not supported | &#10008;   |

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
