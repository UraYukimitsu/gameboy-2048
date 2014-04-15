@echo off
setlocal enabledelayedexpansion
for %%i in (*.c) do (
echo Compiling %%i...
..\bin\lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -c -o %%i.o %%i
set str=!str! %%i.o
)
..\bin\lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -Wl-yt1 -Wl-yo4 -Wl-ya0 -o 2048.gb %str%
del *.lst *.o *.map *.sym
pause
