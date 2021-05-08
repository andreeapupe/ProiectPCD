#!/bin/bash

if [[ "$1" == *all* ]]
then
    if command -v gcc &> /dev/null
    then
        echo -e "\e[1;42m[-] GCC detected\e[0m"
    else
        echo -e "\e[1;46mGCC not detected. Installing...\e[0m"
        sudo apt-get install gcc
    fi

    if command -v cmake &> /dev/null
    then
        echo -e "\e[1;42m[-] CMake detected\e[0m"
    else
        echo -e "\e[1;46mCMake not detected. Installing...\e[0m"
        sudo apt-get install cmake
    fi

    if command -v make &> /dev/null
    then
        echo -e "\e[1;42m[-] Make detected\e[0m"
    else
        echo -e "\e[1;46mMake not detected. Installing...\e[0m"
        sudo apt-get install make
    fi

    rm -r build
    mkdir build
    cd build
    cmake ..
else
    cd build
    make
fi
