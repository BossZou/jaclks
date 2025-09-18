#!/usr/bin/env bash

# exit shell if cmd fail
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

THIRD_ROOT_DIR="$PROJECT_ROOT_DIR/thirdparty"
THIRD_DIST_DIR="$PROJECT_ROOT_DIR/dist"
THIRD_DIST_PKG_CONFIG_DIR="$THIRD_DIST_DIR/lib/pkgconfig"

export PKG_CONFIG_PATH="${THIRD_DIST_PKG_CONFIG_DIR}:${PKG_CONFIG_PATH}"

if pkg-config --exists gtest; then
	echo "googletest installed"
	echo "version: $(pkg-config --modversion gtest)"
	echo "cflags: $(pkg-config --cflags gtest)"
	echo "libs: $(pkg-config --libs gtest)"
else
	echo "googletest not installed, build and install now ..."
	GOOGLETEST_ROOT_DIR="$THIRD_ROOT_DIR/googletest"
	pushd "${GOOGLETEST_ROOT_DIR}" >/dev/null || exit

	## Build googletest
	cmake -B "${GOOGLETEST_ROOT_DIR}/build" \
		-S "${GOOGLETEST_ROOT_DIR}" \
		-DCMAKE_BUILD_TYPE=Release \
		-DBUILD_SHARED_LIBS=OFF \
		-Dgtest_force_shared_crt=ON \
		-DCMAKE_INSTALL_PREFIX="${THIRD_DIST_DIR}"

	cmake --build "${GOOGLETEST_ROOT_DIR}/build" --config Release --target install -j $(nproc)

	popd >/dev/null || exit
fi

if pkg-config --exists jemalloc; then
	echo "jemalloc installed"
	echo "version: $(pkg-config --modversion jemalloc)"
	echo "cflags: $(pkg-config --cflags jemalloc)"
	echo "libs: $(pkg-config --libs jemalloc)"
else
	echo "jemalloc not installed, build and install now ..."
	JEMALLOC_ROOT_DIR="$THIRD_ROOT_DIR/jemalloc"
	pushd "${JEMALLOC_ROOT_DIR}" >/dev/null || exit

	## Build jemalloc
	sh autogen.sh --prefix=${THIRD_DIST_DIR}
	make -j $(nproc)
	make install

	popd >/dev/null || exit
fi
