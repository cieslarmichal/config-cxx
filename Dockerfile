FROM ubuntu:23.10

RUN apt update -y
RUN apt install -y cmake ninja-build g++-13 libstdc++-13-dev

COPY include/ config-cxx/include/
COPY src/ config-cxx/src/
COPY externals/ config-cxx/externals/
COPY cmake/ config-cxx/cmake/
COPY CMakeLists.txt config-cxx/CMakeLists.txt
COPY scripts/ config-cxx/scripts/

RUN chmod 777 ./config-cxx/scripts/run_tests_linux_gxx.sh

ENTRYPOINT cd config-cxx && ./scripts/run_tests_linux_gxx.sh
