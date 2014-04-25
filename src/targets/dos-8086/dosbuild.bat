@echo off

if "%1"=="YES" goto doit
if "%1"=="Yes" goto doit
if "%1"=="yes" goto doit

echo ****************************************
echo This step will copy the compiler sources
echo to this directory! Run DOSBUILD YES, if
echo you really want to do this!
echo ****************************************
exit

:doit

echo copying source files
copy src\*.* .>NUL

scc -cv cexpr.c
scc -cv cg.c
scc -cv decl.c
scc -cv dosmain.c
scc -cv error.c
scc -cv expr.c
scc -cv gen.c
scc -cv misc.c
scc -cv prep.c
scc -cv scan.c
scc -cv stmt.c
scc -cv sym.c
scc -cv ulibc.c

sld -vvv -o sccmain1.exe -f link.lst
