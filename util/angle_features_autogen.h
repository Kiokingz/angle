// GENERATED FILE - DO NOT EDIT.
// Generated by gen_features.py using data from *_features.json.
//
// Copyright 2022 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// angle_features_autogen.h: List of ANGLE features to help enable/disable them in tests.

#ifndef ANGLE_SRC_TESTS_TEST_UTILS_ANGLE_FEATURES_AUTOGEN_H_
#define ANGLE_SRC_TESTS_TEST_UTILS_ANGLE_FEATURES_AUTOGEN_H_

#include "util_export.h"

namespace angle
{
enum class Feature
{
    AddAndTrueToLoopCondition,
    AddMockTextureNoRenderTarget,
    AdjustSrcDstRegionForBlitFramebuffer,
    AllocateNonZeroMemory,
    AllowAstcFormats,
    AllowBufferReadWrite,
    AllowClearForRobustResourceInit,
    AllowCompressedFormats,
    AllowES3OnFL100,
    AllowETCFormats,
    AllowGenerateMipmapWithCompute,
    AllowGenMultipleMipsPerPass,
    AllowInlineConstVertexData,
    AllowMultisampleStoreAndResolve,
    AllowRuntimeSamplerCompareMode,
    AllowSamplerCompareGradient,
    AllowSamplerCompareLod,
    AllowSeparateDepthStencilBuffers,
    AllowTranslateUniformBlockToStructuredBuffer,
    AlwaysCallUseProgramAfterLink,
    AlwaysUnbindFramebufferTexture2D,
    AsyncCommandQueue,
    Avoid1BitAlphaTextureFormats,
    BasicGLLineRasterization,
    BindEmptyForUnusedDescriptorSets,
    BindTransformFeedbackBufferBeforeBindBufferRange,
    BottomLeftOriginPresentRegionRectangles,
    BresenhamLineRasterization,
    CallClearTwice,
    ClampArrayAccess,
    ClampFragDepth,
    ClampMscRate,
    ClampPointSize,
    ClearToZeroOrOneBroken,
    ClipSrcRegionForBlitFramebuffer,
    CompressVertexData,
    CopyIOSurfaceToNonIOSurfaceForReadOptimization,
    CopyTextureToBufferForReadOptimization,
    CreatePipelineDuringLink,
    DecodeEncodeSRGBForGenerateMipmap,
    DeferFlushUntilEndRenderPass,
    DepthClamping,
    DepthStencilBlitExtraCopy,
    DirectMetalGeneration,
    DisableAnisotropicFiltering,
    DisableB5G6R5Support,
    DisableBlendFuncExtended,
    DisableDrawBuffersIndexed,
    DisableFifoPresentMode,
    DisableFlippingBlitWithCommand,
    DisableGPUSwitchingSupport,
    DisableMultisampledRenderToTexture,
    DisableNativeParallelCompile,
    DisableProgramBinary,
    DisableProgramCachingForTransformFeedback,
    DisableSemaphoreFd,
    DisableSyncControlSupport,
    DisableTimestampQueries,
    DisableWorkerContexts,
    DisallowSeamfulCubeMapEmulation,
    DoesSRGBClearsOnLinearFramebufferAttachments,
    DontInitializeUninitializedLocals,
    DontRelinkProgramsInParallel,
    DontUseLoopsToInitializeVariables,
    DoWhileGLSLCausesGPUHang,
    EmulateAbsIntFunction,
    EmulateAdvancedBlendEquations,
    EmulateAtan2Float,
    EmulateCopyTexImage2DFromRenderbuffers,
    EmulateDithering,
    EmulatedPrerotation180,
    EmulatedPrerotation270,
    EmulatedPrerotation90,
    EmulateImmutableCompressedTexture3D,
    EmulateIsnanFloat,
    EmulateMaxVertexAttribStride,
    EmulatePackSkipRowsAndPackSkipPixels,
    EmulatePrimitiveRestartFixedIndex,
    EmulateR32fImageAtomicExchange,
    EmulateRGB10,
    EmulateTinyStencilTextures,
    EmulateTransformFeedback,
    EnableCaptureLimits,
    EnableCompressingPipelineCacheInThreadPool,
    EnableMultisampledRenderToTexture,
    EnablePrecisionQualifiers,
    EnablePreRotateSurfaces,
    EnableProgramBinaryForCapture,
    ExpandIntegerPowExpressions,
    ExplicitlyEnablePerSampleShading,
    ExposeNonConformantExtensionsAndVersions,
    FinishDoesNotCauseQueriesToBeAvailable,
    FlushAfterEndingTransformFeedback,
    FlushBeforeDeleteTextureIfCopiedTo,
    FlushOnFramebufferChange,
    ForceAtomicValueResolution,
    ForceBufferGPUStorage,
    ForceD16TexFilter,
    ForceD24S8AsUnsupported,
    ForceDepthAttachmentInitOnClear,
    ForceDriverUniformOverSpecConst,
    ForceFallbackFormat,
    ForceFragmentShaderPrecisionHighpToMediump,
    ForceInitShaderVariables,
    ForceMaxUniformBufferSize16KB,
    ForceNearestFiltering,
    ForceNearestMipFiltering,
    ForceNonCSBaseMipmapGeneration,
    ForceRobustResourceInit,
    ForceSubmitImmutableTextureUpdates,
    ForceTextureLodOffset1,
    ForceTextureLodOffset2,
    ForceTextureLodOffset3,
    ForceTextureLodOffset4,
    GenerateSPIRVThroughGlslang,
    GetDimensionsIgnoresBaseLevel,
    HasBaseVertexInstancedDraw,
    HasCheapRenderPass,
    HasDepthAutoResolve,
    HasEvents,
    HasExplicitMemBarrier,
    HasNonUniformDispatch,
    HasShaderStencilOutput,
    HasStencilAutoResolve,
    HasTextureSwizzle,
    InitFragmentOutputVariables,
    InitializeCurrentVertexAttributes,
    IntelDisableFastMath,
    IntelExplicitBoolCastWorkaround,
    KeepBufferShadowCopy,
    LimitMax3dArrayTextureSizeTo1024,
    LimitMaxMSAASamplesTo4,
    LogMemoryReportCallbacks,
    LogMemoryReportStats,
    LoseContextOnOutOfMemory,
    MrtPerfWorkaround,
    MultisampleColorFormatShaderReadWorkaround,
    OverrideSurfaceFormatRGB8ToRGBA8,
    PackLastRowSeparatelyForPaddingInclusion,
    PackOverlappingRowsSeparatelyPackBuffer,
    PadBuffersToMaxVertexAttribStride,
    PerFrameWindowSizeQuery,
    PersistentlyMappedBuffers,
    PreAddTexelFetchOffsets,
    PreferAggregateBarrierCalls,
    PreferCPUForBufferSubData,
    PreferDeviceLocalMemoryHostVisible,
    PreferDrawClearOverVkCmdClearAttachments,
    PreferSkippingInvalidateForEmulatedFormats,
    PreferSubmitAtFBOBoundary,
    PromotePackedFormatsTo8BitPerChannel,
    ProvokingVertex,
    QueryCounterBitsGeneratesErrors,
    ReadPixelsUsingImplementationColorReadFormatForNorm16,
    ReapplyUBOBindingsAfterUsingBinaryProgram,
    RegenerateStructNames,
    RemoveDynamicIndexingOfSwizzledVector,
    RemoveInvariantAndCentroidForESSL3,
    ResetTexImage2DBaseLevel,
    RetainSPIRVDebugInfo,
    RewriteFloatUnaryMinusOperator,
    RewriteRepeatedAssignToSwizzled,
    RewriteRowMajorMatrices,
    RewriteUnaryMinusOperator,
    RGBA4IsNotSupportedForColorRendering,
    RGBDXT1TexturesSampleZeroAlpha,
    RoundOutputAfterDithering,
    SanitizeAMDGPURendererString,
    ScalarizeVecAndMatConstructorArgs,
    SelectViewInGeometryShader,
    SetDataFasterThanImageUpload,
    SetPrimitiveRestartFixedIndexForDrawArrays,
    SetZeroLevelBeforeGenerateMipmap,
    ShadowBuffers,
    ShiftInstancedArrayDataWithOffset,
    SkipVSConstantRegisterZero,
    SupportsAndroidHardwareBuffer,
    SupportsAndroidNativeFenceSync,
    SupportsBlendOperationAdvanced,
    SupportsCustomBorderColor,
    SupportsDepthClipControl,
    SupportsDepthStencilResolve,
    SupportsExternalFenceCapabilities,
    SupportsExternalFenceFd,
    SupportsExternalMemoryDmaBufAndModifiers,
    SupportsExternalMemoryFd,
    SupportsExternalMemoryFuchsia,
    SupportsExternalMemoryHost,
    SupportsExternalSemaphoreCapabilities,
    SupportsExternalSemaphoreFd,
    SupportsExternalSemaphoreFuchsia,
    SupportsFilteringPrecision,
    SupportsFragmentShadingRate,
    SupportsGeometryStreamsCapability,
    SupportsGGPFrameToken,
    SupportsHostQueryReset,
    SupportsImageCubeArray,
    SupportsImageFormatList,
    SupportsIncrementalPresent,
    SupportsIndexTypeUint8,
    SupportsLockSurfaceExtension,
    SupportsMultiDrawIndirect,
    SupportsMultisampledRenderToSingleSampled,
    SupportsMultiview,
    SupportsNegativeViewport,
    SupportsPipelineStatisticsQuery,
    SupportsProtectedMemory,
    SupportsRenderpass2,
    SupportsRenderPassLoadStoreOpNone,
    SupportsRenderPassStoreOpNone,
    SupportsShaderFloat16,
    SupportsShaderFramebufferFetch,
    SupportsShaderFramebufferFetchNonCoherent,
    SupportsShaderStencilExport,
    SupportsSharedPresentableImageExtension,
    SupportsSurfaceCapabilities2Extension,
    SupportsSurfacelessQueryExtension,
    SupportsSurfaceProtectedCapabilitiesExtension,
    SupportsSurfaceProtectedSwapchains,
    SupportsTransformFeedbackExtension,
    SupportsYUVSamplerConversion,
    SwapbuffersOnFlushOrFinishWithSingleBuffer,
    SyncVertexArraysToDefault,
    UnbindFBOBeforeSwitchingContext,
    UnfoldShortCircuits,
    UnpackLastRowSeparatelyForPaddingInclusion,
    UnpackOverlappingRowsSeparatelyUnpackBuffer,
    UnsizedSRGBReadPixelsDoesntTransform,
    UploadTextureDataInChunks,
    UseInstancedPointSpriteEmulation,
    UseMultipleDescriptorsForExternalFormats,
    UseSystemMemoryForConstantBuffers,
    UseUnusedBlocksWithStandardOrSharedLayout,
    VertexIDDoesNotIncludeBaseVertex,
    WaitIdleBeforeSwapchainRecreation,
    ZeroMaxLodWorkaround,

    InvalidEnum,
    EnumCount = InvalidEnum,
};

ANGLE_UTIL_EXPORT extern const char *GetFeatureName(Feature feature);

}  // namespace angle

#endif  // ANGLE_SRC_TESTS_TEST_UTILS_ANGLE_FEATURES_AUTOGEN_H_
