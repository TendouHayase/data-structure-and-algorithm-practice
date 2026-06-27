cmake_minimum_required(VERSION 3.20)
project(trie)

set(CMAKE_CXX_STANDARD 20)

add_executable(trie trie/main.cpp trie/trie.cpp)

project(segment_tree)

add_executable(segment_tree segment_tree/main.cpp segment_tree/segment_tree.cpp)
