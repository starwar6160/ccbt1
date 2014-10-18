@ECHO OFF
REM set PATH=F:\DiskD\zwTools\swigwin-2.0.11;%PATH%
REM set SWIG_LIB=F:\DiskD\zwTools\swigwin-2.0.11\Lib
@ECHO ON
del /Q cs806\*.cs
swig -csharp -c++  -outdir cs806 -namespace "jclms" ccbelock_csharp.i
copy /y cs806\*.cs ..\ccbATMCSecretBoxLib
PAUSE