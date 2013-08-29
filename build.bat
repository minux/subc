set DEVCPPPATH=c:\\dev-cpp\\
set CC=gcc

echo c:\dev-cpp\bin\ must be in the path environment variable!

copy src\Makefile.windows src\Makefile
copy src\targets\init-windows.c src\lib\init.c
copy src\targets\system-windows.c src\lib\system.c
copy src\targets\cg386-syn.c src\cg.c
copy src\targets\cg386.h src\cg.h
copy src\targets\syngen.c src\gen.c
copy src\targets\windows.h src\sys.h
copy src\targets\crt0-windows-386.s src\lib\crt0.s
copy src\targets\limits-32.h src\include\limits.h

echo #undef SYSLIBC >> src\sys.h
echo #define SYSLIBC	"%DEVCPPPATH%lib/libuser32.a %DEVCPPPATH%lib/libkernel32.a %DEVCPPPATH%/lib/libgdi32.a" >> src\sys.h
echo /**/ >> src\sys.h

cd src
make clean
make scc

copy scc %DEVCPPPATH%\bin\scc.exe
mkdir c:\mingw\scc
mkdir c:\mingw\scc\lib
mkdir c:\mingw\scc\include
copy include\*.* c:\mingw\scc\include\
copy lib\libscc.a c:\mingw\scc\lib\
copy lib\crt0.o c:\mingw\scc\lib\

cd ..
echo done!
