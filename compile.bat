@echo off

if not exist bin\ md bin

set dbg=0
if "%dbg%"=="1" (
	set cflags=-O0 -g3
) else (
	set cflags=-O2
)

clang -shared %cflags% -m32 -o bin\tier0.dll src\stubs\tier0.c
clang -shared %cflags% -m32 -o bin\vstdlib.dll src\stubs\vstdlib.c

clang -shared %cflags% -m32 -Lbin -ltier0 -lvstdlib -o bin\sar.dll src\*.c
