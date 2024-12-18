clang++ -std=c++17 libs/EMSearcher/EMSearcher.cpp libs/porter/porter.cpp libs/util/util.cpp main.cpp  -o testapp.exe -Ilibs/EMSearcher -Ilibs/porter -Ilibs/util
testapp.exe