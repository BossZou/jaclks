# 获取当前Makefile所在目录
THIS_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

THIDR_DIR := $(THIS_DIR)/thirdparty

# 安装目录设置
INSTALL_ROOT_DIR := $(THIS_DIR)/dist/third-party

# GTest
GTEST_BUILD_DIR := $(THIDR_DIR)/gtest-build
GTEST_INSTALL_DIR := $(INSTALL_ROOT_DIR)/googletest
GTEST_DIR := $(THIS_DIR)/thirdparty/googletest

gtest:
	@mkdir -p $(GTEST_BUILD_DIR)
	@cd $(GTEST_BUILD_DIR) && \
	  cmake ../googletest -DCMAKE_INSTALL_PREFIX=$(GTEST_INSTALL_DIR) \
	  -DCMAKE_CXX_COMPILER=g++ \
	  -DBUILD_SHARED_LIBS=ON \
	  -DCMAKE_BUILD_TYPE=Release &&	\
	  $(MAKE) -j$(nproc) && \
	  $(MAKE) install

clean_gtest:
	@rm -rf $(GTEST_INSTALL_DIR)
	@rm -rfv $(GTEST_BUILD_DIR)

rebuild_gtest:	clean_gtest gtest
	@echo "Rebuild googletest done"

format:
	@echo "Formatting source files with clang-format..."
	find src/ include/ tests -name '*.cc' -o -name '*.h' | xargs clang-format -verbose -style=file -i
	@echo "Formatting source files with clang-format done."
