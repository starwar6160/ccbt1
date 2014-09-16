@ECHO OFF
REM set PATH=F:\DiskD\zwTools\swigwin-2.0.11;%PATH%
REM set SWIG_LIB=F:\DiskD\zwTools\swigwin-2.0.11\Lib
REM COPY /y $(TargetPath) $(TargetDir)\_$(ProjectName).pyd
@ECHO ON
del /Q HidProtocol.py
swig -python -c++ CCBelock.i
REM copy /y cs806\*.cs ..\jclmsCCBcsLib\swigCode
PAUSE