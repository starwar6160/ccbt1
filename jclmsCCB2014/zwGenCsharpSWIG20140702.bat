@ECHO OFF
set PATH=F:\DiskD\zwTools\swigwin-2.0.11;%PATH%
set SWIG_LIB=F:\DiskD\zwTools\swigwin-2.0.11\Lib
@ECHO ON
del /Q cs702\*.cs
swig -csharp -c++  -outdir cs702 jclmsccb_csharp.i
REM copy /y cs702\*.cs ..\ecdhdotnetLib\ecdhCSLib\cs320
PAUSE