@REM Copyright (c) Alp Can Nalbant. Licensed under the MIT License.

@echo off
title The Windows API Common Helper Library Builder

setlocal EnableDelayedExpansion

set Projects=WinAPICommon KeySender Registerer
set SourceFilesExtension=.cpp
set HeaderFilesExtension=.hpp

set BaseDirectory=%~dp0

set i=0
for %%s in (%Projects%) do (
    if not !i!==0 set Comma=,
    call set ProjectsString=%%ProjectsString%%!Comma![!i!]: %%s
    set /A i+=1
)

set WaitForPendingOperationMilliSeconds=500

set SendedArgumentsCount=0
for %%x in (%*) do set /A SendedArgumentsCount+=1

if %SendedArgumentsCount%==0 call :WaitForPendingOperationBeginWithThreeDot

set IsEverPrompted=0

if %IsEverPrompted%==1 call :WaitForPendingOperationBeginWithThreeDot

@REM Get the current project from first paramater
set "Param1=%~1"
goto :Param1Check
:Param1Prompt
echo . ^>^> Enter the the index number of the project (%ProjectsString%):
set /P Param1=
set IsEverPrompted=1
goto :Param1Check
:Param1PromptWrong
echo . ^>^> You are entered invalid index number please try again (%ProjectsString%):
set /P Param1=
set IsEverPrompted=1
:Param1Check
if "%Param1%"=="" goto :Param1Prompt
set i=%Param1%
if not %i%==0 if not %i%==1 if not %i%==2 goto :Param1PromptWrong

if %IsEverPrompted%==1 call :WaitForPendingOperationBeginWithThreeDot

@REM Get the build mode from second paramater
set "Param2=%~2"
goto :Param2Check
:Param2Prompt
set /P "Param2=. >> Enter the build mode of the project ([D]: Debug,[R]: Release): "
set IsEverPrompted=1
goto :Param2Check
:Param2PromptWrong
set /P "Param2=. >> You are entered invalid build mode please try again ([D]: Debug,[R]: Release): "
set IsEverPrompted=1
:Param2Check
if "%Param2%"=="" goto :Param2Prompt
if /I not "%Param2%"=="D" if /I not "%Param2%"=="R" goto :Param2PromptWrong

if %IsEverPrompted%==1 call :WaitForPendingOperationBeginWithThreeDot

@REM Get the keep open setting from third paramater
set "Param3=%~3"
goto :Param3Check
:Param3Prompt
set /P "Param3=. >> Do you want to keep open the terminal after the end of the operation ([Y]: Yes,[N]: No): "
goto :Param3Check
:Param3PromptWrong
set /P "Param3=. >> You are entered invalid terminal keep open condition please try again ([Y]: Yes,[N]: No): "
:Param3Check
if "%Param3%"=="" goto :Param3Prompt
if /I not "%Param3%"=="Y" if /I not "%Param3%"=="N" goto :Param3PromptWrong

set "Param4=%~4"
set "BuilderConfiguration=%Param4%"

if "%BuilderConfiguration%"=="" (set IsCalledFromAnotherBuilder=False) else (set IsCalledFromAnotherBuilder=True)

if %IsCalledFromAnotherBuilder%==False (
    call :WaitForPendingOperationBeginWithOneDot
    echo .... ______________________________________________________________
    echo .... ##############################################################
    echo .... Welcome to Builder of The Windows API Common Helper Project^^!
    echo .... ##############################################################
    echo .... ______________________________________________________________
    call :WaitForPendingOperationBeginWithThreeDot
)

@REM Store the values of the sended parameters from the caller.
set ProjectIndex=%Param1%
if /I "%Param2%"=="D" set BuildMode=Debug
if /I "%Param2%"=="R" set BuildMode=Release

@REM To prevent auto-closing of the console. Useful when manually running the batch file.
if /I "%Param3%"=="Y" set IsKeepOpenEnabled=True
if /I "%Param3%"=="N" set IsKeepOpenEnabled=False

if not "%BuilderConfiguration%"=="" (
    call %BuilderConfiguration% %BuildMode% WinAPICommon True
)

if "%BinaryOutputsDirectory%"=="" (
    set BinaryOutputsDirectory=%BaseDirectory%bin\%BuildMode%
)
if "%ObjectOutputsDirectory%"=="" (
    set ObjectOutputsDirectory=%BaseDirectory%obj\%BuildMode%
)

set IsStaticLibrary=False
set IsDynamicLibrary=False

