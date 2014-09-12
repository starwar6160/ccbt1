@ECHO OFF
REM set PATH=F:\DiskD\zwTools\swigwin-2.0.11;%PATH%
REM set SWIG_LIB=F:\DiskD\zwTools\swigwin-2.0.11\Lib
@ECHO ON
del /Q Python912\*.py
swig -python -c++  -outdir Python912 jclmsccb_python.i
REM copy /y Python912\*.py ..\jclmsCCBcsLib\swigCode
PAUSE