#!/bin/sh

# Set up environment
apt-get -y install git g++ autoconf pkg-config libtool libjson-spirit-dev libreadline-dev \
    libdw-dev libssl-dev valgrind \
    python-pip python-xmlrunner libboost-all-dev \
    libcrypto++-dev libprotobuf-dev protobuf-compiler

# Pip
pip install unittest-xml-reporting
