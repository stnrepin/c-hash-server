FROM debian

ARG CMAKE_BUILD_CONFIGURATION=Release
ARG CMAKE_BUILD_TESTS=0

RUN apt-get update
RUN apt-get -y install git cmake make curl libssl-dev

ARG CMAKE_FLAGS="-DCMAKE_BUILD_TYPE=$CMAKE_BUILD_CONFIGURATION \
                 -DBUILD_TESTS=$CMAKE_BUILD_TESTS"

RUN git clone --recursive https://github.com/stnrepin/c-hash-server /usr/src/app
RUN echo "CMake flags: $CMAKE_FLAGS" && \
    cd /usr/src/app && \
    mkdir build && cd build && \
    cmake $CMAKE_FLAGS .. && \
    make && \
    if [ $CMAKE_BUILD_TESTS -eq 1 ]; then ctest -V; fi && \
    echo 'Build'

ENTRYPOINT /usr/src/app/bin/hash-server

