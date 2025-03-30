# 获取当前Makefile所在目录
THIS_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

THIDR_DIR := $(THIS_DIR)/thirdparty

# 安装目录设置
INSTALL_DIR := $(THIS_DIR)/dist/third-party

# GTest
GTEST_BUILD_DIR := $(THIDR_DIR)/gtest-build

GTEST_DIR := $(THIS_DIR)/thirdparty/googletest

gtest:
	rm -rf $(GTEST_BUILD_DIR) && mkdir -p $(GTEST_BUILD_DIR)
	cd $(GTEST_BUILD_DIR) && \
	  cmake ../googletest -DCMAKE_INSTALL_PREFIX=$(INSTALL_DIR) \
	  -DCMAKE_CXX_COMPILER=g++ \
	  -DBUILD_SHARED_LIBS=ON \
	  -DCMAKE_BUILD_TYPE=Release

format:
	@echo "Formatting source files with clang-format..."
	find src/ include/ -name '*.cc' -o -name '*.h' -print0 | xargs -0 clang-format -style=file -i
	@echo "Formatting source files with clang-format done."