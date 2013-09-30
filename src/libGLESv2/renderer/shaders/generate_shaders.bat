@ECHO OFF
REM
REM Copyright (c) 2013 The ANGLE Project Authors. All rights reserved.
REM Use of this source code is governed by a BSD-style license that can be
REM found in the LICENSE file.
REM

PATH %PATH%;%ProgramFiles(x86)%\Windows Kits\8.0\bin\x86;%DXSDK_DIR%\Utilities\bin\x86

setlocal
set errorCount=0
set successCount=0
set debug=0

if "%1" == "debug" (
    set debug=1
)
if "%1" == "release" (
    set debug=0
)

::              | Input file          | Entry point           | Type | Output file                        | Debug |
call:BuildShader Blit.vs               standardvs              vs_2_0 compiled\standardvs.h                %debug%
call:BuildShader Blit.vs               flipyvs                 vs_2_0 compiled\flipyvs.h                   %debug%
call:BuildShader Blit.ps               passthroughps           ps_2_0 compiled\passthroughps.h             %debug%
call:BuildShader Blit.ps               luminanceps             ps_2_0 compiled\luminanceps.h               %debug%
call:BuildShader Blit.ps               componentmaskps         ps_2_0 compiled\componentmaskps.h           %debug%

call:BuildShader Passthrough2D11.hlsl VS_Passthrough2D         vs_4_0 compiled\passthrough2d11vs.h         %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughDepth2D    ps_4_0 compiled\passthroughdepth2d11ps.h    %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughRGBA2D     ps_4_0 compiled\passthroughrgba2d11ps.h     %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughRGBA2DUI   ps_4_0 compiled\passthroughrgba2dui11ps.h   %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughRGBA2DI    ps_4_0 compiled\passthroughrgba2di11ps.h    %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughRGB2D      ps_4_0 compiled\passthroughrgb2d11ps.h      %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughRGB2DUI    ps_4_0 compiled\passthroughrgb2dui11ps.h    %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughRGB2DI     ps_4_0 compiled\passthroughrgb2di11ps.h     %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughRG2D       ps_4_0 compiled\passthroughrg2d11ps.h       %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughRG2DUI     ps_4_0 compiled\passthroughrg2dui11ps.h     %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughRG2DI      ps_4_0 compiled\passthroughrg2di11ps.h      %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughR2D        ps_4_0 compiled\passthroughr2d11ps.h        %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughR2DUI      ps_4_0 compiled\passthroughr2dui11ps.h      %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughR2DI       ps_4_0 compiled\passthroughr2di11ps.h       %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughLum2D      ps_4_0 compiled\passthroughlum2d11ps.h      %debug%
call:BuildShader Passthrough2D11.hlsl PS_PassthroughLumAlpha2D ps_4_0 compiled\passthroughlumalpha2d11ps.h %debug%


call:BuildShader Passthrough3D11.hlsl VS_Passthrough3D         vs_4_0 compiled\passthrough3d11vs.h         %debug%
call:BuildShader Passthrough3D11.hlsl GS_Passthrough3D         gs_4_0 compiled\passthrough3d11gs.h         %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughRGBA3D     ps_4_0 compiled\passthroughrgba3d11ps.h     %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughRGBA3DUI   ps_4_0 compiled\passthroughrgba3dui11ps.h   %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughRGBA3DI    ps_4_0 compiled\passthroughrgba3di11ps.h    %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughRGB3D      ps_4_0 compiled\passthroughrgb3d11ps.h      %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughRGB3DUI    ps_4_0 compiled\passthroughrgb3dui11ps.h    %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughRGB3DI     ps_4_0 compiled\passthroughrgb3di11ps.h     %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughRG3D       ps_4_0 compiled\passthroughrg3d11ps.h       %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughRG3DUI     ps_4_0 compiled\passthroughrg3dui11ps.h     %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughRG3DI      ps_4_0 compiled\passthroughrg3di11ps.h      %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughR3D        ps_4_0 compiled\passthroughr3d11ps.h        %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughR3DUI      ps_4_0 compiled\passthroughr3dui11ps.h      %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughR3DI       ps_4_0 compiled\passthroughr3di11ps.h       %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughLum3D      ps_4_0 compiled\passthroughlum3d11ps.h      %debug%
call:BuildShader Passthrough3D11.hlsl PS_PassthroughLumAlpha3D ps_4_0 compiled\passthroughlumalpha3d11ps.h %debug%

call:BuildShader Clear11.hlsl         VS_ClearFloat            vs_4_0 compiled\clearfloat11vs.h            %debug%
call:BuildShader Clear11.hlsl         PS_ClearFloat            ps_4_0 compiled\clearfloat11ps.h            %debug%

call:BuildShader Clear11.hlsl         VS_ClearUint             vs_4_0 compiled\clearuint11vs.h             %debug%
call:BuildShader Clear11.hlsl         PS_ClearUint             ps_4_0 compiled\clearuint11ps.h             %debug%

call:BuildShader Clear11.hlsl         VS_ClearSint             vs_4_0 compiled\clearsint11vs.h             %debug%
call:BuildShader Clear11.hlsl         PS_ClearSint             ps_4_0 compiled\clearsint11ps.h             %debug%

echo.

if %successCount% GTR 0 (
   echo %successCount% shaders compiled successfully.
)
if %errorCount% GTR 0 (
   echo There were %errorCount% shader compilation errors.
)

endlocal
exit /b

:BuildShader
set input=%~1
set entry=%~2
set type=%~3
set output=%~4
set debug=%~5

if %debug% == 0 (
    set "buildCMD=fxc /nologo /E %entry% /T %type% /Fh %output% %input%"
) else (
    set "buildCMD=fxc /nologo /Zi /Od /E %entry% /T %type% /Fh %output% %input%"
)

set error=0
%buildCMD% || set error=1

if %error% == 0 (
    set /a successCount=%successCount%+1
) else (
    set /a errorCount=%errorCount%+1
)

exit /b
