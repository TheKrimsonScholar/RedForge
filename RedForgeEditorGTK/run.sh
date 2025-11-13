#!/bin/bash
dos2unix /mnt/c/users/krims/documents/github/redforge/redforge/*.cpp /mnt/c/users/krims/documents/github/redforge/redforge/*.h /mnt/c/users/krims/documents/github/redforge/redforge/*.hpp
dos2unix /mnt/c/users/krims/documents/github/redforge/redforgeeditor/*.cpp /mnt/c/users/krims/documents/github/redforge/redforgeeditor/*.h /mnt/c/users/krims/documents/github/redforge/redforgeeditor/*.hpp
dos2unix /mnt/c/users/krims/documents/github/redforge/redforge/include/**/*.hpp /mnt/c/users/krims/documents/github/redforge/redforge/include/**/*.h
g++ -std=c++20 -shared -fPIC -pthread -o libRedForge.so \
    /mnt/c/users/krims/documents/github/redforge/redforge/*.cpp \
    /mnt/c/users/krims/documents/github/redforge/redforge/include/**/*.cpp \
    -I/mnt/c/users/krims/documents/github/redforge/redforge \
    -I/mnt/c/users/krims/documents/github/redforge/redforge/include
g++ -std=c++20 -pthread -o RedForgeEditor *.cpp -L. -lRedForge \
    -I/mnt/c/users/krims/documents/github/redforge/redforge \
    $(pkg-config --cflags --libs gtkmm-4.0 glfw3 gl vulkan epoxy) -lGL -lvulkan
