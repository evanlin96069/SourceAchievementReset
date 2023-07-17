if not exist bin\ md bin

clang -shared -O0 -m32 -o bin\tier0.dll src\stubs\tier0.c
clang -shared -O0 -m32 -Lbin -ltier0 -o bin\example_plugin.dll src\*.c