@REM Set the source directory, project name, library status and the source file names without their extensions based of the current project via switch case mechanism.
:SwitchCase
    goto :Case_%ProjectIndex% 2>nul || (
        echo Invalid index number of the project: %ProjectIndex%
    )
    goto :SwitchCaseEnd
    :Case_0
        set SourceDirectory=%BaseDirectory%src
        set SourceFiles=Log KeySender System SystemCommon StringCommon String FileSystem Registry RegistryKey
        set ProjectName=WinAPICommon
        set PrecompiledHeader=%SourceDirectory%/Precompiled
        set IsStaticLibrary=True
    goto :SwitchCaseEnd
    :Case_1
        set SourceDirectory=%BaseDirectory%Examples\KeySenderSample
        set SourceFiles=WinMain
        set ProjectName=KeySender
        set IncludeDirectories=%BaseDirectory%src
        set StaticLibraries=WinAPICommon
        set StaticLibraryDirectories=%BinaryOutputsDirectory%
        set PrecompiledHeader=%BaseDirectory%/src/Precompiled
    goto :SwitchCaseEnd
    :Case_2
        set SourceDirectory=%BaseDirectory%Examples\RegistererSample
        set SourceFiles=WinMain
        set ProjectName=Registerer
        set IncludeDirectories=%BaseDirectory%src
        set StaticLibraries=WinAPICommon
        set StaticLibraryDirectories=%BinaryOutputsDirectory%
        set PrecompiledHeader=%BaseDirectory%/src/Precompiled
    goto :SwitchCaseEnd
:SwitchCaseEnd

if /I "%IsStaticLibrary%"=="True" if /I "%IsDynamicLibrary%"=="True" (
    echo . [X] Both variables ^(IsStaticLibrary ^& IsDynamicLibrary^) cannot have the same "True" value.
    goto :FatalError
)

set IsLibrary=True
if /I not "%IsStaticLibrary%"=="True" if /I not "%IsDynamicLibrary%"=="True" set IsLibrary=False

@REM Set the Environment variables of the current user temporarily via setlocal command and in this way do not make consistent changes.
setlocal

set CompilerDirectory=C:\msys64\mingw64\bin
set Compilerx64Directory=C:\msys64\mingw64\x86_64-w64-mingw32\bin
set CompilerUserDirectory=C:\msys64\usr\bin

@REM Add compiler binary path to environment variable for the current user. This command override the PATH you need to use %PATH% variable in the string.
@REM As an alternative use the /M option immediately after the setx command for the setting the variable at SYSTEM scope.
@REM setx command always makes consistent changes even before setlocal command used.
@REM setx PATH "%PATH%;%CompilerDirectory%;%Compilerx64Directory%;%CompilerUserDirectory%"
PATH="%PATH%;%CompilerDirectory%;%Compilerx64Directory%;%CompilerUserDirectory%"

@REM Idk why but cannot recognize this compiler executable files when i don't specify their paths.
@REM Even i already set the environment PATH variable for their location.
set Compiler=%CompilerDirectory%\x86_64-w64-mingw32-c++.exe
set Archiver=%CompilerDirectory%\x86_64-w64-mingw32-gcc-ar.exe
set MakeDir=%CompilerUserDirectory%\mkdir.exe

for %%s in (%SourceFiles%) do (
    call set ObjectFiles=%%ObjectFiles%% %ObjectOutputsDirectory%/%%s.o
)

for %%s in (%IncludeDirectories%) do (
    call set PrefixedIncludeDirectories=%%PrefixedIncludeDirectories%% -I%%s
)
for %%s in (%StaticLibraries%) do (
    call set PrefixedStaticLibraries=%%PrefixedStaticLibraries%% -l%%s
)
for %%s in (%StaticLibraryDirectories%) do (
    call set PrefixedStaticLibraryDirectories=%%PrefixedStaticLibraryDirectories%% -L%%s
)
set IncludeDirectories=%PrefixedIncludeDirectories%
set StaticLibraries=%PrefixedStaticLibraries%
set StaticLibraryDirectories=%PrefixedStaticLibraryDirectories%

@REM Set the compiler variables of the static libraries for the linking process.
set StaticWin32Libraries=-lwtsapi32 -lkernel32 -lshlwapi -lcmcfg32 -lpathcch -lshell32 -lUser32 -lUxTheme -lDwmapi -lGdi32
set StaticMinGWLibraries=-static-libgcc -static-libstdc++ -lpthread
set AllStaticLibraries=%StaticLibraries% %StaticWin32Libraries% %StaticMinGWLibraries%

