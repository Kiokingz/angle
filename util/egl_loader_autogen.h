// GENERATED FILE - DO NOT EDIT.
// Generated by generate_loader.py using data from egl.xml and egl_angle_ext.xml.
//
// Copyright 2018 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// egl_loader_autogen.h:
//   Simple EGL function loader.

#ifndef UTIL_EGL_LOADER_AUTOGEN_H_
#define UTIL_EGL_LOADER_AUTOGEN_H_

#include "util/util_export.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

#define eglChooseConfig l_eglChooseConfig
#define eglCopyBuffers l_eglCopyBuffers
#define eglCreateContext l_eglCreateContext
#define eglCreatePbufferSurface l_eglCreatePbufferSurface
#define eglCreatePixmapSurface l_eglCreatePixmapSurface
#define eglCreateWindowSurface l_eglCreateWindowSurface
#define eglDestroyContext l_eglDestroyContext
#define eglDestroySurface l_eglDestroySurface
#define eglGetConfigAttrib l_eglGetConfigAttrib
#define eglGetConfigs l_eglGetConfigs
#define eglGetCurrentDisplay l_eglGetCurrentDisplay
#define eglGetCurrentSurface l_eglGetCurrentSurface
#define eglGetDisplay l_eglGetDisplay
#define eglGetError l_eglGetError
#define eglGetProcAddress l_eglGetProcAddress
#define eglInitialize l_eglInitialize
#define eglMakeCurrent l_eglMakeCurrent
#define eglQueryContext l_eglQueryContext
#define eglQueryString l_eglQueryString
#define eglQuerySurface l_eglQuerySurface
#define eglSwapBuffers l_eglSwapBuffers
#define eglTerminate l_eglTerminate
#define eglWaitGL l_eglWaitGL
#define eglWaitNative l_eglWaitNative
#define eglBindTexImage l_eglBindTexImage
#define eglReleaseTexImage l_eglReleaseTexImage
#define eglSurfaceAttrib l_eglSurfaceAttrib
#define eglSwapInterval l_eglSwapInterval
#define eglBindAPI l_eglBindAPI
#define eglQueryAPI l_eglQueryAPI
#define eglCreatePbufferFromClientBuffer l_eglCreatePbufferFromClientBuffer
#define eglReleaseThread l_eglReleaseThread
#define eglWaitClient l_eglWaitClient
#define eglGetCurrentContext l_eglGetCurrentContext
#define eglCreateSync l_eglCreateSync
#define eglDestroySync l_eglDestroySync
#define eglClientWaitSync l_eglClientWaitSync
#define eglGetSyncAttrib l_eglGetSyncAttrib
#define eglCreateImage l_eglCreateImage
#define eglDestroyImage l_eglDestroyImage
#define eglGetPlatformDisplay l_eglGetPlatformDisplay
#define eglCreatePlatformWindowSurface l_eglCreatePlatformWindowSurface
#define eglCreatePlatformPixmapSurface l_eglCreatePlatformPixmapSurface
#define eglWaitSync l_eglWaitSync
#define eglSetBlobCacheFuncsANDROID l_eglSetBlobCacheFuncsANDROID
#define eglCreateNativeClientBufferANDROID l_eglCreateNativeClientBufferANDROID
#define eglGetCompositorTimingANDROID l_eglGetCompositorTimingANDROID
#define eglGetCompositorTimingSupportedANDROID l_eglGetCompositorTimingSupportedANDROID
#define eglGetFrameTimestampSupportedANDROID l_eglGetFrameTimestampSupportedANDROID
#define eglGetFrameTimestampsANDROID l_eglGetFrameTimestampsANDROID
#define eglGetNextFrameIdANDROID l_eglGetNextFrameIdANDROID
#define eglGetNativeClientBufferANDROID l_eglGetNativeClientBufferANDROID
#define eglDupNativeFenceFDANDROID l_eglDupNativeFenceFDANDROID
#define eglPresentationTimeANDROID l_eglPresentationTimeANDROID
#define eglCreateDeviceANGLE l_eglCreateDeviceANGLE
#define eglReleaseDeviceANGLE l_eglReleaseDeviceANGLE
#define eglAcquireExternalContextANGLE l_eglAcquireExternalContextANGLE
#define eglReleaseExternalContextANGLE l_eglReleaseExternalContextANGLE
#define eglQueryDisplayAttribANGLE l_eglQueryDisplayAttribANGLE
#define eglQueryStringiANGLE l_eglQueryStringiANGLE
#define eglCopyMetalSharedEventANGLE l_eglCopyMetalSharedEventANGLE
#define eglSetValidationEnabledANGLE l_eglSetValidationEnabledANGLE
#define eglForceGPUSwitchANGLE l_eglForceGPUSwitchANGLE
#define eglHandleGPUSwitchANGLE l_eglHandleGPUSwitchANGLE
#define eglReacquireHighPowerGPUANGLE l_eglReacquireHighPowerGPUANGLE
#define eglReleaseHighPowerGPUANGLE l_eglReleaseHighPowerGPUANGLE
#define eglPrepareSwapBuffersANGLE l_eglPrepareSwapBuffersANGLE
#define eglProgramCacheGetAttribANGLE l_eglProgramCacheGetAttribANGLE
#define eglProgramCachePopulateANGLE l_eglProgramCachePopulateANGLE
#define eglProgramCacheQueryANGLE l_eglProgramCacheQueryANGLE
#define eglProgramCacheResizeANGLE l_eglProgramCacheResizeANGLE
#define eglQuerySurfacePointerANGLE l_eglQuerySurfacePointerANGLE
#define eglCreateStreamProducerD3DTextureANGLE l_eglCreateStreamProducerD3DTextureANGLE
#define eglStreamPostD3DTextureANGLE l_eglStreamPostD3DTextureANGLE
#define eglSwapBuffersWithFrameTokenANGLE l_eglSwapBuffersWithFrameTokenANGLE
#define eglGetMscRateANGLE l_eglGetMscRateANGLE
#define eglExportVkImageANGLE l_eglExportVkImageANGLE
#define eglWaitUntilWorkScheduledANGLE l_eglWaitUntilWorkScheduledANGLE
#define eglGetSyncValuesCHROMIUM l_eglGetSyncValuesCHROMIUM
#define eglQueryDeviceAttribEXT l_eglQueryDeviceAttribEXT
#define eglQueryDeviceStringEXT l_eglQueryDeviceStringEXT
#define eglQueryDisplayAttribEXT l_eglQueryDisplayAttribEXT
#define eglQueryDmaBufFormatsEXT l_eglQueryDmaBufFormatsEXT
#define eglQueryDmaBufModifiersEXT l_eglQueryDmaBufModifiersEXT
#define eglCreatePlatformPixmapSurfaceEXT l_eglCreatePlatformPixmapSurfaceEXT
#define eglCreatePlatformWindowSurfaceEXT l_eglCreatePlatformWindowSurfaceEXT
#define eglGetPlatformDisplayEXT l_eglGetPlatformDisplayEXT
#define eglDebugMessageControlKHR l_eglDebugMessageControlKHR
#define eglLabelObjectKHR l_eglLabelObjectKHR
#define eglQueryDebugKHR l_eglQueryDebugKHR
#define eglClientWaitSyncKHR l_eglClientWaitSyncKHR
#define eglCreateSyncKHR l_eglCreateSyncKHR
#define eglDestroySyncKHR l_eglDestroySyncKHR
#define eglGetSyncAttribKHR l_eglGetSyncAttribKHR
#define eglCreateImageKHR l_eglCreateImageKHR
#define eglDestroyImageKHR l_eglDestroyImageKHR
#define eglLockSurfaceKHR l_eglLockSurfaceKHR
#define eglQuerySurface64KHR l_eglQuerySurface64KHR
#define eglUnlockSurfaceKHR l_eglUnlockSurfaceKHR
#define eglSetDamageRegionKHR l_eglSetDamageRegionKHR
#define eglSignalSyncKHR l_eglSignalSyncKHR
#define eglCreateStreamKHR l_eglCreateStreamKHR
#define eglDestroyStreamKHR l_eglDestroyStreamKHR
#define eglQueryStreamKHR l_eglQueryStreamKHR
#define eglQueryStreamu64KHR l_eglQueryStreamu64KHR
#define eglStreamAttribKHR l_eglStreamAttribKHR
#define eglStreamConsumerAcquireKHR l_eglStreamConsumerAcquireKHR
#define eglStreamConsumerGLTextureExternalKHR l_eglStreamConsumerGLTextureExternalKHR
#define eglStreamConsumerReleaseKHR l_eglStreamConsumerReleaseKHR
#define eglSwapBuffersWithDamageKHR l_eglSwapBuffersWithDamageKHR
#define eglWaitSyncKHR l_eglWaitSyncKHR
#define eglPostSubBufferNV l_eglPostSubBufferNV
#define eglStreamConsumerGLTextureExternalAttribsNV l_eglStreamConsumerGLTextureExternalAttribsNV

