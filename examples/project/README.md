1. Download config-cxx

    ```
    mkdir externals
    cd externals
    git submodule add https://github.com/cieslarmichal/config-cxx.git
    git submodule update --init --recursive
    ```
2. Set environment variables

        ```
        export CONFIG_CXX_DIR=$PWD/config
        export CXX_ENV=development
        ```
