cmake_build() {
    local BUILD_TYPE="${1}"
    local FAST_BUILD="${2:-OFF}"

    if [[ "${FAST_BUILD}" = "ON" ]]; then
        local BUILD_DIR="build-fast"
    else
        local BUILD_DIR="build-${BUILD_TYPE,,}"
    fi

    echo "Building ${BUILD_TYPE} in ${BUILD_DIR} (FAST_BUILD=${FAST_BUILD})"

    if [[ -n "${BUILD_DIR}" && -d "${BUILD_DIR}" ]]; then
        rm -rf "./${BUILD_DIR}"
    fi

    cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DFAST_BUILD="${FAST_BUILD}"
    cmake --build "${BUILD_DIR}"
}

alias cmakedeb='cmake_build Debug'
alias cmakerel='cmake_build Release'
alias cmakerelwithdebinfo='cmake_build RelWithDebInfo'
alias cmakefast='cmake_build Release ON'
