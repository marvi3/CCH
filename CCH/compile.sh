#!/bin/sh
g++ -O3 -DNDEBUG -std=c++0x Customizer.cpp -c
g++ -O3 -DNDEBUG -std=c++0x QueryManager.cpp -c
g++ -O3 -DNDEBUG -std=c++0x CCHGraph.cpp -c
g++ -O3 -DNDEBUG -std=c++0x Customizer.cpp QueryManager.cpp contractor.cpp  CCHGraph.cpp CCH.cpp -o main