#if defined(__cplusplus)
extern "C" {
#endif  // defined(__cplusplus)
ANGLE_UTIL_EXPORT extern PFNEGLCHOOSECONFIGPROC l_eglChooseConfig;
ANGLE_UTIL_EXPORT extern PFNEGLCOPYBUFFERSPROC l_eglCopyBuffers;
ANGLE_UTIL_EXPORT extern PFNEGLCREATECONTEXTPROC l_eglCreateContext;
ANGLE_UTIL_EXPORT extern PFNEGLCREATEPBUFFERSURFACEPROC l_eglCreatePbufferSurface;
ANGLE_UTIL_EXPORT extern PFNEGLCREATEPIXMAPSURFACEPROC l_eglCreatePixmapSurface;
ANGLE_UTIL_EXPORT extern PFNEGLCREATEWINDOWSURFACEPROC l_eglCreateWindowSurface;
ANGLE_UTIL_EXPORT extern PFNEGLDESTROYCONTEXTPROC l_eglDestroyContext;
ANGLE_UTIL_EXPORT extern PFNEGLDESTROYSURFACEPROC l_eglDestroySurface;
ANGLE_UTIL_EXPORT extern PFNEGLGETCONFIGATTRIBPROC l_eglGetConfigAttrib;
ANGLE_UTIL_EXPORT extern PFNEGLGETCONFIGSPROC l_eglGetConfigs;
ANGLE_UTIL_EXPORT extern PFNEGLGETCURRENTDISPLAYPROC l_eglGetCurrentDisplay;
ANGLE_UTIL_EXPORT extern PFNEGLGETCURRENTSURFACEPROC l_eglGetCurrentSurface;
ANGLE_UTIL_EXPORT extern PFNEGLGETDISPLAYPROC l_eglGetDisplay;
ANGLE_UTIL_EXPORT extern PFNEGLGETERRORPROC l_eglGetError;
ANGLE_UTIL_EXPORT extern PFNEGLGETPROCADDRESSPROC l_eglGetProcAddress;
ANGLE_UTIL_EXPORT extern PFNEGLINITIALIZEPROC l_eglInitialize;
ANGLE_UTIL_EXPORT extern PFNEGLMAKECURRENTPROC l_eglMakeCurrent;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYCONTEXTPROC l_eglQueryContext;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYSTRINGPROC l_eglQueryString;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYSURFACEPROC l_eglQuerySurface;
ANGLE_UTIL_EXPORT extern PFNEGLSWAPBUFFERSPROC l_eglSwapBuffers;
ANGLE_UTIL_EXPORT extern PFNEGLTERMINATEPROC l_eglTerminate;
ANGLE_UTIL_EXPORT extern PFNEGLWAITGLPROC l_eglWaitGL;
ANGLE_UTIL_EXPORT extern PFNEGLWAITNATIVEPROC l_eglWaitNative;
ANGLE_UTIL_EXPORT extern PFNEGLBINDTEXIMAGEPROC l_eglBindTexImage;
ANGLE_UTIL_EXPORT extern PFNEGLRELEASETEXIMAGEPROC l_eglReleaseTexImage;
ANGLE_UTIL_EXPORT extern PFNEGLSURFACEATTRIBPROC l_eglSurfaceAttrib;
ANGLE_UTIL_EXPORT extern PFNEGLSWAPINTERVALPROC l_eglSwapInterval;
ANGLE_UTIL_EXPORT extern PFNEGLBINDAPIPROC l_eglBindAPI;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYAPIPROC l_eglQueryAPI;
ANGLE_UTIL_EXPORT extern PFNEGLCREATEPBUFFERFROMCLIENTBUFFERPROC l_eglCreatePbufferFromClientBuffer;
ANGLE_UTIL_EXPORT extern PFNEGLRELEASETHREADPROC l_eglReleaseThread;
ANGLE_UTIL_EXPORT extern PFNEGLWAITCLIENTPROC l_eglWaitClient;
ANGLE_UTIL_EXPORT extern PFNEGLGETCURRENTCONTEXTPROC l_eglGetCurrentContext;
ANGLE_UTIL_EXPORT extern PFNEGLCREATESYNCPROC l_eglCreateSync;
ANGLE_UTIL_EXPORT extern PFNEGLDESTROYSYNCPROC l_eglDestroySync;
ANGLE_UTIL_EXPORT extern PFNEGLCLIENTWAITSYNCPROC l_eglClientWaitSync;
ANGLE_UTIL_EXPORT extern PFNEGLGETSYNCATTRIBPROC l_eglGetSyncAttrib;
ANGLE_UTIL_EXPORT extern PFNEGLCREATEIMAGEPROC l_eglCreateImage;
ANGLE_UTIL_EXPORT extern PFNEGLDESTROYIMAGEPROC l_eglDestroyImage;
ANGLE_UTIL_EXPORT extern PFNEGLGETPLATFORMDISPLAYPROC l_eglGetPlatformDisplay;
ANGLE_UTIL_EXPORT extern PFNEGLCREATEPLATFORMWINDOWSURFACEPROC l_eglCreatePlatformWindowSurface;
ANGLE_UTIL_EXPORT extern PFNEGLCREATEPLATFORMPIXMAPSURFACEPROC l_eglCreatePlatformPixmapSurface;
ANGLE_UTIL_EXPORT extern PFNEGLWAITSYNCPROC l_eglWaitSync;
ANGLE_UTIL_EXPORT extern PFNEGLSETBLOBCACHEFUNCSANDROIDPROC l_eglSetBlobCacheFuncsANDROID;
ANGLE_UTIL_EXPORT extern PFNEGLCREATENATIVECLIENTBUFFERANDROIDPROC
    l_eglCreateNativeClientBufferANDROID;
ANGLE_UTIL_EXPORT extern PFNEGLGETCOMPOSITORTIMINGANDROIDPROC l_eglGetCompositorTimingANDROID;
ANGLE_UTIL_EXPORT extern PFNEGLGETCOMPOSITORTIMINGSUPPORTEDANDROIDPROC
    l_eglGetCompositorTimingSupportedANDROID;
ANGLE_UTIL_EXPORT extern PFNEGLGETFRAMETIMESTAMPSUPPORTEDANDROIDPROC
    l_eglGetFrameTimestampSupportedANDROID;
ANGLE_UTIL_EXPORT extern PFNEGLGETFRAMETIMESTAMPSANDROIDPROC l_eglGetFrameTimestampsANDROID;
ANGLE_UTIL_EXPORT extern PFNEGLGETNEXTFRAMEIDANDROIDPROC l_eglGetNextFrameIdANDROID;
ANGLE_UTIL_EXPORT extern PFNEGLGETNATIVECLIENTBUFFERANDROIDPROC l_eglGetNativeClientBufferANDROID;
ANGLE_UTIL_EXPORT extern PFNEGLDUPNATIVEFENCEFDANDROIDPROC l_eglDupNativeFenceFDANDROID;
ANGLE_UTIL_EXPORT extern PFNEGLPRESENTATIONTIMEANDROIDPROC l_eglPresentationTimeANDROID;
ANGLE_UTIL_EXPORT extern PFNEGLCREATEDEVICEANGLEPROC l_eglCreateDeviceANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLRELEASEDEVICEANGLEPROC l_eglReleaseDeviceANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLACQUIREEXTERNALCONTEXTANGLEPROC l_eglAcquireExternalContextANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLRELEASEEXTERNALCONTEXTANGLEPROC l_eglReleaseExternalContextANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYDISPLAYATTRIBANGLEPROC l_eglQueryDisplayAttribANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYSTRINGIANGLEPROC l_eglQueryStringiANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLCOPYMETALSHAREDEVENTANGLEPROC l_eglCopyMetalSharedEventANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLSETVALIDATIONENABLEDANGLEPROC l_eglSetValidationEnabledANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLFORCEGPUSWITCHANGLEPROC l_eglForceGPUSwitchANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLHANDLEGPUSWITCHANGLEPROC l_eglHandleGPUSwitchANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLREACQUIREHIGHPOWERGPUANGLEPROC l_eglReacquireHighPowerGPUANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLRELEASEHIGHPOWERGPUANGLEPROC l_eglReleaseHighPowerGPUANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLPREPARESWAPBUFFERSANGLEPROC l_eglPrepareSwapBuffersANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLPROGRAMCACHEGETATTRIBANGLEPROC l_eglProgramCacheGetAttribANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLPROGRAMCACHEPOPULATEANGLEPROC l_eglProgramCachePopulateANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLPROGRAMCACHEQUERYANGLEPROC l_eglProgramCacheQueryANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLPROGRAMCACHERESIZEANGLEPROC l_eglProgramCacheResizeANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYSURFACEPOINTERANGLEPROC l_eglQuerySurfacePointerANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLCREATESTREAMPRODUCERD3DTEXTUREANGLEPROC
    l_eglCreateStreamProducerD3DTextureANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLSTREAMPOSTD3DTEXTUREANGLEPROC l_eglStreamPostD3DTextureANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLSWAPBUFFERSWITHFRAMETOKENANGLEPROC
    l_eglSwapBuffersWithFrameTokenANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLGETMSCRATEANGLEPROC l_eglGetMscRateANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLEXPORTVKIMAGEANGLEPROC l_eglExportVkImageANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLWAITUNTILWORKSCHEDULEDANGLEPROC l_eglWaitUntilWorkScheduledANGLE;
ANGLE_UTIL_EXPORT extern PFNEGLGETSYNCVALUESCHROMIUMPROC l_eglGetSyncValuesCHROMIUM;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYDEVICEATTRIBEXTPROC l_eglQueryDeviceAttribEXT;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYDEVICESTRINGEXTPROC l_eglQueryDeviceStringEXT;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYDISPLAYATTRIBEXTPROC l_eglQueryDisplayAttribEXT;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYDMABUFFORMATSEXTPROC l_eglQueryDmaBufFormatsEXT;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYDMABUFMODIFIERSEXTPROC l_eglQueryDmaBufModifiersEXT;
ANGLE_UTIL_EXPORT extern PFNEGLCREATEPLATFORMPIXMAPSURFACEEXTPROC
    l_eglCreatePlatformPixmapSurfaceEXT;
ANGLE_UTIL_EXPORT extern PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC
    l_eglCreatePlatformWindowSurfaceEXT;
ANGLE_UTIL_EXPORT extern PFNEGLGETPLATFORMDISPLAYEXTPROC l_eglGetPlatformDisplayEXT;
ANGLE_UTIL_EXPORT extern PFNEGLDEBUGMESSAGECONTROLKHRPROC l_eglDebugMessageControlKHR;
ANGLE_UTIL_EXPORT extern PFNEGLLABELOBJECTKHRPROC l_eglLabelObjectKHR;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYDEBUGKHRPROC l_eglQueryDebugKHR;
ANGLE_UTIL_EXPORT extern PFNEGLCLIENTWAITSYNCKHRPROC l_eglClientWaitSyncKHR;
ANGLE_UTIL_EXPORT extern PFNEGLCREATESYNCKHRPROC l_eglCreateSyncKHR;
ANGLE_UTIL_EXPORT extern PFNEGLDESTROYSYNCKHRPROC l_eglDestroySyncKHR;
ANGLE_UTIL_EXPORT extern PFNEGLGETSYNCATTRIBKHRPROC l_eglGetSyncAttribKHR;
ANGLE_UTIL_EXPORT extern PFNEGLCREATEIMAGEKHRPROC l_eglCreateImageKHR;
ANGLE_UTIL_EXPORT extern PFNEGLDESTROYIMAGEKHRPROC l_eglDestroyImageKHR;
ANGLE_UTIL_EXPORT extern PFNEGLLOCKSURFACEKHRPROC l_eglLockSurfaceKHR;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYSURFACE64KHRPROC l_eglQuerySurface64KHR;
ANGLE_UTIL_EXPORT extern PFNEGLUNLOCKSURFACEKHRPROC l_eglUnlockSurfaceKHR;
ANGLE_UTIL_EXPORT extern PFNEGLSETDAMAGEREGIONKHRPROC l_eglSetDamageRegionKHR;
ANGLE_UTIL_EXPORT extern PFNEGLSIGNALSYNCKHRPROC l_eglSignalSyncKHR;
ANGLE_UTIL_EXPORT extern PFNEGLCREATESTREAMKHRPROC l_eglCreateStreamKHR;
ANGLE_UTIL_EXPORT extern PFNEGLDESTROYSTREAMKHRPROC l_eglDestroyStreamKHR;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYSTREAMKHRPROC l_eglQueryStreamKHR;
ANGLE_UTIL_EXPORT extern PFNEGLQUERYSTREAMU64KHRPROC l_eglQueryStreamu64KHR;
ANGLE_UTIL_EXPORT extern PFNEGLSTREAMATTRIBKHRPROC l_eglStreamAttribKHR;
ANGLE_UTIL_EXPORT extern PFNEGLSTREAMCONSUMERACQUIREKHRPROC l_eglStreamConsumerAcquireKHR;
ANGLE_UTIL_EXPORT extern PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALKHRPROC
    l_eglStreamConsumerGLTextureExternalKHR;
ANGLE_UTIL_EXPORT extern PFNEGLSTREAMCONSUMERRELEASEKHRPROC l_eglStreamConsumerReleaseKHR;
ANGLE_UTIL_EXPORT extern PFNEGLSWAPBUFFERSWITHDAMAGEKHRPROC l_eglSwapBuffersWithDamageKHR;
ANGLE_UTIL_EXPORT extern PFNEGLWAITSYNCKHRPROC l_eglWaitSyncKHR;
ANGLE_UTIL_EXPORT extern PFNEGLPOSTSUBBUFFERNVPROC l_eglPostSubBufferNV;
ANGLE_UTIL_EXPORT extern PFNEGLSTREAMCONSUMERGLTEXTUREEXTERNALATTRIBSNVPROC
    l_eglStreamConsumerGLTextureExternalAttribsNV;

typedef void (*GenericProc)(void);
typedef GenericProc(KHRONOS_APIENTRY *LoadProc)(const char *);
ANGLE_UTIL_EXPORT void LoadUtilEGL(LoadProc loadProc);

#if defined(__cplusplus)
}       // extern "C"
#endif  // defined(__cplusplus)

#endif  // UTIL_EGL_LOADER_AUTOGEN_H_
