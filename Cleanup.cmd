
rd /s /q "..\build-SerialPrograms*"

::cd %~dp0\ClientProject
::@call Cleanup.cmd

cd %~dp0\NativePrograms
@call Cleanup.cmd

cd %~dp0\HexGenerator
@call Cleanup.cmd

cd %~dp0\HexGeneratorLauncher
@call Cleanup.cmd

cd %~dp0\SerialPrograms
@call Cleanup.cmd



rd /s /q "..\build-HexGenerator-Desktop_Qt_5_12_12_MinGW_32_bit-Debug"
rd /s /q "..\build-HexGenerator-Desktop_Qt_5_12_12_MinGW_32_bit-Release"
rd /s /q "..\build-SerialPrograms-Desktop_Qt_5_12_12_MSVC2017_64bit-Release"
rd /s /q "..\build-SerialPrograms-Desktop_Qt_5_12_12_MSVC2017_64bit-RelWithDebInfo"
rd /s /q "..\build-SerialPrograms-Desktop_Qt_6_3_1_MSVC2019_64bit-Debug"
rd /s /q "..\build-SerialPrograms-Desktop_Qt_6_3_1_MSVC2019_64bit-RelWithDebInfo"
rd /s /q "..\build-SerialPrograms-Desktop_Qt_6_3_2_MSVC2019_64bit-Debug"
rd /s /q "..\build-SerialPrograms-Desktop_Qt_6_3_2_MSVC2019_64bit-RelWithDebInfo"
rd /s /q "..\build-SerialPrograms-Desktop_Qt_6_4_0_MSVC2019_64bit-Debug"
rd /s /q "..\build-SerialPrograms-Desktop_Qt_6_4_0_MSVC2019_64bit-RelWithDebInfo"