@REM Set the variables of the debug mode building process.
set CompilerWarningOptions=-Wall -Wextra -Wformat=2 -Wunused -Wpedantic -Wshadow
set DebugBuildOptions=-D _DEBUG -O0 -g

@REM Set the variables of the release mode building process.
set ReleaseBuildOptions=-D NDEBUG -O2 -s

set StandardBuildOptions=-std=c++20 -m64 -municode -D UNICODE -D _UNICODE -D WINVER=0x0A00 -D _WIN32_WINNT=0x0A00

if /I "%BuildMode%"=="Debug" (
    set BuildOptions=%StandardBuildOptions% %DebugBuildOptions% %CompilerWarningOptions%
)
if /I "%BuildMode%"=="Release" (
    set BuildOptions=%StandardBuildOptions% %ReleaseBuildOptions%
)

echo . [^^!] Creating the %BuildMode% output folders ...

%MakeDir% -p %BinaryOutputsDirectory% %ObjectOutputsDirectory%

if %IsLibrary%==False (set ProjectTypeTitle=project) else (set ProjectTypeTitle=library project)

echo . [^^!] Building %ProjectName% %ProjectTypeTitle% in %BuildMode% mode ...

if not "%CustomBuildOperationFunction%"=="" goto :%CustomBuildOperationFunction%

if not "%PrecompiledHeader%"=="" set PrecompiledHeader=-include %PrecompiledHeader%%HeaderFilesExtension%

@REM Start the compiling process of current executable or static library project. Shared library projects does not supported yet.
@REM Create their object files from their source files of the current project for the linking operation.
@REM Source files used with their full paths. Relative paths do not used because can clickable in VSCode terminal in this way.
for %%s in (%SourceFiles%) do (
    %Compiler% -fdiagnostics-color=always %PrecompiledHeader% %IncludeDirectories% %BuildOptions% -c %SourceDirectory%\%%s%SourceFilesExtension% -o %ObjectOutputsDirectory%/%%s.o
)

if %IsCalledFromAnotherBuilder%==False (
    set "ArchiverOutputsDirectory=%BinaryOutputsDirectory%"
) else (
    set "ArchiverOutputsDirectory=%ObjectOutputsDirectory%"
)

@REM Start the linking process of current executable or static library project. Shared library projects does not supported yet.
if %IsLibrary%==False (
    %Compiler% %StaticLibraryDirectories% %BuildOptions% -o %BinaryOutputsDirectory%/%ProjectName%.exe %ObjectFiles% -static %AllStaticLibraries%
) else (
    %Archiver% rcs %ArchiverOutputsDirectory%/lib%ProjectName%.a %ObjectFiles%
)

goto :EndOfBuildOperation

endlocal

:EndOfBuildOperation

if /I "%IsKeepOpenEnabled%"=="True" goto :DoNotCloseAndPause
if /I "%IsKeepOpenEnabled%"=="False" goto :ExitTerminal

:DoNotCloseAndPause
pause

:ExitTerminal
exit /B 0

:FatalError
echo . [X] An fatal error occurred during the process, operation of the batch file terminated unexpectedly.

if /I "%IsKeepOpenEnabled%"=="True" goto :DoNotCloseAndPause
exit /B 1

:WaitForPendingOperationBeginWithOneDot
if %SendedArgumentsCount%==0 call ping 192.0.2.0 -n 1 -w %WaitForPendingOperationMilliSeconds%> nul
echo .
if %SendedArgumentsCount%==0 call ping 192.0.2.0 -n 1 -w %WaitForPendingOperationMilliSeconds%> nul
echo ..
if %SendedArgumentsCount%==0 call ping 192.0.2.0 -n 1 -w %WaitForPendingOperationMilliSeconds%> nul
echo ...
if %SendedArgumentsCount%==0 call ping 192.0.2.0 -n 1 -w %WaitForPendingOperationMilliSeconds%> nul
goto:eof

:WaitForPendingOperationBeginWithThreeDot
if %SendedArgumentsCount%==0 call ping 192.0.2.0 -n 1 -w %WaitForPendingOperationMilliSeconds%> nul
echo ...
if %SendedArgumentsCount%==0 call ping 192.0.2.0 -n 1 -w %WaitForPendingOperationMilliSeconds%> nul
echo ..
if %SendedArgumentsCount%==0 call ping 192.0.2.0 -n 1 -w %WaitForPendingOperationMilliSeconds%> nul
echo .
if %SendedArgumentsCount%==0 call ping 192.0.2.0 -n 1 -w %WaitForPendingOperationMilliSeconds%> nul
goto:eof
