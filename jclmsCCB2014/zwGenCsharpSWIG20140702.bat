@ECHO OFF
set PATH=F:\DiskD\zwTools\swigwin-2.0.11;%PATH%
set SWIG_LIB=F:\DiskD\zwTools\swigwin-2.0.11\Lib
@ECHO ON
del /Q cs702\*.cs
swig -csharp -c++  -outdir cs702 -namespace "jclms" jclmsccb_csharp.i
copy /y cs702\*.cs ..\jclmsCCBcsLib\swigCode
PAUSE