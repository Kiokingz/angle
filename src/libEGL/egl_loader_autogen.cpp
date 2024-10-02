// GENERATED FILE - DO NOT EDIT.
// Generated by generate_loader.py using data from egl.xml and egl_angle_ext.xml.
//
// Copyright 2018 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// egl_loader_autogen.cpp:
//   Simple EGL function loader.

#include "egl_loader_autogen.h"

extern "C" {
PFNEGLCHOOSECONFIGPROC l_EGL_ChooseConfig;
PFNEGLCOPYBUFFERSPROC l_EGL_CopyBuffers;
PFNEGLCREATECONTEXTPROC l_EGL_CreateContext;
PFNEGLCREATEPBUFFERSURFACEPROC l_EGL_CreatePbufferSurface;
PFNEGLCREATEPIXMAPSURFACEPROC l_EGL_CreatePixmapSurface;
PFNEGLCREATEWINDOWSURFACEPROC l_EGL_CreateWindowSurface;
PFNEGLDESTROYCONTEXTPROC l_EGL_DestroyContext;
PFNEGLDESTROYSURFACEPROC l_EGL_DestroySurface;
PFNEGLGETCONFIGATTRIBPROC l_EGL_GetConfigAttrib;
PFNEGLGETCONFIGSPROC l_EGL_GetConfigs;
PFNEGLGETCURRENTDISPLAYPROC l_EGL_GetCurrentDisplay;
PFNEGLGETCURRENTSURFACEPROC l_EGL_GetCurrentSurface;
PFNEGLGETDISPLAYPROC l_EGL_GetDisplay;
PFNEGLGETERRORPROC l_EGL_GetError;
PFNEGLGETPROCADDRESSPROC l_EGL_GetProcAddress;
PFNEGLINITIALIZEPROC l_EGL_Initialize;
PFNEGLMAKECURRENTPROC l_EGL_MakeCurrent;
PFNEGLQUERYCONTEXTPROC l_EGL_QueryContext;
PFNEGLQUERYSTRINGPROC l_EGL_QueryString;
PFNEGLQUERYSURFACEPROC l_EGL_QuerySurface;
PFNEGLSWAPBUFFERSPROC l_EGL_SwapBuffers;
PFNEGLTERMINATEPROC l_EGL_Terminate;
PFNEGLWAITGLPROC l_EGL_WaitGL;
PFNEGLWAITNATIVEPROC l_EGL_WaitNative;
PFNEGLBINDTEXIMAGEPROC l_EGL_BindTexImage;
PFNEGLRELEASETEXIMAGEPROC l_EGL_ReleaseTexImage;
PFNEGLSURFACEATTRIBPROC l_EGL_SurfaceAttrib;
PFNEGLSWAPINTERVALPROC l_EGL_SwapInterval;
PFNEGLBINDAPIPROC l_EGL_BindAPI;
PFNEGLQUERYAPIPROC l_EGL_QueryAPI;
PFNEGLCREATEPBUFFERFROMCLIENTBUFFERPROC l_EGL_CreatePbufferFromClientBuffer;
PFNEGLRELEASETHREADPROC l_EGL_ReleaseThread;
PFNEGLWAITCLIENTPROC l_EGL_WaitClient;
PFNEGLGETCURRENTCONTEXTPROC l_EGL_GetCurrentContext;
PFNEGLCREATESYNCPROC l_EGL_CreateSync;
PFNEGLDESTROYSYNCPROC l_EGL_DestroySync;
PFNEGLCLIENTWAITSYNCPROC l_EGL_ClientWaitSync;
PFNEGLGETSYNCATTRIBPROC l_EGL_GetSyncAttrib;
PFNEGLCREATEIMAGEPROC l_EGL_CreateImage;
PFNEGLDESTROYIMAGEPROC l_EGL_DestroyImage;
PFNEGLGETPLATFORMDISPLAYPROC l_EGL_GetPlatformDisplay;
PFNEGLCREATEPLATFORMWINDOWSURFACEPROC l_EGL_CreatePlatformWindowSurface;
PFNEGLCREATEPLATFORMPIXMAPSURFACEPROC l_EGL_CreatePlatformPixmapSurface;
PFNEGLWAITSYNCPROC l_EGL_WaitSync;
PFNEGLSETBLOBCACHEFUNCSANDROIDPROC l_EGL_SetBlobCacheFuncsANDROID;
PFNEGLCREATENATIVECLIENTBUFFERANDROIDPROC l_EGL_CreateNativeClientBufferANDROID;
PFNEGLGETCOMPOSITORTIMINGANDROIDPROC l_EGL_GetCompositorTimingANDROID;
PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC l_EGL_GetCompositorTimingSupportedANDROID;
PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC l_EGL_GetFrameTimestampSupportedANDROID;
PFNEGLGETFRAMETIMESTAMPSANDROIDPROC l_EGL_GetFrameTimestampsANDROID;
PFNEGLGETNEXTFRAMEIDANDROIDPROC l_EGL_GetNextFrameIdANDROID;
PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC l_EGL_GetNativeClientBufferANDROID;
PFNEGLDUPNATIVEFENCEFDANDROIDPROC l_EGL_DupNativeFenceFDANDROID;
PFNEGLPRESENTATIONTIMEANDROIDPROC l_EGL_PresentationTimeANDROID;
PFNEGLCREATEDEVICEANGLEPROC l_EGL_CreateDeviceANGLE;
PFNEGLRELEASEDEVICEANGLEPROC l_EGL_ReleaseDeviceANGLE;
PFNEGLACQUIREEXTERNALCONTEXTANGLEPROC l_EGL_AcquireExternalContextANGLE;
PFNEGLRELEASEEXTERNALCONTEXTANGLEPROC l_EGL_ReleaseExternalContextANGLE;
PFNEGLQUERYDISPLAYATTRIBANGLEPROC l_EGL_QueryDisplayAttribANGLE;
PFNEGLQUERYSTRINGIANGLEPROC l_EGL_QueryStringiANGLE;
PFNEGLCOPYMETALSHAREDEVENTANGLEPROC l_EGL_CopyMetalSharedEventANGLE;
PFNEGLSETVALIDATIONENABLEDANGLEPROC l_EGL_SetValidationEnabledANGLE;
PFNEGLFORCEGPUSWITCHANGLEPROC l_EGL_ForceGPUSwitchANGLE;
PFNEGLHANDLEGPUSWITCHANGLEPROC l_EGL_HandleGPUSwitchANGLE;
PFNEGLREACQUIREHIGHPOWERGPUANGLEPROC l_EGL_ReacquireHighPowerGPUANGLE;
PFNEGLRELEASEHIGHPOWERGPUANGLEPROC l_EGL_ReleaseHighPowerGPUANGLE;
PFNEGLPREPARESWAPBUFFERSANGLEPROC l_EGL_PrepareSwapBuffersANGLE;
PFNEGLPROGRAMCACHEGETATTRIBANGLEPROC l_EGL_ProgramCacheGetAttribANGLE;
PFNEGLPROGRAMCACHEPOPULATEANGLEPROC l_EGL_ProgramCachePopulateANGLE;
PFNEGLPROGRAMCACHEQUERYANGLEPROC l_EGL_ProgramCacheQueryANGLE;
PFNEGLPROGRAMCACHERESIZEANGLEPROC l_EGL_ProgramCacheResizeANGLE;
PFNEGLQUERYSURFACEPOINTERANGLEPROC l_EGL_QuerySurfacePointerANGLE;
PFNEGLCREATESTREAMPRODUCERD3DTEXTUREANGLEPROC l_EGL_CreateStreamProducerD3DTextureANGLE;
PFNEGLSTREAMPOSTD3DTEXTUREANGLEPROC l_EGL_StreamPostD3DTextureANGLE;
PFNEGLSWAPBUFFERSWITHFRAMETOKENANGLEPROC l_EGL_SwapBuffersWithFrameTokenANGLE;
PFNEGLGETMSCRATEANGLEPROC l_EGL_GetMscRateANGLE;
PFNEGLEXPORTVKIMAGEANGLEPROC l_EGL_ExportVkImageANGLE;
PFNEGLWAITUNTILWORKSCHEDULEDANGLEPROC l_EGL_WaitUntilWorkScheduledANGLE;
PFNEGLGETSYNCVALUESCHROMIUMPROC l_EGL_GetSyncValuesCHROMIUM;
PFNEGLQUERYDEVICEATTRIBEXTPROC l_EGL_QueryDeviceAttribEXT;
PFNEGLQUERYDEVICESTRINGEXTPROC l_EGL_QueryDeviceStringEXT;
PFNEGLQUERYDISPLAYATTRIBEXTPROC l_EGL_QueryDisplayAttribEXT;
PFNEGLQUERYDMABUFFORMATSEXTPROC l_EGL_QueryDmaBufFormatsEXT;
PFNEGLQUERYDMABUFMODIFIERSEXTPROC l_EGL_QueryDmaBufModifiersEXT;
PFNEGLCREATEPLATFORMPIXMAPSURFACEEXTPROC l_EGL_CreatePlatformPixmapSurfaceEXT;
PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC l_EGL_CreatePlatformWindowSurfaceEXT;
PFNEGLGETPLATFORMDISPLAYEXTPROC l_EGL_GetPlatformDisplayEXT;
PFNEGLDEBUGMESSAGECONTROLKHRPROC l_EGL_DebugMessageControlKHR;
PFNEGLLABELOBJECTKHRPROC l_EGL_LabelObjectKHR;
PFNEGLQUERYDEBUGKHRPROC l_EGL_QueryDebugKHR;
PFNEGLCLIENTWAITSYNCKHRPROC l_EGL_ClientWaitSyncKHR;
PFNEGLCREATESYNCKHRPROC l_EGL_CreateSyncKHR;
PFNEGLDESTROYSYNCKHRPROC l_EGL_DestroySyncKHR;
PFNEGLGETSYNCATTRIBKHRPROC l_EGL_GetSyncAttribKHR;
PFNEGLCREATEIMAGEKHRPROC l_EGL_CreateImageKHR;
PFNEGLDESTROYIMAGEKHRPROC l_EGL_DestroyImageKHR;
PFNEGLLOCKSURFACEKHRPROC l_EGL_LockSurfaceKHR;
PFNEGLQUERYSURFACE64KHRPROC l_EGL_QuerySurface64KHR;
PFNEGLUNLOCKSURFACEKHRPROC l_EGL_UnlockSurfaceKHR;
PFNEGLSETDAMAGEREGIONKHRPROC l_EGL_SetDamageRegionKHR;
PFNEGLSIGNALSYNCKHRPROC l_EGL_SignalSyncKHR;
PFNEGLCREATESTREAMKHRPROC l_EGL_CreateStreamKHR;
PFNEGLDESTROYSTREAMKHRPROC l_EGL_DestroyStreamKHR;
PFNEGLQUERYSTREAMKHRPROC l_EGL_QueryStreamKHR;
PFNEGLQUERYSTREAMU64KHRPROC l_EGL_QueryStreamu64KHR;
PFNEGLSTREAMATTRIBKHRPROC l_EGL_StreamAttribKHR;
PFNEGLSTREAMCONSUMERACQUIREKHRPROC l_EGL_StreamConsumerAcquireKHR;
PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC l_EGL_StreamConsumerGLTextureExternalKHR;
PFNEGLSTREAMCONSUMERRELEASEKHRPROC l_EGL_StreamConsumerReleaseKHR;
PFNEGLSWAPBUFFERSWITHDAMAGEKHRPROC l_EGL_SwapBuffersWithDamageKHR;
PFNEGLWAITSYNCKHRPROC l_EGL_WaitSyncKHR;
PFNEGLPOSTSUBBUFFERNVPROC l_EGL_PostSubBufferNV;
PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALATTRIBSNVPROC l_EGL_StreamConsumerGLTextureExternalAttribsNV;

void LoadLibEGL_EGL(LoadProc loadProc)
{
    l_EGL_ChooseConfig  = reinterpret_cast<PFNEGLCHOOSECONFIGPROC>(loadProc("EGL_ChooseConfig"));
    l_EGL_CopyBuffers   = reinterpret_cast<PFNEGLCOPYBUFFERSPROC>(loadProc("EGL_CopyBuffers"));
    l_EGL_CreateContext = reinterpret_cast<PFNEGLCREATECONTEXTPROC>(loadProc("EGL_CreateContext"));
    l_EGL_CreatePbufferSurface =
        reinterpret_cast<PFNEGLCREATEPBUFFERSURFACEPROC>(loadProc("EGL_CreatePbufferSurface"));
    l_EGL_CreatePixmapSurface =
        reinterpret_cast<PFNEGLCREATEPIXMAPSURFACEPROC>(loadProc("EGL_CreatePixmapSurface"));
    l_EGL_CreateWindowSurface =
        reinterpret_cast<PFNEGLCREATEWINDOWSURFACEPROC>(loadProc("EGL_CreateWindowSurface"));
    l_EGL_DestroyContext =
        reinterpret_cast<PFNEGLDESTROYCONTEXTPROC>(loadProc("EGL_DestroyContext"));
    l_EGL_DestroySurface =
        reinterpret_cast<PFNEGLDESTROYSURFACEPROC>(loadProc("EGL_DestroySurface"));
    l_EGL_GetConfigAttrib =
        reinterpret_cast<PFNEGLGETCONFIGATTRIBPROC>(loadProc("EGL_GetConfigAttrib"));
    l_EGL_GetConfigs = reinterpret_cast<PFNEGLGETCONFIGSPROC>(loadProc("EGL_GetConfigs"));
    l_EGL_GetCurrentDisplay =
        reinterpret_cast<PFNEGLGETCURRENTDISPLAYPROC>(loadProc("EGL_GetCurrentDisplay"));
    l_EGL_GetCurrentSurface =
        reinterpret_cast<PFNEGLGETCURRENTSURFACEPROC>(loadProc("EGL_GetCurrentSurface"));
    l_EGL_GetDisplay = reinterpret_cast<PFNEGLGETDISPLAYPROC>(loadProc("EGL_GetDisplay"));
    l_EGL_GetError   = reinterpret_cast<PFNEGLGETERRORPROC>(loadProc("EGL_GetError"));
    l_EGL_GetProcAddress =
        reinterpret_cast<PFNEGLGETPROCADDRESSPROC>(loadProc("EGL_GetProcAddress"));
    l_EGL_Initialize   = reinterpret_cast<PFNEGLINITIALIZEPROC>(loadProc("EGL_Initialize"));
    l_EGL_MakeCurrent  = reinterpret_cast<PFNEGLMAKECURRENTPROC>(loadProc("EGL_MakeCurrent"));
    l_EGL_QueryContext = reinterpret_cast<PFNEGLQUERYCONTEXTPROC>(loadProc("EGL_QueryContext"));
    l_EGL_QueryString  = reinterpret_cast<PFNEGLQUERYSTRINGPROC>(loadProc("EGL_QueryString"));
    l_EGL_QuerySurface = reinterpret_cast<PFNEGLQUERYSURFACEPROC>(loadProc("EGL_QuerySurface"));
    l_EGL_SwapBuffers  = reinterpret_cast<PFNEGLSWAPBUFFERSPROC>(loadProc("EGL_SwapBuffers"));
    l_EGL_Terminate    = reinterpret_cast<PFNEGLTERMINATEPROC>(loadProc("EGL_Terminate"));
    l_EGL_WaitGL       = reinterpret_cast<PFNEGLWAITGLPROC>(loadProc("EGL_WaitGL"));
    l_EGL_WaitNative   = reinterpret_cast<PFNEGLWAITNATIVEPROC>(loadProc("EGL_WaitNative"));
    l_EGL_BindTexImage = reinterpret_cast<PFNEGLBINDTEXIMAGEPROC>(loadProc("EGL_BindTexImage"));
    l_EGL_ReleaseTexImage =
        reinterpret_cast<PFNEGLRELEASETEXIMAGEPROC>(loadProc("EGL_ReleaseTexImage"));
    l_EGL_SurfaceAttrib = reinterpret_cast<PFNEGLSURFACEATTRIBPROC>(loadProc("EGL_SurfaceAttrib"));
    l_EGL_SwapInterval  = reinterpret_cast<PFNEGLSWAPINTERVALPROC>(loadProc("EGL_SwapInterval"));
    l_EGL_BindAPI       = reinterpret_cast<PFNEGLBINDAPIPROC>(loadProc("EGL_BindAPI"));
    l_EGL_QueryAPI      = reinterpret_cast<PFNEGLQUERYAPIPROC>(loadProc("EGL_QueryAPI"));
    l_EGL_CreatePbufferFromClientBuffer = reinterpret_cast<PFNEGLCREATEPBUFFERFROMCLIENTBUFFERPROC>(
        loadProc("EGL_CreatePbufferFromClientBuffer"));
    l_EGL_ReleaseThread = reinterpret_cast<PFNEGLRELEASETHREADPROC>(loadProc("EGL_ReleaseThread"));
    l_EGL_WaitClient    = reinterpret_cast<PFNEGLWAITCLIENTPROC>(loadProc("EGL_WaitClient"));
    l_EGL_GetCurrentContext =
        reinterpret_cast<PFNEGLGETCURRENTCONTEXTPROC>(loadProc("EGL_GetCurrentContext"));
    l_EGL_CreateSync  = reinterpret_cast<PFNEGLCREATESYNCPROC>(loadProc("EGL_CreateSync"));
    l_EGL_DestroySync = reinterpret_cast<PFNEGLDESTROYSYNCPROC>(loadProc("EGL_DestroySync"));
    l_EGL_ClientWaitSync =
        reinterpret_cast<PFNEGLCLIENTWAITSYNCPROC>(loadProc("EGL_ClientWaitSync"));
    l_EGL_GetSyncAttrib = reinterpret_cast<PFNEGLGETSYNCATTRIBPROC>(loadProc("EGL_GetSyncAttrib"));
    l_EGL_CreateImage   = reinterpret_cast<PFNEGLCREATEIMAGEPROC>(loadProc("EGL_CreateImage"));
    l_EGL_DestroyImage  = reinterpret_cast<PFNEGLDESTROYIMAGEPROC>(loadProc("EGL_DestroyImage"));
    l_EGL_GetPlatformDisplay =
        reinterpret_cast<PFNEGLGETPLATFORMDISPLAYPROC>(loadProc("EGL_GetPlatformDisplay"));
    l_EGL_CreatePlatformWindowSurface = reinterpret_cast<PFNEGLCREATEPLATFORMWINDOWSURFACEPROC>(
        loadProc("EGL_CreatePlatformWindowSurface"));
    l_EGL_CreatePlatformPixmapSurface = reinterpret_cast<PFNEGLCREATEPLATFORMPIXMAPSURFACEPROC>(
        loadProc("EGL_CreatePlatformPixmapSurface"));
    l_EGL_WaitSync                 = reinterpret_cast<PFNEGLWAITSYNCPROC>(loadProc("EGL_WaitSync"));
    l_EGL_SetBlobCacheFuncsANDROID = reinterpret_cast<PFNEGLSETBLOBCACHEFUNCSANDROIDPROC>(
        loadProc("EGL_SetBlobCacheFuncsANDROID"));
    l_EGL_CreateNativeClientBufferANDROID =
        reinterpret_cast<PFNEGLCREATENATIVECLIENTBUFFERANDROIDPROC>(
            loadProc("EGL_CreateNativeClientBufferANDROID"));
    l_EGL_GetCompositorTimingANDROID = reinterpret_cast<PFNEGLGETCOMPOSITORTIMINGANDROIDPROC>(
        loadProc("EGL_GetCompositorTimingANDROID"));
    l_EGL_GetCompositorTimingSupportedANDROID =
        reinterpret_cast<PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC>(
            loadProc("EGL_GetCompositorTimingSupportedANDROID"));
    l_EGL_GetFrameTimestampSupportedANDROID =
        reinterpret_cast<PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC>(
            loadProc("EGL_GetFrameTimestampSupportedANDROID"));
    l_EGL_GetFrameTimestampsANDROID = reinterpret_cast<PFNEGLGETFRAMETIMESTAMPSANDROIDPROC>(
        loadProc("EGL_GetFrameTimestampsANDROID"));
    l_EGL_GetNextFrameIdANDROID =
        reinterpret_cast<PFNEGLGETNEXTFRAMEIDANDROIDPROC>(loadProc("EGL_GetNextFrameIdANDROID"));
    l_EGL_GetNativeClientBufferANDROID = reinterpret_cast<PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC>(
        loadProc("EGL_GetNativeClientBufferANDROID"));
    l_EGL_DupNativeFenceFDANDROID = reinterpret_cast<PFNEGLDUPNATIVEFENCEFDANDROIDPROC>(
        loadProc("EGL_DupNativeFenceFDANDROID"));
    l_EGL_PresentationTimeANDROID = reinterpret_cast<PFNEGLPRESENTATIONTIMEANDROIDPROC>(
        loadProc("EGL_PresentationTimeANDROID"));
    l_EGL_CreateDeviceANGLE =
        reinterpret_cast<PFNEGLCREATEDEVICEANGLEPROC>(loadProc("EGL_CreateDeviceANGLE"));
    l_EGL_ReleaseDeviceANGLE =
        reinterpret_cast<PFNEGLRELEASEDEVICEANGLEPROC>(loadProc("EGL_ReleaseDeviceANGLE"));
    l_EGL_AcquireExternalContextANGLE = reinterpret_cast<PFNEGLACQUIREEXTERNALCONTEXTANGLEPROC>(
        loadProc("EGL_AcquireExternalContextANGLE"));
    l_EGL_ReleaseExternalContextANGLE = reinterpret_cast<PFNEGLRELEASEEXTERNALCONTEXTANGLEPROC>(
        loadProc("EGL_ReleaseExternalContextANGLE"));
    l_EGL_QueryDisplayAttribANGLE = reinterpret_cast<PFNEGLQUERYDISPLAYATTRIBANGLEPROC>(
        loadProc("EGL_QueryDisplayAttribANGLE"));
    l_EGL_QueryStringiANGLE =
        reinterpret_cast<PFNEGLQUERYSTRINGIANGLEPROC>(loadProc("EGL_QueryStringiANGLE"));
    l_EGL_CopyMetalSharedEventANGLE = reinterpret_cast<PFNEGLCOPYMETALSHAREDEVENTANGLEPROC>(
        loadProc("EGL_CopyMetalSharedEventANGLE"));
    l_EGL_SetValidationEnabledANGLE = reinterpret_cast<PFNEGLSETVALIDATIONENABLEDANGLEPROC>(
        loadProc("EGL_SetValidationEnabledANGLE"));
    l_EGL_ForceGPUSwitchANGLE =
        reinterpret_cast<PFNEGLFORCEGPUSWITCHANGLEPROC>(loadProc("EGL_ForceGPUSwitchANGLE"));
    l_EGL_HandleGPUSwitchANGLE =
        reinterpret_cast<PFNEGLHANDLEGPUSWITCHANGLEPROC>(loadProc("EGL_HandleGPUSwitchANGLE"));
    l_EGL_ReacquireHighPowerGPUANGLE = reinterpret_cast<PFNEGLREACQUIREHIGHPOWERGPUANGLEPROC>(
        loadProc("EGL_ReacquireHighPowerGPUANGLE"));
    l_EGL_ReleaseHighPowerGPUANGLE = reinterpret_cast<PFNEGLRELEASEHIGHPOWERGPUANGLEPROC>(
        loadProc("EGL_ReleaseHighPowerGPUANGLE"));
    l_EGL_PrepareSwapBuffersANGLE = reinterpret_cast<PFNEGLPREPARESWAPBUFFERSANGLEPROC>(
        loadProc("EGL_PrepareSwapBuffersANGLE"));
    l_EGL_ProgramCacheGetAttribANGLE = reinterpret_cast<PFNEGLPROGRAMCACHEGETATTRIBANGLEPROC>(
        loadProc("EGL_ProgramCacheGetAttribANGLE"));
    l_EGL_ProgramCachePopulateANGLE = reinterpret_cast<PFNEGLPROGRAMCACHEPOPULATEANGLEPROC>(
        loadProc("EGL_ProgramCachePopulateANGLE"));
    l_EGL_ProgramCacheQueryANGLE =
        reinterpret_cast<PFNEGLPROGRAMCACHEQUERYANGLEPROC>(loadProc("EGL_ProgramCacheQueryANGLE"));
    l_EGL_ProgramCacheResizeANGLE = reinterpret_cast<PFNEGLPROGRAMCACHERESIZEANGLEPROC>(
        loadProc("EGL_ProgramCacheResizeANGLE"));
    l_EGL_QuerySurfacePointerANGLE = reinterpret_cast<PFNEGLQUERYSURFACEPOINTERANGLEPROC>(
        loadProc("EGL_QuerySurfacePointerANGLE"));
    l_EGL_CreateStreamProducerD3DTextureANGLE =
        reinterpret_cast<PFNEGLCREATESTREAMPRODUCERD3DTEXTUREANGLEPROC>(
            loadProc("EGL_CreateStreamProducerD3DTextureANGLE"));
    l_EGL_StreamPostD3DTextureANGLE = reinterpret_cast<PFNEGLSTREAMPOSTD3DTEXTUREANGLEPROC>(
        loadProc("EGL_StreamPostD3DTextureANGLE"));
    l_EGL_SwapBuffersWithFrameTokenANGLE =
        reinterpret_cast<PFNEGLSWAPBUFFERSWITHFRAMETOKENANGLEPROC>(
            loadProc("EGL_SwapBuffersWithFrameTokenANGLE"));
    l_EGL_GetMscRateANGLE =
        reinterpret_cast<PFNEGLGETMSCRATEANGLEPROC>(loadProc("EGL_GetMscRateANGLE"));
    l_EGL_ExportVkImageANGLE =
        reinterpret_cast<PFNEGLEXPORTVKIMAGEANGLEPROC>(loadProc("EGL_ExportVkImageANGLE"));
    l_EGL_WaitUntilWorkScheduledANGLE = reinterpret_cast<PFNEGLWAITUNTILWORKSCHEDULEDANGLEPROC>(
        loadProc("EGL_WaitUntilWorkScheduledANGLE"));
    l_EGL_GetSyncValuesCHROMIUM =
        reinterpret_cast<PFNEGLGETSYNCVALUESCHROMIUMPROC>(loadProc("EGL_GetSyncValuesCHROMIUM"));
    l_EGL_QueryDeviceAttribEXT =
        reinterpret_cast<PFNEGLQUERYDEVICEATTRIBEXTPROC>(loadProc("EGL_QueryDeviceAttribEXT"));
    l_EGL_QueryDeviceStringEXT =
        reinterpret_cast<PFNEGLQUERYDEVICESTRINGEXTPROC>(loadProc("EGL_QueryDeviceStringEXT"));
    l_EGL_QueryDisplayAttribEXT =
        reinterpret_cast<PFNEGLQUERYDISPLAYATTRIBEXTPROC>(loadProc("EGL_QueryDisplayAttribEXT"));
    l_EGL_QueryDmaBufFormatsEXT =
        reinterpret_cast<PFNEGLQUERYDMABUFFORMATSEXTPROC>(loadProc("EGL_QueryDmaBufFormatsEXT"));
    l_EGL_QueryDmaBufModifiersEXT = reinterpret_cast<PFNEGLQUERYDMABUFMODIFIERSEXTPROC>(
        loadProc("EGL_QueryDmaBufModifiersEXT"));
    l_EGL_CreatePlatformPixmapSurfaceEXT =
        reinterpret_cast<PFNEGLCREATEPLATFORMPIXMAPSURFACEEXTPROC>(
            loadProc("EGL_CreatePlatformPixmapSurfaceEXT"));
    l_EGL_CreatePlatformWindowSurfaceEXT =
        reinterpret_cast<PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC>(
            loadProc("EGL_CreatePlatformWindowSurfaceEXT"));
    l_EGL_GetPlatformDisplayEXT =
        reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(loadProc("EGL_GetPlatformDisplayEXT"));
    l_EGL_DebugMessageControlKHR =
        reinterpret_cast<PFNEGLDEBUGMESSAGECONTROLKHRPROC>(loadProc("EGL_DebugMessageControlKHR"));
    l_EGL_LabelObjectKHR =
        reinterpret_cast<PFNEGLLABELOBJECTKHRPROC>(loadProc("EGL_LabelObjectKHR"));
    l_EGL_QueryDebugKHR = reinterpret_cast<PFNEGLQUERYDEBUGKHRPROC>(loadProc("EGL_QueryDebugKHR"));
    l_EGL_ClientWaitSyncKHR =
        reinterpret_cast<PFNEGLCLIENTWAITSYNCKHRPROC>(loadProc("EGL_ClientWaitSyncKHR"));
    l_EGL_CreateSyncKHR = reinterpret_cast<PFNEGLCREATESYNCKHRPROC>(loadProc("EGL_CreateSyncKHR"));
    l_EGL_DestroySyncKHR =
        reinterpret_cast<PFNEGLDESTROYSYNCKHRPROC>(loadProc("EGL_DestroySyncKHR"));
    l_EGL_GetSyncAttribKHR =
        reinterpret_cast<PFNEGLGETSYNCATTRIBKHRPROC>(loadProc("EGL_GetSyncAttribKHR"));
    l_EGL_CreateImageKHR =
        reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(loadProc("EGL_CreateImageKHR"));
    l_EGL_DestroyImageKHR =
        reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(loadProc("EGL_DestroyImageKHR"));
    l_EGL_LockSurfaceKHR =
        reinterpret_cast<PFNEGLLOCKSURFACEKHRPROC>(loadProc("EGL_LockSurfaceKHR"));
    l_EGL_QuerySurface64KHR =
        reinterpret_cast<PFNEGLQUERYSURFACE64KHRPROC>(loadProc("EGL_QuerySurface64KHR"));
    l_EGL_UnlockSurfaceKHR =
        reinterpret_cast<PFNEGLUNLOCKSURFACEKHRPROC>(loadProc("EGL_UnlockSurfaceKHR"));
    l_EGL_SetDamageRegionKHR =
        reinterpret_cast<PFNEGLSETDAMAGEREGIONKHRPROC>(loadProc("EGL_SetDamageRegionKHR"));
    l_EGL_SignalSyncKHR = reinterpret_cast<PFNEGLSIGNALSYNCKHRPROC>(loadProc("EGL_SignalSyncKHR"));
    l_EGL_CreateStreamKHR =
        reinterpret_cast<PFNEGLCREATESTREAMKHRPROC>(loadProc("EGL_CreateStreamKHR"));
    l_EGL_DestroyStreamKHR =
        reinterpret_cast<PFNEGLDESTROYSTREAMKHRPROC>(loadProc("EGL_DestroyStreamKHR"));
    l_EGL_QueryStreamKHR =
        reinterpret_cast<PFNEGLQUERYSTREAMKHRPROC>(loadProc("EGL_QueryStreamKHR"));
    l_EGL_QueryStreamu64KHR =
        reinterpret_cast<PFNEGLQUERYSTREAMU64KHRPROC>(loadProc("EGL_QueryStreamu64KHR"));
    l_EGL_StreamAttribKHR =
        reinterpret_cast<PFNEGLSTREAMATTRIBKHRPROC>(loadProc("EGL_StreamAttribKHR"));
    l_EGL_StreamConsumerAcquireKHR = reinterpret_cast<PFNEGLSTREAMCONSUMERACQUIREKHRPROC>(
        loadProc("EGL_StreamConsumerAcquireKHR"));
    l_EGL_StreamConsumerGLTextureExternalKHR =
        reinterpret_cast<PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC>(
            loadProc("EGL_StreamConsumerGLTextureExternalKHR"));
    l_EGL_StreamConsumerReleaseKHR = reinterpret_cast<PFNEGLSTREAMCONSUMERRELEASEKHRPROC>(
        loadProc("EGL_StreamConsumerReleaseKHR"));
    l_EGL_SwapBuffersWithDamageKHR = reinterpret_cast<PFNEGLSWAPBUFFERSWITHDAMAGEKHRPROC>(
        loadProc("EGL_SwapBuffersWithDamageKHR"));
    l_EGL_WaitSyncKHR = reinterpret_cast<PFNEGLWAITSYNCKHRPROC>(loadProc("EGL_WaitSyncKHR"));
    l_EGL_PostSubBufferNV =
        reinterpret_cast<PFNEGLPOSTSUBBUFFERNVPROC>(loadProc("EGL_PostSubBufferNV"));
    l_EGL_StreamConsumerGLTextureExternalAttribsNV =
        reinterpret_cast<PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALATTRIBSNVPROC>(
            loadProc("EGL_StreamConsumerGLTextureExternalAttribsNV"));
}
}  // extern "C"
