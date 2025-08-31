#!/usr/bin/env bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

THIRD_ROOT_DIR="$PROJECT_ROOT_DIR/thirdparty"
THIRD_DIST_DIR="$PROJECT_ROOT_DIR/dist"

pushd ${PROJECT_ROOT_DIR} >/dev/null

## Build googletest
GOOGLETEST_ROOT_DIR="$THIRD_ROOT_DIR/googletest"

cmake -B ${GOOGLETEST_ROOT_DIR}/build \
	-S ${GOOGLETEST_ROOT_DIR} \
	-DCMAKE_BUILD_TYPE=Release \
	-DBUILD_SHARED_LIBS=OFF \
	-Dgtest_force_shared_crt=ON \
	-DCMAKE_INSTALL_PREFIX=${THIRD_DIST_DIR}

cmake --build ${GOOGLETEST_ROOT_DIR}/build --config Release --target install

popd >/dev/null
