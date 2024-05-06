//
// Copyright 2018 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// vk_helpers:
//   Helper utility classes that manage Vulkan resources.

#ifndef LIBANGLE_RENDERER_VULKAN_VK_HELPERS_H_
#define LIBANGLE_RENDERER_VULKAN_VK_HELPERS_H_

#include "common/MemoryBuffer.h"
#include "common/SimpleMutex.h"
#include "libANGLE/renderer/vulkan/MemoryTracking.h"
#include "libANGLE/renderer/vulkan/Suballocation.h"
#include "libANGLE/renderer/vulkan/vk_cache_utils.h"
#include "libANGLE/renderer/vulkan/vk_format_utils.h"
#include "libANGLE/renderer/vulkan/vk_ref_counted_event.h"

#include <functional>

namespace gl
{
class ImageIndex;
}  // namespace gl

namespace rx
{
namespace vk
{
constexpr VkBufferUsageFlags kVertexBufferUsageFlags =
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
constexpr VkBufferUsageFlags kIndexBufferUsageFlags =
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
constexpr VkBufferUsageFlags kIndirectBufferUsageFlags =
    VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
constexpr size_t kVertexBufferAlignment   = 4;
constexpr size_t kIndexBufferAlignment    = 4;
constexpr size_t kIndirectBufferAlignment = 4;

constexpr VkBufferUsageFlags kStagingBufferFlags =
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
constexpr size_t kStagingBufferSize = 1024 * 16;

constexpr VkImageCreateFlags kVkImageCreateFlagsNone = 0;

// Most likely initial chroma filter mode given GL_TEXTURE_EXTERNAL_OES default
// min & mag filters are linear.
constexpr VkFilter kDefaultYCbCrChromaFilter = VK_FILTER_LINEAR;

constexpr VkPipelineStageFlags kSwapchainAcquireImageWaitStageFlags =
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |          // First use is a blit command.
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |  // First use is a draw command.
    VK_PIPELINE_STAGE_TRANSFER_BIT;                  // First use is a clear without scissor.

// For each level, write  layers that don't conflict in parallel.  The layer is hashed to
// `layer % kMaxParallelLayerWrites` and used to track whether that subresource is currently
// being written.  If so, a barrier is inserted; otherwise, the barrier is avoided.  If the updated
// layer count is greater than kMaxParallelLayerWrites, there will be a few unnecessary
// barriers.
constexpr uint32_t kMaxParallelLayerWrites = 64;
using ImageLayerWriteMask                  = std::bitset<kMaxParallelLayerWrites>;

using StagingBufferOffsetArray = std::array<VkDeviceSize, 2>;

// Imagine an image going through a few layout transitions:
//
//           srcStage 1    dstStage 2          srcStage 2     dstStage 3
//  Layout 1 ------Transition 1-----> Layout 2 ------Transition 2------> Layout 3
//           srcAccess 1  dstAccess 2          srcAccess 2   dstAccess 3
//   \_________________  ___________________/
//                     \/
//               A transition
//
// Every transition requires 6 pieces of information: from/to layouts, src/dst stage masks and
// src/dst access masks.  At the moment we decide to transition the image to Layout 2 (i.e.
// Transition 1), we need to have Layout 1, srcStage 1 and srcAccess 1 stored as history of the
// image.  To perform the transition, we need to know Layout 2, dstStage 2 and dstAccess 2.
// Additionally, we need to know srcStage 2 and srcAccess 2 to retain them for the next transition.
//
// That is, with the history kept, on every new transition we need 5 pieces of new information:
// layout/dstStage/dstAccess to transition into the layout, and srcStage/srcAccess for the future
// transition out from it.  Given the small number of possible combinations of these values, an
// enum is used were each value encapsulates these 5 pieces of information:
//
//                       +--------------------------------+
//           srcStage 1  | dstStage 2          srcStage 2 |   dstStage 3
//  Layout 1 ------Transition 1-----> Layout 2 ------Transition 2------> Layout 3
//           srcAccess 1 |dstAccess 2          srcAccess 2|  dstAccess 3
//                       +---------------  ---------------+
//                                       \/
//                                 One enum value
//
// Note that, while generally dstStage for the to-transition and srcStage for the from-transition
// are the same, they may occasionally be BOTTOM_OF_PIPE and TOP_OF_PIPE respectively.
enum class ImageLayout
{
    Undefined = 0,
    // Framebuffer attachment layouts are placed first, so they can fit in fewer bits in
    // PackedAttachmentOpsDesc.

    // Color (Write):
    ColorWrite,
    MSRTTEmulationColorUnresolveAndResolve,

    // Depth (Write), Stencil (Write)
    DepthWriteStencilWrite,

    // Depth (Write), Stencil (Read)
    DepthWriteStencilRead,
    DepthWriteStencilReadFragmentShaderStencilRead,
    DepthWriteStencilReadAllShadersStencilRead,

    // Depth (Read), Stencil (Write)
    DepthReadStencilWrite,
    DepthReadStencilWriteFragmentShaderDepthRead,
    DepthReadStencilWriteAllShadersDepthRead,

    // Depth (Read), Stencil (Read)
    DepthReadStencilRead,
    DepthReadStencilReadFragmentShaderRead,
    DepthReadStencilReadAllShadersRead,

    // The GENERAL layout is used when there's a feedback loop.  For depth/stencil it does't matter
    // which aspect is participating in feedback and whether the other aspect is read-only.
    ColorWriteFragmentShaderFeedback,
    ColorWriteAllShadersFeedback,
    DepthStencilFragmentShaderFeedback,
    DepthStencilAllShadersFeedback,

    // Depth/stencil resolve is special because it uses the _color_ output stage and mask
    DepthStencilResolve,
    MSRTTEmulationDepthStencilUnresolveAndResolve,

    Present,
    SharedPresent,
    // The rest of the layouts.
    ExternalPreInitialized,
    ExternalShadersReadOnly,
    ExternalShadersWrite,
    TransferSrc,
    TransferDst,
    TransferSrcDst,
    // Used when the image is transitioned on the host for use by host image copy
    HostCopy,
    VertexShaderReadOnly,
    VertexShaderWrite,
    // PreFragment == Vertex, Tessellation and Geometry stages
    PreFragmentShadersReadOnly,
    PreFragmentShadersWrite,
    FragmentShadingRateAttachmentReadOnly,
    FragmentShaderReadOnly,
    FragmentShaderWrite,
    ComputeShaderReadOnly,
    ComputeShaderWrite,
    AllGraphicsShadersReadOnly,
    AllGraphicsShadersWrite,
    TransferDstAndComputeWrite,

    InvalidEnum,
    EnumCount = InvalidEnum,
};

VkImageCreateFlags GetImageCreateFlags(gl::TextureType textureType);

ImageLayout GetImageLayoutFromGLImageLayout(Context *context, GLenum layout);

GLenum ConvertImageLayoutToGLImageLayout(ImageLayout imageLayout);

VkImageLayout ConvertImageLayoutToVkImageLayout(Context *context, ImageLayout imageLayout);

struct ImageLayoutEventMaps
{
    // The list of RefCountedEvents that have been tracked. The mask is used to accelerate the
    // loop of map
    angle::PackedEnumMap<ImageLayout, RefCountedEvent> map;
    angle::PackedEnumBitSet<ImageLayout, uint64_t> mask;
};

// A dynamic buffer is conceptually an infinitely long buffer. Each time you write to the buffer,
// you will always write to a previously unused portion. After a series of writes, you must flush
// the buffer data to the device. Buffer lifetime currently assumes that each new allocation will
// last as long or longer than each prior allocation.
//
// Dynamic buffers are used to implement a variety of data streaming operations in Vulkan, such
// as for immediate vertex array and element array data, uniform updates, and other dynamic data.
//
// Internally dynamic buffers keep a collection of VkBuffers. When we write past the end of a
// currently active VkBuffer we keep it until it is no longer in use. We then mark it available
// for future allocations in a free list.
class BufferHelper;
using BufferHelperQueue = std::deque<std::unique_ptr<BufferHelper>>;

class DynamicBuffer : angle::NonCopyable
{
  public:
    DynamicBuffer();
    DynamicBuffer(DynamicBuffer &&other);
    ~DynamicBuffer();

    void init(Renderer *renderer,
              VkBufferUsageFlags usage,
              size_t alignment,
              size_t initialSize,
              bool hostVisible);

    // This call will allocate a new region at the end of the current buffer. If it can't find
    // enough space in the current buffer, it returns false. This gives caller a chance to deal with
    // buffer switch that may occur with allocate call.
    bool allocateFromCurrentBuffer(size_t sizeInBytes, BufferHelper **bufferHelperOut);

    // This call will allocate a new region at the end of the buffer with default alignment. It
    // internally may trigger a new buffer to be created (which is returned in the optional
    // parameter `newBufferAllocatedOut`). The new region will be in the returned buffer at given
    // offset.
    angle::Result allocate(Context *context,
                           size_t sizeInBytes,
                           BufferHelper **bufferHelperOut,
                           bool *newBufferAllocatedOut);

    // This releases resources when they might currently be in use.
    void release(Renderer *renderer);

    // This adds in-flight buffers to the mResourceUseList in the share group and then releases
    // them.
    void updateQueueSerialAndReleaseInFlightBuffers(ContextVk *contextVk,
                                                    const QueueSerial &queueSerial);

    // This frees resources immediately.
    void destroy(Renderer *renderer);

    BufferHelper *getCurrentBuffer() const { return mBuffer.get(); }

    // **Accumulate** an alignment requirement.  A dynamic buffer is used as the staging buffer for
    // image uploads, which can contain updates to unrelated mips, possibly with different formats.
    // The staging buffer should have an alignment that can satisfy all those formats, i.e. it's the
    // lcm of all alignments set in its lifetime.
    void requireAlignment(Renderer *renderer, size_t alignment);
    size_t getAlignment() const { return mAlignment; }

    // For testing only!
    void setMinimumSizeForTesting(size_t minSize);

    bool isCoherent() const
    {
        return (mMemoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
    }

    bool valid() const { return mSize != 0; }

  private:
    void reset();
    angle::Result allocateNewBuffer(Context *context);

    VkBufferUsageFlags mUsage;
    bool mHostVisible;
    size_t mInitialSize;
    std::unique_ptr<BufferHelper> mBuffer;
    uint32_t mNextAllocationOffset;
    size_t mSize;
    size_t mSizeInRecentHistory;
    size_t mAlignment;
    VkMemoryPropertyFlags mMemoryPropertyFlags;

    BufferHelperQueue mInFlightBuffers;
    BufferHelperQueue mBufferFreeList;
};

// Class DescriptorSetHelper. This is a wrapper of VkDescriptorSet with GPU resource use tracking.
class DescriptorSetHelper final : public Resource
{
  public:
    DescriptorSetHelper(const VkDescriptorSet &descriptorSet) { mDescriptorSet = descriptorSet; }
    DescriptorSetHelper(const ResourceUse &use, const VkDescriptorSet &descriptorSet)
    {
        mUse           = use;
        mDescriptorSet = descriptorSet;
    }
    DescriptorSetHelper(DescriptorSetHelper &&other) : Resource(std::move(other))
    {
        mDescriptorSet       = other.mDescriptorSet;
        other.mDescriptorSet = VK_NULL_HANDLE;
    }

    VkDescriptorSet getDescriptorSet() const { return mDescriptorSet; }

  private:
    VkDescriptorSet mDescriptorSet;
};
using DescriptorSetList = std::deque<DescriptorSetHelper>;

// Uses DescriptorPool to allocate descriptor sets as needed. If a descriptor pool becomes full, we
// allocate new pools internally as needed. Renderer takes care of the lifetime of the discarded
// pools. Note that we used a fixed layout for descriptor pools in ANGLE.

// Shared handle to a descriptor pool. Each helper is allocated from the dynamic descriptor pool.
// Can be used to share descriptor pools between multiple ProgramVks and the ContextVk.
class CommandBufferHelperCommon;

class DescriptorPoolHelper final : public Resource
{
  public:
    DescriptorPoolHelper();
    ~DescriptorPoolHelper() override;

    bool valid() { return mDescriptorPool.valid(); }

    angle::Result init(Context *context,
                       const std::vector<VkDescriptorPoolSize> &poolSizesIn,
                       uint32_t maxSets);
    void destroy(Renderer *renderer);
    void release(Renderer *renderer);

    bool allocateDescriptorSet(Context *context,
                               const DescriptorSetLayout &descriptorSetLayout,
                               VkDescriptorSet *descriptorSetsOut);

    void addGarbage(DescriptorSetHelper &&garbage)
    {
        mValidDescriptorSets--;
        mDescriptorSetGarbageList.emplace_back(std::move(garbage));
    }

    void onNewDescriptorSetAllocated(const vk::SharedDescriptorSetCacheKey &sharedCacheKey)
    {
        mDescriptorSetCacheManager.addKey(sharedCacheKey);
    }
    bool hasValidDescriptorSet() const { return mValidDescriptorSets != 0; }

  private:
    // Track the number of descriptorSets allocated out of this pool that are valid. DescriptorSets
    // that have been allocated but in the mDescriptorSetGarbageList is considered as inactive.
    uint32_t mValidDescriptorSets;
    // Track the number of remaining descriptorSets in the pool that can be allocated.
    uint32_t mFreeDescriptorSets;
    DescriptorPool mDescriptorPool;
    // Keeps track descriptorSets that has been released. Because freeing descriptorSet require
    // DescriptorPool, we store individually released descriptor sets here instead of usual garbage
    // list in the renderer to avoid complicated threading issues and other weirdness associated
    // with pooled object destruction. This list is mutually exclusive with mDescriptorSetCache.
    DescriptorSetList mDescriptorSetGarbageList;
    // Manages the texture descriptor set cache that allocated from this pool
    vk::DescriptorSetCacheManager mDescriptorSetCacheManager;
};

using RefCountedDescriptorPoolBinding = BindingPointer<DescriptorPoolHelper>;

class DynamicDescriptorPool final : angle::NonCopyable
{
  public:
    DynamicDescriptorPool();
    ~DynamicDescriptorPool();

    DynamicDescriptorPool(DynamicDescriptorPool &&other);
    DynamicDescriptorPool &operator=(DynamicDescriptorPool &&other);

    // The DynamicDescriptorPool only handles one pool size at this time.
    // Note that setSizes[i].descriptorCount is expected to be the number of descriptors in
    // an individual set.  The pool size will be calculated accordingly.
    angle::Result init(Context *context,
                       const VkDescriptorPoolSize *setSizes,
                       size_t setSizeCount,
                       const DescriptorSetLayout &descriptorSetLayout);
    void destroy(Renderer *renderer);

    bool valid() const { return !mDescriptorPools.empty(); }

    // We use the descriptor type to help count the number of free sets.
    // By convention, sets are indexed according to the constants in vk_cache_utils.h.
    angle::Result allocateDescriptorSet(Context *context,
                                        const DescriptorSetLayout &descriptorSetLayout,
                                        RefCountedDescriptorPoolBinding *bindingOut,
                                        VkDescriptorSet *descriptorSetOut);

    angle::Result getOrAllocateDescriptorSet(Context *context,
                                             CommandBufferHelperCommon *commandBufferHelper,
                                             const DescriptorSetDesc &desc,
                                             const DescriptorSetLayout &descriptorSetLayout,
                                             RefCountedDescriptorPoolBinding *bindingOut,
                                             VkDescriptorSet *descriptorSetOut,
                                             SharedDescriptorSetCacheKey *sharedCacheKeyOut);

    void releaseCachedDescriptorSet(Renderer *renderer, const DescriptorSetDesc &desc);
    void destroyCachedDescriptorSet(Renderer *renderer, const DescriptorSetDesc &desc);

    template <typename Accumulator>
    void accumulateDescriptorCacheStats(VulkanCacheType cacheType, Accumulator *accum) const
    {
        accum->accumulateCacheStats(cacheType, mCacheStats);
    }
    void resetDescriptorCacheStats() { mCacheStats.resetHitAndMissCount(); }
    size_t getTotalCacheKeySizeBytes() const
    {
        return mDescriptorSetCache.getTotalCacheKeySizeBytes();
    }

    // Release the pool if it is no longer been used and contains no valid descriptorSet.
    void checkAndReleaseUnusedPool(Renderer *renderer, RefCountedDescriptorPoolHelper *pool);

    // For testing only!
    static uint32_t GetMaxSetsPerPoolForTesting();
    static void SetMaxSetsPerPoolForTesting(uint32_t maxSetsPerPool);
    static uint32_t GetMaxSetsPerPoolMultiplierForTesting();
    static void SetMaxSetsPerPoolMultiplierForTesting(uint32_t maxSetsPerPool);

  private:
    angle::Result allocateNewPool(Context *context);

    static constexpr uint32_t kMaxSetsPerPoolMax = 512;
    static uint32_t mMaxSetsPerPool;
    static uint32_t mMaxSetsPerPoolMultiplier;
    size_t mCurrentPoolIndex;
    std::vector<std::unique_ptr<RefCountedDescriptorPoolHelper>> mDescriptorPools;
    std::vector<VkDescriptorPoolSize> mPoolSizes;
    // This cached handle is used for verifying the layout being used to allocate descriptor sets
    // from the pool matches the layout that the pool was created for, to ensure that the free
    // descriptor count is accurate and new pools are created appropriately.
    VkDescriptorSetLayout mCachedDescriptorSetLayout;
    // Tracks cache for descriptorSet. Note that cached DescriptorSet can be reuse even if it is GPU
    // busy.
    DescriptorSetCache mDescriptorSetCache;
    // Statistics for the cache.
    CacheStats mCacheStats;
};

using RefCountedDescriptorPool = RefCounted<DynamicDescriptorPool>;
using DescriptorPoolPointer    = BindingPointer<DynamicDescriptorPool>;

// Maps from a descriptor set layout (represented by DescriptorSetLayoutDesc) to a set of
// DynamicDescriptorPools. The purpose of the class is so multiple GL Programs can share descriptor
// set caches. We need to stratify the sets by the descriptor set layout to ensure compatibility.
class MetaDescriptorPool final : angle::NonCopyable
{
  public:
    MetaDescriptorPool();
    ~MetaDescriptorPool();

    void destroy(Renderer *renderer);

    angle::Result bindCachedDescriptorPool(Context *context,
                                           const DescriptorSetLayoutDesc &descriptorSetLayoutDesc,
                                           uint32_t descriptorCountMultiplier,
                                           DescriptorSetLayoutCache *descriptorSetLayoutCache,
                                           DescriptorPoolPointer *descriptorPoolOut);

    template <typename Accumulator>
    void accumulateDescriptorCacheStats(VulkanCacheType cacheType, Accumulator *accum) const
    {
        for (const auto &iter : mPayload)
        {
            const vk::RefCountedDescriptorPool &pool = iter.second;
            pool.get().accumulateDescriptorCacheStats(cacheType, accum);
        }
    }

    void resetDescriptorCacheStats()
    {
        for (auto &iter : mPayload)
        {
            vk::RefCountedDescriptorPool &pool = iter.second;
            pool.get().resetDescriptorCacheStats();
        }
    }

    size_t getTotalCacheKeySizeBytes() const
    {
        size_t totalSize = 0;

        for (const auto &iter : mPayload)
        {
            const RefCountedDescriptorPool &pool = iter.second;
            totalSize += pool.get().getTotalCacheKeySizeBytes();
        }

        return totalSize;
    }

  private:
    std::unordered_map<DescriptorSetLayoutDesc, RefCountedDescriptorPool> mPayload;
};

template <typename Pool>
class DynamicallyGrowingPool : angle::NonCopyable
{
  public:
    DynamicallyGrowingPool();
    virtual ~DynamicallyGrowingPool();

    bool isValid() { return mPoolSize > 0; }

  protected:
    angle::Result initEntryPool(Context *contextVk, uint32_t poolSize);

    virtual void destroyPoolImpl(VkDevice device, Pool &poolToDestroy) = 0;
    void destroyEntryPool(VkDevice device);

    // Checks to see if any pool is already free, in which case it sets it as current pool and
    // returns true.
    bool findFreeEntryPool(ContextVk *contextVk);

    // Allocates a new entry and initializes it with the given pool.
    angle::Result allocateNewEntryPool(ContextVk *contextVk, Pool &&pool);

    // Called by the implementation whenever an entry is freed.
    void onEntryFreed(ContextVk *contextVk, size_t poolIndex, const ResourceUse &use);

    const Pool &getPool(size_t index) const
    {
        return const_cast<DynamicallyGrowingPool *>(this)->getPool(index);
    }

    Pool &getPool(size_t index)
    {
        ASSERT(index < mPools.size());
        return mPools[index].pool;
    }

    uint32_t getPoolSize() const { return mPoolSize; }

    virtual angle::Result allocatePoolImpl(ContextVk *contextVk,
                                           Pool &poolToAllocate,
                                           uint32_t entriesToAllocate) = 0;
    angle::Result allocatePoolEntries(ContextVk *contextVk,
                                      uint32_t entryCount,
                                      uint32_t *poolIndexOut,
                                      uint32_t *currentEntryOut);

  private:
    // The pool size, to know when a pool is completely freed.
    uint32_t mPoolSize;

    struct PoolResource : public Resource
    {
        PoolResource(Pool &&poolIn, uint32_t freedCountIn);
        PoolResource(PoolResource &&other);

        Pool pool;

        // A count corresponding to each pool indicating how many of its allocated entries
        // have been freed. Once that value reaches mPoolSize for each pool, that pool is considered
        // free and reusable.  While keeping a bitset would allow allocation of each index, the
        // slight runtime overhead of finding free indices is not worth the slight memory overhead
        // of creating new pools when unnecessary.
        uint32_t freedCount;
    };
    std::vector<PoolResource> mPools;

    // Index into mPools indicating pool we are currently allocating from.
    size_t mCurrentPool;
    // Index inside mPools[mCurrentPool] indicating which index can be allocated next.
    uint32_t mCurrentFreeEntry;
};

// DynamicQueryPool allocates indices out of QueryPool as needed.  Once a QueryPool is exhausted,
// another is created.  The query pools live permanently, but are recycled as indices get freed.

// These are arbitrary default sizes for query pools.
constexpr uint32_t kDefaultOcclusionQueryPoolSize           = 64;
constexpr uint32_t kDefaultTimestampQueryPoolSize           = 64;
constexpr uint32_t kDefaultTransformFeedbackQueryPoolSize   = 128;
constexpr uint32_t kDefaultPrimitivesGeneratedQueryPoolSize = 128;

class QueryHelper;

class DynamicQueryPool final : public DynamicallyGrowingPool<QueryPool>
{
  public:
    DynamicQueryPool();
    ~DynamicQueryPool() override;

    angle::Result init(ContextVk *contextVk, VkQueryType type, uint32_t poolSize);
    void destroy(VkDevice device);

    angle::Result allocateQuery(ContextVk *contextVk, QueryHelper *queryOut, uint32_t queryCount);
    void freeQuery(ContextVk *contextVk, QueryHelper *query);

    const QueryPool &getQueryPool(size_t index) const { return getPool(index); }

  private:
    angle::Result allocatePoolImpl(ContextVk *contextVk,
                                   QueryPool &poolToAllocate,
                                   uint32_t entriesToAllocate) override;
    void destroyPoolImpl(VkDevice device, QueryPool &poolToDestroy) override;

    // Information required to create new query pools
    VkQueryType mQueryType;
};

// Stores the result of a Vulkan query call. XFB queries in particular store two result values.
class QueryResult final
{
  public:
    QueryResult(uint32_t intsPerResult) : mIntsPerResult(intsPerResult), mResults{} {}

    void operator+=(const QueryResult &rhs)
    {
        mResults[0] += rhs.mResults[0];
        mResults[1] += rhs.mResults[1];
    }

    size_t getDataSize() const { return mIntsPerResult * sizeof(uint64_t); }
    void setResults(uint64_t *results, uint32_t queryCount);
    uint64_t getResult(size_t index) const
    {
        ASSERT(index < mIntsPerResult);
        return mResults[index];
    }

    static constexpr size_t kDefaultResultIndex                      = 0;
    static constexpr size_t kTransformFeedbackPrimitivesWrittenIndex = 0;
    static constexpr size_t kPrimitivesGeneratedIndex                = 1;

  private:
    uint32_t mIntsPerResult;
    std::array<uint64_t, 2> mResults;
};

// Queries in Vulkan are identified by the query pool and an index for a query within that pool.
// Unlike other pools, such as descriptor pools where an allocation returns an independent object
// from the pool, the query allocations are not done through a Vulkan function and are only an
// integer index.
//
// Furthermore, to support arbitrarily large number of queries, DynamicQueryPool creates query pools
// of a fixed size as needed and allocates indices within those pools.
//
// The QueryHelper class below keeps the pool and index pair together.  For multiview, multiple
// consecutive query indices are implicitly written to by the driver, so the query count is
// additionally kept.
class QueryHelper final : public Resource
{
  public:
    QueryHelper();
    ~QueryHelper() override;
    QueryHelper(QueryHelper &&rhs);
    QueryHelper &operator=(QueryHelper &&rhs);
    void init(const DynamicQueryPool *dynamicQueryPool,
              const size_t queryPoolIndex,
              uint32_t query,
              uint32_t queryCount);
    void deinit();

    bool valid() const { return mDynamicQueryPool != nullptr; }

    // Begin/end queries.  These functions break the render pass.
    angle::Result beginQuery(ContextVk *contextVk);
    angle::Result endQuery(ContextVk *contextVk);
    // Begin/end queries within a started render pass.
    angle::Result beginRenderPassQuery(ContextVk *contextVk);
    void endRenderPassQuery(ContextVk *contextVk);

    angle::Result flushAndWriteTimestamp(ContextVk *contextVk);
    // When syncing gpu/cpu time, main thread accesses primary directly
    void writeTimestampToPrimary(ContextVk *contextVk, PrimaryCommandBuffer *primary);
    // All other timestamp accesses should be made on outsideRenderPassCommandBuffer
    void writeTimestamp(ContextVk *contextVk,
                        OutsideRenderPassCommandBuffer *outsideRenderPassCommandBuffer);

    // Whether this query helper has generated and submitted any commands.
    bool hasSubmittedCommands() const;

    angle::Result getUint64ResultNonBlocking(ContextVk *contextVk,
                                             QueryResult *resultOut,
                                             bool *availableOut);
    angle::Result getUint64Result(ContextVk *contextVk, QueryResult *resultOut);

  private:
    friend class DynamicQueryPool;
    const QueryPool &getQueryPool() const
    {
        ASSERT(valid());
        return mDynamicQueryPool->getQueryPool(mQueryPoolIndex);
    }

    // Reset needs to always be done outside a render pass, which may be different from the
    // passed-in command buffer (which could be the render pass').
    template <typename CommandBufferT>
    void beginQueryImpl(ContextVk *contextVk,
                        OutsideRenderPassCommandBuffer *resetCommandBuffer,
                        CommandBufferT *commandBuffer);
    template <typename CommandBufferT>
    void endQueryImpl(ContextVk *contextVk, CommandBufferT *commandBuffer);
    template <typename CommandBufferT>
    void resetQueryPoolImpl(ContextVk *contextVk,
                            const QueryPool &queryPool,
                            CommandBufferT *commandBuffer);
    VkResult getResultImpl(ContextVk *contextVk,
                           const VkQueryResultFlags flags,
                           QueryResult *resultOut);

    const DynamicQueryPool *mDynamicQueryPool;
    size_t mQueryPoolIndex;
    uint32_t mQuery;
    uint32_t mQueryCount;

    enum class QueryStatus
    {
        Inactive,
        Active,
        Ended
    };
    QueryStatus mStatus;
};

// Semaphores that are allocated from the semaphore pool are encapsulated in a helper object,
// keeping track of where in the pool they are allocated from.
class SemaphoreHelper final : angle::NonCopyable
{
  public:
    SemaphoreHelper();
    ~SemaphoreHelper();

    SemaphoreHelper(SemaphoreHelper &&other);
    SemaphoreHelper &operator=(SemaphoreHelper &&other);

    void init(const size_t semaphorePoolIndex, const Semaphore *semaphore);
    void deinit();

    const Semaphore *getSemaphore() const { return mSemaphore; }

    // Used only by DynamicSemaphorePool.
    size_t getSemaphorePoolIndex() const { return mSemaphorePoolIndex; }

  private:
    size_t mSemaphorePoolIndex;
    const Semaphore *mSemaphore;
};

// This defines enum for VkPipelineStageFlagBits so that we can use it to compare and index into
// array.
enum class PipelineStage : uint32_t
{
    // Bellow are ordered based on Graphics Pipeline Stages
    TopOfPipe              = 0,
    DrawIndirect           = 1,
    VertexInput            = 2,
    VertexShader           = 3,
    TessellationControl    = 4,
    TessellationEvaluation = 5,
    GeometryShader         = 6,
    TransformFeedback      = 7,
    FragmentShadingRate    = 8,
    EarlyFragmentTest      = 9,
    FragmentShader         = 10,
    LateFragmentTest       = 11,
    ColorAttachmentOutput  = 12,

    // Compute specific pipeline Stage
    ComputeShader = 13,

    // Transfer specific pipeline Stage
    Transfer     = 14,
    BottomOfPipe = 15,

    // Host specific pipeline stage
    Host = 16,

    InvalidEnum = 17,
    EnumCount   = InvalidEnum,
};
using PipelineStagesMask = angle::PackedEnumBitSet<PipelineStage, uint32_t>;

PipelineStage GetPipelineStage(gl::ShaderType stage);

// This wraps data and API for vkCmdPipelineBarrier call
class PipelineBarrier : angle::NonCopyable
{
  public:
    PipelineBarrier()
        : mSrcStageMask(0),
          mDstStageMask(0),
          mMemoryBarrierSrcAccess(0),
          mMemoryBarrierDstAccess(0),
          mImageMemoryBarriers()
    {}
    ~PipelineBarrier() { ASSERT(mImageMemoryBarriers.empty()); }

    bool isEmpty() const { return mImageMemoryBarriers.empty() && mMemoryBarrierDstAccess == 0; }

    void execute(PrimaryCommandBuffer *primary)
    {
        if (isEmpty())
        {
            return;
        }

        // Issue vkCmdPipelineBarrier call
        VkMemoryBarrier memoryBarrier = {};
        uint32_t memoryBarrierCount   = 0;
        if (mMemoryBarrierDstAccess != 0)
        {
            memoryBarrier.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
            memoryBarrier.srcAccessMask = mMemoryBarrierSrcAccess;
            memoryBarrier.dstAccessMask = mMemoryBarrierDstAccess;
            memoryBarrierCount++;
        }
        primary->pipelineBarrier(
            mSrcStageMask, mDstStageMask, 0, memoryBarrierCount, &memoryBarrier, 0, nullptr,
            static_cast<uint32_t>(mImageMemoryBarriers.size()), mImageMemoryBarriers.data());

        reset();
    }

    // merge two barriers into one
    void merge(PipelineBarrier *other)
    {
        mSrcStageMask |= other->mSrcStageMask;
        mDstStageMask |= other->mDstStageMask;
        mMemoryBarrierSrcAccess |= other->mMemoryBarrierSrcAccess;
        mMemoryBarrierDstAccess |= other->mMemoryBarrierDstAccess;
        mImageMemoryBarriers.insert(mImageMemoryBarriers.end(), other->mImageMemoryBarriers.begin(),
                                    other->mImageMemoryBarriers.end());
        other->reset();
    }

    void mergeMemoryBarrier(VkPipelineStageFlags srcStageMask,
                            VkPipelineStageFlags dstStageMask,
                            VkAccessFlags srcAccess,
                            VkAccessFlags dstAccess)
    {
        mSrcStageMask |= srcStageMask;
        mDstStageMask |= dstStageMask;
        mMemoryBarrierSrcAccess |= srcAccess;
        mMemoryBarrierDstAccess |= dstAccess;
    }

    void mergeImageBarrier(VkPipelineStageFlags srcStageMask,
                           VkPipelineStageFlags dstStageMask,
                           const VkImageMemoryBarrier &imageMemoryBarrier)
    {
        ASSERT(imageMemoryBarrier.pNext == nullptr);
        mSrcStageMask |= srcStageMask;
        mDstStageMask |= dstStageMask;
        mImageMemoryBarriers.push_back(imageMemoryBarrier);
    }

    void reset()
    {
        mSrcStageMask           = 0;
        mDstStageMask           = 0;
        mMemoryBarrierSrcAccess = 0;
        mMemoryBarrierDstAccess = 0;
        mImageMemoryBarriers.clear();
    }

    void addDiagnosticsString(std::ostringstream &out) const;

  private:
    VkPipelineStageFlags mSrcStageMask;
    VkPipelineStageFlags mDstStageMask;
    VkAccessFlags mMemoryBarrierSrcAccess;
    VkAccessFlags mMemoryBarrierDstAccess;
    std::vector<VkImageMemoryBarrier> mImageMemoryBarriers;
};

class PipelineBarrierArray final
{
  public:
    void mergeMemoryBarrier(PipelineStage stageIndex,
                            VkPipelineStageFlags srcStageMask,
                            VkPipelineStageFlags dstStageMask,
                            VkAccessFlags srcAccess,
                            VkAccessFlags dstAccess)
    {
        mBarriers[stageIndex].mergeMemoryBarrier(srcStageMask, dstStageMask, srcAccess, dstAccess);
        mBarrierMask.set(stageIndex);
    }

    void mergeImageBarrier(PipelineStage stageIndex,
                           VkPipelineStageFlags srcStageMask,
                           VkPipelineStageFlags dstStageMask,
                           const VkImageMemoryBarrier &imageMemoryBarrier)
    {
        mBarriers[stageIndex].mergeImageBarrier(srcStageMask, dstStageMask, imageMemoryBarrier);
        mBarrierMask.set(stageIndex);
    }

    void execute(Renderer *renderer, PrimaryCommandBuffer *primary);

    void addDiagnosticsString(std::ostringstream &out) const;

  private:
    angle::PackedEnumMap<PipelineStage, PipelineBarrier> mBarriers;
    PipelineStagesMask mBarrierMask;
};

enum class MemoryCoherency : uint8_t
{
    CachedNonCoherent,
    CachedPreferCoherent,
    UnCachedCoherent,

    InvalidEnum = 3,
    EnumCount   = 3,
};
ANGLE_INLINE bool IsCached(MemoryCoherency coherency)
{
    return coherency == MemoryCoherency::CachedNonCoherent ||
           coherency == MemoryCoherency::CachedPreferCoherent;
}

enum class MemoryHostVisibility
{
    NonVisible,
    Visible
};

class BufferPool;

class BufferHelper : public ReadWriteResource
{
  public:
    BufferHelper();
    ~BufferHelper() override;

    BufferHelper(BufferHelper &&other);
    BufferHelper &operator=(BufferHelper &&other);

    angle::Result init(Context *context,
                       const VkBufferCreateInfo &createInfo,
                       VkMemoryPropertyFlags memoryPropertyFlags);
    angle::Result initExternal(Context *context,
                               VkMemoryPropertyFlags memoryProperties,
                               const VkBufferCreateInfo &requestedCreateInfo,
                               GLeglClientBufferEXT clientBuffer);
    VkResult initSuballocation(Context *context,
                               uint32_t memoryTypeIndex,
                               size_t size,
                               size_t alignment,
                               BufferUsageType usageType,
                               BufferPool *pool);

    void destroy(Renderer *renderer);
    void release(Renderer *renderer);
    void releaseBufferAndDescriptorSetCache(Renderer *renderer);

    BufferSerial getBufferSerial() const { return mSerial; }
    BufferSerial getBlockSerial() const
    {
        ASSERT(mSuballocation.valid());
        return mSuballocation.getBlockSerial();
    }
    BufferBlock *getBufferBlock() const { return mSuballocation.getBufferBlock(); }
    bool valid() const { return mSuballocation.valid(); }
    const Buffer &getBuffer() const { return mSuballocation.getBuffer(); }
    VkDeviceSize getOffset() const { return mSuballocation.getOffset(); }
    VkDeviceSize getSize() const { return mSuballocation.getSize(); }
    VkMemoryMapFlags getMemoryPropertyFlags() const
    {
        return mSuballocation.getMemoryPropertyFlags();
    }
    uint8_t *getMappedMemory() const
    {
        ASSERT(isMapped());
        return mSuballocation.getMappedMemory();
    }
    // Returns the main buffer block's pointer.
    uint8_t *getBlockMemory() const { return mSuballocation.getBlockMemory(); }
    VkDeviceSize getBlockMemorySize() const { return mSuballocation.getBlockMemorySize(); }
    bool isHostVisible() const { return mSuballocation.isHostVisible(); }
    bool isCoherent() const { return mSuballocation.isCoherent(); }
    bool isCached() const { return mSuballocation.isCached(); }
    bool isMapped() const { return mSuballocation.isMapped(); }

    angle::Result map(Context *context, uint8_t **ptrOut);
    angle::Result mapWithOffset(Context *context, uint8_t **ptrOut, size_t offset);
    void unmap(Renderer *renderer) {}
    // After a sequence of writes, call flush to ensure the data is visible to the device.
    angle::Result flush(Renderer *renderer);
    angle::Result flush(Renderer *renderer, VkDeviceSize offset, VkDeviceSize size);
    // After a sequence of writes, call invalidate to ensure the data is visible to the host.
    angle::Result invalidate(Renderer *renderer);
    angle::Result invalidate(Renderer *renderer, VkDeviceSize offset, VkDeviceSize size);

    void changeQueue(uint32_t newQueueFamilyIndex, OutsideRenderPassCommandBuffer *commandBuffer);

    // Performs an ownership transfer from an external instance or API.
    void acquireFromExternal(uint32_t externalQueueFamilyIndex,
                             uint32_t rendererQueueFamilyIndex,
                             OutsideRenderPassCommandBuffer *commandBuffer);

    // Performs an ownership transfer to an external instance or API.
    void releaseToExternal(uint32_t rendererQueueFamilyIndex,
                           uint32_t externalQueueFamilyIndex,
                           OutsideRenderPassCommandBuffer *commandBuffer);

    // Returns true if the image is owned by an external API or instance.
    bool isReleasedToExternal() const;

    void recordReadBarrier(VkAccessFlags readAccessType,
                           VkPipelineStageFlags readStage,
                           PipelineStage stageIndex,
                           PipelineBarrierArray *barriers);

    void recordWriteBarrier(VkAccessFlags writeAccessType,
                            VkPipelineStageFlags writeStage,
                            PipelineStage stageIndex,
                            PipelineBarrierArray *barriers);

    void fillWithColor(const angle::Color<uint8_t> &color,
                       const gl::InternalFormat &internalFormat);

    // Special handling for VertexArray code so that we can create a dedicated VkBuffer for the
    // sub-range of memory of the actual buffer data size that user requested (i.e, excluding extra
    // paddings that we added for alignment, which will not get zero filled).
    const Buffer &getBufferForVertexArray(ContextVk *contextVk,
                                          VkDeviceSize actualDataSize,
                                          VkDeviceSize *offsetOut);

    void onNewDescriptorSet(const SharedDescriptorSetCacheKey &sharedCacheKey)
    {
        mDescriptorSetCacheManager.addKey(sharedCacheKey);
    }

    angle::Result initializeNonZeroMemory(Context *context,
                                          VkBufferUsageFlags usage,
                                          VkDeviceSize size);

    // Buffer's user size and allocation size may be different due to alignment requirement. In
    // normal usage we just use the actual allocation size and it is good enough. But when
    // robustResourceInit is enabled, mBufferWithUserSize is created to match the exact user
    // size. Thus when user size changes, we must clear and recreate this mBufferWithUserSize.
    // Returns true if mBufferWithUserSize is released.
    bool onBufferUserSizeChange(Renderer *renderer);

    void initializeBarrierTracker(Context *context);

  private:
    // Only called by DynamicBuffer.
    friend class DynamicBuffer;
    void setSuballocationOffsetAndSize(VkDeviceSize offset, VkDeviceSize size)
    {
        mSuballocation.setOffsetAndSize(offset, size);
    }

    // Suballocation object.
    BufferSuballocation mSuballocation;
    // This normally is invalid. We always use the BufferBlock's buffer and offset combination. But
    // when robust resource init is enabled, we may want to create a dedicated VkBuffer for the
    // suballocation so that vulkan driver will ensure no access beyond this sub-range. In that
    // case, this VkBuffer will be created lazily as needed.
    Buffer mBufferWithUserSize;

    // For memory barriers.
    uint32_t mCurrentQueueFamilyIndex;
    VkFlags mCurrentWriteAccess;
    VkFlags mCurrentReadAccess;
    VkPipelineStageFlags mCurrentWriteStages;
    VkPipelineStageFlags mCurrentReadStages;

    BufferSerial mSerial;
    // Manages the descriptorSet cache that created with this BufferHelper object.
    DescriptorSetCacheManager mDescriptorSetCacheManager;
    // For external buffer
    GLeglClientBufferEXT mClientBuffer;

    // Whether ANGLE currently has ownership of this resource or it's released to external.
    bool mIsReleasedToExternal;
};

class BufferPool : angle::NonCopyable
{
  public:
    BufferPool();
    BufferPool(BufferPool &&other);
    ~BufferPool();

    // Init that gives the ability to pass in specified memory property flags for the buffer.
    void initWithFlags(Renderer *renderer,
                       vma::VirtualBlockCreateFlags flags,
                       VkBufferUsageFlags usage,
                       VkDeviceSize initialSize,
                       uint32_t memoryTypeIndex,
                       VkMemoryPropertyFlags memoryProperty);

    VkResult allocateBuffer(Context *context,
                            VkDeviceSize sizeInBytes,
                            VkDeviceSize alignment,
                            BufferSuballocation *suballocation);

    // Frees resources immediately, or orphan the non-empty BufferBlocks if allowed. If orphan is
    // not allowed, it will assert if BufferBlock is still not empty.
    void destroy(Renderer *renderer, bool orphanAllowed);
    // Remove and destroy empty BufferBlocks
    void pruneEmptyBuffers(Renderer *renderer);

    bool valid() const { return mSize != 0; }

    void addStats(std::ostringstream *out) const;
    size_t getBufferCount() const { return mBufferBlocks.size() + mEmptyBufferBlocks.size(); }
    VkDeviceSize getMemorySize() const { return mTotalMemorySize; }

  private:
    VkResult allocateNewBuffer(Context *context, VkDeviceSize sizeInBytes);
    VkDeviceSize getTotalEmptyMemorySize() const;

    vma::VirtualBlockCreateFlags mVirtualBlockCreateFlags;
    VkBufferUsageFlags mUsage;
    bool mHostVisible;
    VkDeviceSize mSize;
    uint32_t mMemoryTypeIndex;
    VkDeviceSize mTotalMemorySize;
    BufferBlockPointerVector mBufferBlocks;
    BufferBlockPointerVector mEmptyBufferBlocks;
    // Tracks the number of new buffers needed for suballocation since last pruneEmptyBuffers call.
    // We will use this heuristic information to decide how many empty buffers to keep around.
    size_t mNumberOfNewBuffersNeededSinceLastPrune;
    // max size to go down the suballocation code path. Any allocation greater or equal this size
    // will call into vulkan directly to allocate a dedicated VkDeviceMemory.
    static constexpr size_t kMaxBufferSizeForSuballocation = 4 * 1024 * 1024;
};
using BufferPoolPointerArray = std::array<std::unique_ptr<BufferPool>, VK_MAX_MEMORY_TYPES>;

// Stores clear value In packed attachment index
class PackedClearValuesArray final
{
  public:
    PackedClearValuesArray();
    ~PackedClearValuesArray();

    PackedClearValuesArray(const PackedClearValuesArray &other);
    PackedClearValuesArray &operator=(const PackedClearValuesArray &rhs);
    void store(PackedAttachmentIndex index,
               VkImageAspectFlags aspectFlags,
               const VkClearValue &clearValue);
    void storeNoDepthStencil(PackedAttachmentIndex index, const VkClearValue &clearValue);
    const VkClearValue &operator[](PackedAttachmentIndex index) const
    {
        return mValues[index.get()];
    }
    const VkClearValue *data() const { return mValues.data(); }

  private:
    gl::AttachmentArray<VkClearValue> mValues;
};

class ImageHelper;
using ImageHelperPtr = ImageHelper *;

// Reference to a render pass attachment (color or depth/stencil) alongside render-pass-related
// tracking such as when the attachment is last written to or invalidated.  This is used to
// determine loadOp and storeOp of the attachment, and enables optimizations that need to know
// how the attachment has been used.
class RenderPassAttachment final
{
  public:
    RenderPassAttachment();
    ~RenderPassAttachment() = default;

    void init(ImageHelper *image,
              UniqueSerial imageSiblingSerial,
              gl::LevelIndex levelIndex,
              uint32_t layerIndex,
              uint32_t layerCount,
              VkImageAspectFlagBits aspect);
    void reset();

    void onAccess(ResourceAccess access, uint32_t currentCmdCount);
    void invalidate(const gl::Rectangle &invalidateArea,
                    bool isAttachmentEnabled,
                    uint32_t currentCmdCount);
    void onRenderAreaGrowth(ContextVk *contextVk, const gl::Rectangle &newRenderArea);
    void finalizeLoadStore(Context *context,
                           uint32_t currentCmdCount,
                           bool hasUnresolveAttachment,
                           bool hasResolveAttachment,
                           RenderPassLoadOp *loadOp,
                           RenderPassStoreOp *storeOp,
                           bool *isInvalidatedOut);
    void restoreContent();
    bool hasAnyAccess() const { return mAccess != ResourceAccess::Unused; }
    bool hasWriteAccess() const { return HasResourceWriteAccess(mAccess); }

    ImageHelper *getImage() { return mImage; }

    bool hasImage(const ImageHelper *image, UniqueSerial imageSiblingSerial) const
    {
        // Compare values because we do want that invalid serials compare equal.
        return mImage == image && mImageSiblingSerial.getValue() == imageSiblingSerial.getValue();
    }

  private:
    bool hasWriteAfterInvalidate(uint32_t currentCmdCount) const;
    bool isInvalidated(uint32_t currentCmdCount) const;
    bool onAccessImpl(ResourceAccess access, uint32_t currentCmdCount);

    // The attachment image itself
    ImageHelper *mImage;
    // Invalid or serial of EGLImage/Surface sibling target.
    UniqueSerial mImageSiblingSerial;
    // The subresource used in the render pass
    gl::LevelIndex mLevelIndex;
    uint32_t mLayerIndex;
    uint32_t mLayerCount;
    VkImageAspectFlagBits mAspect;
    // Tracks the highest access during the entire render pass (Write being the highest), excluding
    // clear through loadOp.  This allows loadOp=Clear to be optimized out when we find out that the
    // attachment is not used in the render pass at all and storeOp=DontCare, or that a
    // mid-render-pass clear could be hoisted to loadOp=Clear.
    ResourceAccess mAccess;
    // The index of the last draw command after which the attachment is invalidated
    uint32_t mInvalidatedCmdCount;
    // The index of the last draw command after which the attachment output is disabled
    uint32_t mDisabledCmdCount;
    // The area that has been invalidated
    gl::Rectangle mInvalidateArea;
};

// Stores RenderPassAttachment In packed attachment index
class PackedRenderPassAttachmentArray final
{
  public:
    PackedRenderPassAttachmentArray() : mAttachments{} {}
    ~PackedRenderPassAttachmentArray() = default;
    RenderPassAttachment &operator[](PackedAttachmentIndex index)
    {
        return mAttachments[index.get()];
    }
    void reset()
    {
        for (RenderPassAttachment &attachment : mAttachments)
        {
            attachment.reset();
        }
    }

  private:
    gl::AttachmentArray<RenderPassAttachment> mAttachments;
};

class SecondaryCommandBufferCollector final
{
  public:
    SecondaryCommandBufferCollector()                                              = default;
    SecondaryCommandBufferCollector(const SecondaryCommandBufferCollector &)       = delete;
    SecondaryCommandBufferCollector(SecondaryCommandBufferCollector &&)            = default;
    void operator=(const SecondaryCommandBufferCollector &)                        = delete;
    SecondaryCommandBufferCollector &operator=(SecondaryCommandBufferCollector &&) = default;
    ~SecondaryCommandBufferCollector() { ASSERT(empty()); }

    void collectCommandBuffer(priv::SecondaryCommandBuffer &&commandBuffer);
    void collectCommandBuffer(VulkanSecondaryCommandBuffer &&commandBuffer);
    void retireCommandBuffers();

    bool empty() const { return mCollectedCommandBuffers.empty(); }

  private:
    std::vector<VulkanSecondaryCommandBuffer> mCollectedCommandBuffers;
};

struct CommandsState
{
    std::vector<VkSemaphore> waitSemaphores;
    std::vector<VkPipelineStageFlags> waitSemaphoreStageMasks;
    PrimaryCommandBuffer primaryCommands;
    SecondaryCommandBufferCollector secondaryCommands;
};

// How the ImageHelper object is being used by the renderpass
enum class RenderPassUsage
{
    // Attached to the render taget of the current renderpass commands. It could be read/write or
    // read only access.
    RenderTargetAttachment,
    // This is special case of RenderTargetAttachment where the render target access is read only.
    // Right now it is only tracked for depth stencil attachment
    DepthReadOnlyAttachment,
    StencilReadOnlyAttachment,
    // This is special case of RenderTargetAttachment where the render target access is formed
    // feedback loop. Right now it is only tracked for depth stencil attachment
    DepthFeedbackLoop,
    StencilFeedbackLoop,
    // Attached to the texture sampler of the current renderpass commands
    ColorTextureSampler,
    DepthTextureSampler,
    StencilTextureSampler,
    // Fragment shading rate attachment
    FragmentShadingRateReadOnlyAttachment,

    InvalidEnum,
    EnumCount = InvalidEnum,
};
using RenderPassUsageFlags = angle::PackedEnumBitSet<RenderPassUsage, uint16_t>;
constexpr RenderPassUsageFlags kDepthStencilReadOnlyBits = RenderPassUsageFlags(
    {RenderPassUsage::DepthReadOnlyAttachment, RenderPassUsage::StencilReadOnlyAttachment});
constexpr RenderPassUsageFlags kDepthStencilFeedbackModeBits = RenderPassUsageFlags(
    {RenderPassUsage::DepthFeedbackLoop, RenderPassUsage::StencilFeedbackLoop});

// The following are used to help track the state of an invalidated attachment.
// This value indicates an "infinite" CmdCount that is not valid for comparing
constexpr uint32_t kInfiniteCmdCount = 0xFFFFFFFF;

// CommandBufferHelperCommon and derivatives OutsideRenderPassCommandBufferHelper and
// RenderPassCommandBufferHelper wrap the outside/inside render pass secondary command buffers,
// together with other information such as barriers to issue before the command buffer, tracking of
// resource usages, etc.  When the asyncCommandQueue feature is enabled, objects of these classes
// are handed off to the worker thread to be executed on the primary command buffer.
class CommandBufferHelperCommon : angle::NonCopyable
{
  public:
    void bufferWrite(ContextVk *contextVk,
                     VkAccessFlags writeAccessType,
                     PipelineStage writeStage,
                     BufferHelper *buffer);

    bool usesBuffer(const BufferHelper &buffer) const
    {
        return buffer.usedByCommandBuffer(mQueueSerial);
    }

    bool usesBufferForWrite(const BufferHelper &buffer) const
    {
        return buffer.writtenByCommandBuffer(mQueueSerial);
    }

    void executeBarriers(Renderer *renderer, CommandsState *commandsState);

    // The markOpen and markClosed functions are to aid in proper use of the *CommandBufferHelper.
    // saw invalid use due to threading issues that can be easily caught by marking when it's safe
    // (open) to write to the command buffer.
#if !defined(ANGLE_ENABLE_ASSERTS)
    void markOpen() {}
    void markClosed() {}
#endif

    void setHasShaderStorageOutput() { mHasShaderStorageOutput = true; }
    bool hasShaderStorageOutput() const { return mHasShaderStorageOutput; }

    bool hasGLMemoryBarrierIssued() const { return mHasGLMemoryBarrierIssued; }

    void retainResource(Resource *resource) { resource->setQueueSerial(mQueueSerial); }

    void retainResourceForWrite(ReadWriteResource *writeResource)
    {
        writeResource->setWriteQueueSerial(mQueueSerial);
    }

    // Update image with this command buffer's queueSerial. If VkEvent is enabled, image's current
    // event is also updated with this command's event.
    void retainImage(Context *context, ImageHelper *image);

    // Returns true if event already existed in this command buffer.
    bool hasSetEventPendingFlush(const RefCountedEvent &event) const
    {
        ASSERT(event.valid());
        return mRefCountedEvents.map[event.getImageLayout()] == event;
    }

    // Issue VkCmdSetEvent call for events in this command buffer.
    template <typename CommandBufferT>
    void flushSetEventsImpl(Context *context, CommandBufferT *commandBuffer);

    RefCountedEventCollector *getRefCountedEventCollector() { return &mRefCountedEventCollector; }

    const QueueSerial &getQueueSerial() const { return mQueueSerial; }

    void setAcquireNextImageSemaphore(VkSemaphore semaphore)
    {
        ASSERT(semaphore != VK_NULL_HANDLE);
        ASSERT(!mAcquireNextImageSemaphore.valid());
        mAcquireNextImageSemaphore.setHandle(semaphore);
    }

    // Dumping the command stream is disabled by default.
    static constexpr bool kEnableCommandStreamDiagnostics = false;

  protected:
    CommandBufferHelperCommon();
    ~CommandBufferHelperCommon();

    void initializeImpl();

    void resetImpl(Context *context);

    template <class DerivedT>
    angle::Result attachCommandPoolImpl(Context *context, SecondaryCommandPool *commandPool);
    template <class DerivedT, bool kIsRenderPassBuffer>
    angle::Result detachCommandPoolImpl(Context *context, SecondaryCommandPool **commandPoolOut);
    template <class DerivedT>
    void releaseCommandPoolImpl();

    template <class DerivedT>
    void attachAllocatorImpl(SecondaryCommandMemoryAllocator *allocator);
    template <class DerivedT>
    SecondaryCommandMemoryAllocator *detachAllocatorImpl();

    template <class DerivedT>
    void assertCanBeRecycledImpl();

    void bufferReadImpl(VkAccessFlags readAccessType,
                        PipelineStage readStage,
                        BufferHelper *buffer);
    void bufferReadImpl(VkAccessFlags readAccessType,
                        const gl::ShaderBitSet &readShaderStages,
                        BufferHelper *buffer)
    {
        for (gl::ShaderType shaderType : readShaderStages)
        {
            const vk::PipelineStage readStage = vk::GetPipelineStage(shaderType);
            bufferReadImpl(readAccessType, readStage, buffer);
        }
    }
    void imageReadImpl(Context *context,
                       VkImageAspectFlags aspectFlags,
                       ImageLayout imageLayout,
                       BarrierType barrierType,
                       ImageHelper *image);
    void imageWriteImpl(Context *context,
                        gl::LevelIndex level,
                        uint32_t layerStart,
                        uint32_t layerCount,
                        VkImageAspectFlags aspectFlags,
                        ImageLayout imageLayout,
                        BarrierType barrierType,
                        ImageHelper *image);

    void updateImageLayoutAndBarrier(Context *context,
                                     ImageHelper *image,
                                     VkImageAspectFlags aspectFlags,
                                     ImageLayout imageLayout,
                                     BarrierType barrierType);

    void addCommandDiagnosticsCommon(std::ostringstream *out);

    // Allocator used by this class.
    SecondaryCommandBlockAllocator mCommandAllocator;

    // Barriers to be executed before the command buffer.
    PipelineBarrierArray mPipelineBarriers;
    EventBarrierArray mEventBarriers;

    // The command pool *CommandBufferHelper::mCommandBuffer is allocated from.  Only used with
    // Vulkan secondary command buffers (as opposed to ANGLE's SecondaryCommandBuffer).
    SecondaryCommandPool *mCommandPool;

    // Whether the command buffers contains any draw/dispatch calls that possibly output data
    // through storage buffers and images.  This is used to determine whether glMemoryBarrier*
    // should flush the command buffer.
    bool mHasShaderStorageOutput;
    // Whether glMemoryBarrier has been called while commands are recorded in this command buffer.
    // This is used to know when to check and potentially flush the command buffer if storage
    // buffers and images are used in it.
    bool mHasGLMemoryBarrierIssued;

    // Tracks resources used in the command buffer.
    QueueSerial mQueueSerial;

    // Only used for swapChain images
    Semaphore mAcquireNextImageSemaphore;

    // The list of RefCountedEvents that have be tracked
    ImageLayoutEventMaps mRefCountedEvents;
    // The list of RefCountedEvents that should be garbage collected when it gets reset.
    RefCountedEventCollector mRefCountedEventCollector;
};

class SecondaryCommandBufferCollector;

class OutsideRenderPassCommandBufferHelper final : public CommandBufferHelperCommon
{
  public:
    OutsideRenderPassCommandBufferHelper();
    ~OutsideRenderPassCommandBufferHelper();

    angle::Result initialize(Context *context);

    angle::Result reset(Context *context, SecondaryCommandBufferCollector *commandBufferCollector);

    static constexpr bool ExecutesInline()
    {
        return OutsideRenderPassCommandBuffer::ExecutesInline();
    }

    OutsideRenderPassCommandBuffer &getCommandBuffer() { return mCommandBuffer; }

    bool empty() const { return mCommandBuffer.empty(); }

    angle::Result attachCommandPool(Context *context, SecondaryCommandPool *commandPool);
    angle::Result detachCommandPool(Context *context, SecondaryCommandPool **commandPoolOut);
    void releaseCommandPool();

    void attachAllocator(SecondaryCommandMemoryAllocator *allocator);
    SecondaryCommandMemoryAllocator *detachAllocator();

    void assertCanBeRecycled();

#if defined(ANGLE_ENABLE_ASSERTS)
    void markOpen() { mCommandBuffer.open(); }
    void markClosed() { mCommandBuffer.close(); }
#endif

    void bufferRead(ContextVk *contextVk,
                    VkAccessFlags readAccessType,
                    PipelineStage readStage,
                    BufferHelper *buffer)
    {
        bufferReadImpl(readAccessType, readStage, buffer);
        setBufferReadQueueSerial(contextVk, buffer);
    }

    void bufferRead(ContextVk *contextVk,
                    VkAccessFlags readAccessType,
                    const gl::ShaderBitSet &readShaderStages,
                    BufferHelper *buffer)
    {
        bufferReadImpl(readAccessType, readShaderStages, buffer);
        setBufferReadQueueSerial(contextVk, buffer);
    }

    void imageRead(ContextVk *contextVk,
                   VkImageAspectFlags aspectFlags,
                   ImageLayout imageLayout,
                   ImageHelper *image);
    void imageWrite(ContextVk *contextVk,
                    gl::LevelIndex level,
                    uint32_t layerStart,
                    uint32_t layerCount,
                    VkImageAspectFlags aspectFlags,
                    ImageLayout imageLayout,
                    ImageHelper *image);

    // Call SetEvent and have image's current event pointing to it.
    void trackImageWithEvent(Context *context, ImageHelper *image);
    void trackImagesWithEvent(Context *context, ImageHelper *srcImage, ImageHelper *dstImage);
    void trackImagesWithEvent(Context *context, const ImageHelperPtr *images, size_t count);

    // Issues SetEvent calls to the command buffer.
    void flushSetEvents(Context *context) { flushSetEventsImpl(context, &mCommandBuffer); }
    // Clean up event garbage. Note that ImageHelper object may still holding reference count to it,
    // so the event itself will not gets destroyed until the last refCount goes away.
    void collectRefCountedEventsGarbage(RefCountedEventsGarbageRecycler *garbageRecycler);

    angle::Result flushToPrimary(Context *context, CommandsState *commandsState);

    void setGLMemoryBarrierIssued()
    {
        if (!mCommandBuffer.empty())
        {
            mHasGLMemoryBarrierIssued = true;
        }
    }

    std::string getCommandDiagnostics();

    void setQueueSerial(SerialIndex index, Serial serial)
    {
        mQueueSerial = QueueSerial(index, serial);
    }

  private:
    angle::Result initializeCommandBuffer(Context *context);
    angle::Result endCommandBuffer(Context *context);
    void setBufferReadQueueSerial(ContextVk *contextVk, BufferHelper *buffer);

    OutsideRenderPassCommandBuffer mCommandBuffer;
    bool mIsCommandBufferEnded = false;

    friend class CommandBufferHelperCommon;
};

enum class ImagelessFramebuffer
{
    No,
    Yes,
};

class RenderPassFramebuffer : angle::NonCopyable
{
  public:
    RenderPassFramebuffer() = default;
    ~RenderPassFramebuffer() { mInitialFramebuffer.release(); }

    RenderPassFramebuffer &operator=(RenderPassFramebuffer &&other)
    {
        mInitialFramebuffer.setHandle(other.mInitialFramebuffer.release());
        std::swap(mImageViews, other.mImageViews);
        mWidth       = other.mWidth;
        mHeight      = other.mHeight;
        mLayers      = other.mLayers;
        mIsImageless = other.mIsImageless;
        return *this;
    }

    void reset();

    void setFramebuffer(Framebuffer &&initialFramebuffer,
                        FramebufferAttachmentsVector<VkImageView> &&imageViews,
                        uint32_t width,
                        uint32_t height,
                        uint32_t layers,
                        ImagelessFramebuffer imagelessFramebuffer)
    {
        ASSERT(initialFramebuffer.valid());
        mInitialFramebuffer = std::move(initialFramebuffer);
        mImageViews         = std::move(imageViews);
        mWidth              = width;
        mHeight             = height;
        mLayers             = layers;
        mIsImageless        = imagelessFramebuffer == ImagelessFramebuffer::Yes;
    }

    bool isImageless() { return mIsImageless; }
    const Framebuffer &getFramebuffer() const { return mInitialFramebuffer; }
    bool needsNewFramebufferWithResolveAttachments() const { return !mInitialFramebuffer.valid(); }

    // Helpers to determine if a resolve attachment already exists
    bool hasColorResolveAttachment(size_t colorIndexGL)
    {
        const size_t viewIndex = kColorResolveAttachmentBegin + colorIndexGL;
        return viewIndex < mImageViews.size() && mImageViews[viewIndex] != VK_NULL_HANDLE;
    }
    bool hasDepthStencilResolveAttachment()
    {
        return mImageViews[kDepthStencilResolveAttachment] != VK_NULL_HANDLE;
    }

    // Add a resolve attachment.  This is only called through glBlitFramebuffer, as other cases
    // where resolve attachments are implicitly added already include the resolve attachment when
    // initially populating mImageViews.
    void addColorResolveAttachment(size_t colorIndexGL, VkImageView view)
    {
        addResolveAttachment(kColorResolveAttachmentBegin + colorIndexGL, view);
    }
    void addDepthStencilResolveAttachment(VkImageView view)
    {
        addResolveAttachment(kDepthStencilResolveAttachment, view);
    }

    // Prepare for rendering by creating a new framebuffer because the initial framebuffer is not
    // valid (due to added resolve attachments).  This is called when the render pass is finalized.
    angle::Result packResolveViewsAndCreateFramebuffer(Context *context,
                                                       const RenderPass &renderPass,
                                                       Framebuffer *framebufferOut);

    // Prepare for rendering using the initial imageless framebuffer.
    void packResolveViewsForRenderPassBegin(VkRenderPassAttachmentBeginInfo *beginInfoOut);

    // Packs views in a contiguous list.
    //
    // It can be used before creating a framebuffer, or when starting a render pass with an
    // imageless framebuffer.
    static void PackViews(FramebufferAttachmentsVector<VkImageView> *views);

    static constexpr size_t kColorResolveAttachmentBegin = gl::IMPLEMENTATION_MAX_DRAW_BUFFERS + 2;
    static constexpr size_t kDepthStencilResolveAttachment =
        gl::IMPLEMENTATION_MAX_DRAW_BUFFERS * 2 + 2;

  private:
    void addResolveAttachment(size_t viewIndex, VkImageView view);
    void packResolveViews();

    // The following is the framebuffer object that was used to start the render pass.  If the
    // resolve attachments have not been modified, the same framebuffer object can be used.
    // Otherwise a temporary framebuffer object is created when the render pass is closed.  This
    // inefficiency is removed with VK_KHR_dynamic_rendering when supported.
    Framebuffer mInitialFramebuffer;

    // The first gl::IMPLEMENTATION_MAX_DRAW_BUFFERS + 2 attachments are laid out as follows:
    //
    // - Color attachments, if any
    // - Depth/stencil attachment, if any
    // - Fragment shading rate attachment, if any
    // - Padding if needed
    //
    // Starting from index gl::IMPLEMENTATION_MAX_DRAW_BUFFERS + 2, there are potentially another
    // gl::IMPLEMENTATION_MAX_DRAW_BUFFERS + 1 resolve attachments.  However, these are not packed
    // (with gaps per missing attachment, and depth/stencil resolve is last).  This allow more
    // resolve attachments to be added by optimizing calls to glBlitFramebuffer.  When the render
    // pass is closed, the resolve attachments are packed.
    FramebufferAttachmentsVector<VkImageView> mImageViews = {};

    uint32_t mWidth  = 0;
    uint32_t mHeight = 0;
    uint32_t mLayers = 0;

    bool mIsImageless = false;
};

class RenderPassCommandBufferHelper final : public CommandBufferHelperCommon
{
  public:
    RenderPassCommandBufferHelper();
    ~RenderPassCommandBufferHelper();

    angle::Result initialize(Context *context);

    angle::Result reset(Context *context, SecondaryCommandBufferCollector *commandBufferCollector);

    static constexpr bool ExecutesInline() { return RenderPassCommandBuffer::ExecutesInline(); }

    RenderPassCommandBuffer &getCommandBuffer()
    {
        return mCommandBuffers[mCurrentSubpassCommandBufferIndex];
    }

    bool empty() const { return mCommandBuffers[0].empty(); }

    angle::Result attachCommandPool(Context *context, SecondaryCommandPool *commandPool);
    void detachCommandPool(SecondaryCommandPool **commandPoolOut);
    void releaseCommandPool();

    void attachAllocator(SecondaryCommandMemoryAllocator *allocator);
    SecondaryCommandMemoryAllocator *detachAllocator();

    void assertCanBeRecycled();

#if defined(ANGLE_ENABLE_ASSERTS)
    void markOpen() { getCommandBuffer().open(); }
    void markClosed() { getCommandBuffer().close(); }
#endif

    void imageRead(ContextVk *contextVk,
                   VkImageAspectFlags aspectFlags,
                   ImageLayout imageLayout,
                   ImageHelper *image);
    void imageWrite(ContextVk *contextVk,
                    gl::LevelIndex level,
                    uint32_t layerStart,
                    uint32_t layerCount,
                    VkImageAspectFlags aspectFlags,
                    ImageLayout imageLayout,
                    ImageHelper *image);

    void bufferRead(ContextVk *contextVk,
                    VkAccessFlags readAccessType,
                    PipelineStage readStage,
                    BufferHelper *buffer)
    {
        bufferReadImpl(readAccessType, readStage, buffer);
        buffer->setQueueSerial(mQueueSerial);
    }
    void bufferRead(ContextVk *contextVk,
                    VkAccessFlags readAccessType,
                    const gl::ShaderBitSet &readShaderStages,
                    BufferHelper *buffer)
    {
        bufferReadImpl(readAccessType, readShaderStages, buffer);
        buffer->setQueueSerial(mQueueSerial);
    }

    void colorImagesDraw(gl::LevelIndex level,
                         uint32_t layerStart,
                         uint32_t layerCount,
                         ImageHelper *image,
                         ImageHelper *resolveImage,
                         UniqueSerial imageSiblingSerial,
                         PackedAttachmentIndex packedAttachmentIndex);
    void depthStencilImagesDraw(gl::LevelIndex level,
                                uint32_t layerStart,
                                uint32_t layerCount,
                                ImageHelper *image,
                                ImageHelper *resolveImage,
                                UniqueSerial imageSiblingSerial);
    void fragmentShadingRateImageRead(ImageHelper *image);

    bool usesImage(const ImageHelper &image) const;
    bool startedAndUsesImageWithBarrier(const ImageHelper &image) const;

    angle::Result flushToPrimary(Context *context,
                                 CommandsState *commandsState,
                                 const RenderPass &renderPass,
                                 VkFramebuffer framebufferOverride);

    bool started() const { return mRenderPassStarted; }

    // Finalize the layout if image has any deferred layout transition.
    void finalizeImageLayout(Context *context,
                             const ImageHelper *image,
                             UniqueSerial imageSiblingSerial);

    angle::Result beginRenderPass(ContextVk *contextVk,
                                  RenderPassFramebuffer &&framebuffer,
                                  const gl::Rectangle &renderArea,
                                  const RenderPassDesc &renderPassDesc,
                                  const AttachmentOpsArray &renderPassAttachmentOps,
                                  const PackedAttachmentCount colorAttachmentCount,
                                  const PackedAttachmentIndex depthStencilAttachmentIndex,
                                  const PackedClearValuesArray &clearValues,
                                  const QueueSerial &queueSerial,
                                  RenderPassCommandBuffer **commandBufferOut);

    angle::Result endRenderPass(ContextVk *contextVk);

    angle::Result nextSubpass(ContextVk *contextVk, RenderPassCommandBuffer **commandBufferOut);

    void beginTransformFeedback(size_t validBufferCount,
                                const VkBuffer *counterBuffers,
                                const VkDeviceSize *counterBufferOffsets,
                                bool rebindBuffers);

    void endTransformFeedback();

    void invalidateRenderPassColorAttachment(const gl::State &state,
                                             size_t colorIndexGL,
                                             PackedAttachmentIndex attachmentIndex,
                                             const gl::Rectangle &invalidateArea);
    void invalidateRenderPassDepthAttachment(const gl::DepthStencilState &dsState,
                                             const gl::Rectangle &invalidateArea);
    void invalidateRenderPassStencilAttachment(const gl::DepthStencilState &dsState,
                                               const gl::Rectangle &invalidateArea);

    void updateRenderPassColorClear(PackedAttachmentIndex colorIndexVk,
                                    const VkClearValue &colorClearValue);
    void updateRenderPassDepthStencilClear(VkImageAspectFlags aspectFlags,
                                           const VkClearValue &clearValue);

    const gl::Rectangle &getRenderArea() const { return mRenderArea; }

    // If render pass is started with a small render area due to a small scissor, and if a new
    // larger scissor is specified, grow the render area to accomodate it.
    void growRenderArea(ContextVk *contextVk, const gl::Rectangle &newRenderArea);

    void resumeTransformFeedback();
    void pauseTransformFeedback();
    bool isTransformFeedbackStarted() const { return mValidTransformFeedbackBufferCount > 0; }
    bool isTransformFeedbackActiveUnpaused() const { return mIsTransformFeedbackActiveUnpaused; }

    uint32_t getAndResetCounter()
    {
        uint32_t count = mCounter;
        mCounter       = 0;
        return count;
    }

    RenderPassFramebuffer &getFramebuffer() { return mFramebuffer; }
    const RenderPassFramebuffer &getFramebuffer() const { return mFramebuffer; }

    void onColorAccess(PackedAttachmentIndex packedAttachmentIndex, ResourceAccess access);
    void onDepthAccess(ResourceAccess access);
    void onStencilAccess(ResourceAccess access);

    bool hasAnyColorAccess(PackedAttachmentIndex packedAttachmentIndex)
    {
        ASSERT(packedAttachmentIndex < mColorAttachmentsCount);
        return mColorAttachments[packedAttachmentIndex].hasAnyAccess();
    }
    bool hasAnyDepthAccess() { return mDepthAttachment.hasAnyAccess(); }
    bool hasAnyStencilAccess() { return mStencilAttachment.hasAnyAccess(); }

    void addColorResolveAttachment(size_t colorIndexGL, VkImageView view);
    void addDepthStencilResolveAttachment(VkImageView view, VkImageAspectFlags aspects);

    bool hasDepthWriteOrClear() const
    {
        return mDepthAttachment.hasWriteAccess() ||
               mAttachmentOps[mDepthStencilAttachmentIndex].loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR;
    }

    bool hasStencilWriteOrClear() const
    {
        return mStencilAttachment.hasWriteAccess() ||
               mAttachmentOps[mDepthStencilAttachmentIndex].stencilLoadOp ==
                   VK_ATTACHMENT_LOAD_OP_CLEAR;
    }

    bool hasDepthStencilWriteOrClear() const
    {
        return hasDepthWriteOrClear() || hasStencilWriteOrClear();
    }

    const RenderPassDesc &getRenderPassDesc() const { return mRenderPassDesc; }
    const AttachmentOpsArray &getAttachmentOps() const { return mAttachmentOps; }

    void setImageOptimizeForPresent(ImageHelper *image) { mImageOptimizeForPresent = image; }

    void setGLMemoryBarrierIssued()
    {
        if (mRenderPassStarted)
        {
            mHasGLMemoryBarrierIssued = true;
        }
    }
    std::string getCommandDiagnostics();

    // Readonly depth stencil mode and feedback loop mode
    void updateDepthReadOnlyMode(RenderPassUsageFlags dsUsageFlags);
    void updateStencilReadOnlyMode(RenderPassUsageFlags dsUsageFlags);
    void updateDepthStencilReadOnlyMode(RenderPassUsageFlags dsUsageFlags,
                                        VkImageAspectFlags dsAspectFlags);

    void collectRefCountedEventsGarbage(RefCountedEventsGarbageRecycler *garbageRecycler);

  private:
    uint32_t getSubpassCommandBufferCount() const { return mCurrentSubpassCommandBufferIndex + 1; }

    angle::Result initializeCommandBuffer(Context *context);
    angle::Result beginRenderPassCommandBuffer(ContextVk *contextVk);
    angle::Result endRenderPassCommandBuffer(ContextVk *contextVk);

    uint32_t getRenderPassWriteCommandCount()
    {
        // All subpasses are chained (no subpasses running in parallel), so the cmd count can be
        // considered continuous among subpasses.
        return mPreviousSubpassesCmdCount + getCommandBuffer().getRenderPassWriteCommandCount();
    }

    void updateStartedRenderPassWithDepthStencilMode(RenderPassAttachment *resolveAttachment,
                                                     bool renderPassHasWriteOrClear,
                                                     RenderPassUsageFlags dsUsageFlags,
                                                     RenderPassUsage readOnlyAttachmentUsage);

    // We can't determine the image layout at the renderpass start time since their full usage
    // aren't known until later time. We finalize the layout when either ImageHelper object is
    // released or when renderpass ends.
    void finalizeColorImageLayout(Context *context,
                                  ImageHelper *image,
                                  PackedAttachmentIndex packedAttachmentIndex,
                                  bool isResolveImage);
    void finalizeColorImageLoadStore(Context *context, PackedAttachmentIndex packedAttachmentIndex);
    void finalizeDepthStencilImageLayout(Context *context);
    void finalizeDepthStencilResolveImageLayout(Context *context);
    void finalizeDepthStencilLoadStore(Context *context);

    void finalizeColorImageLayoutAndLoadStore(Context *context,
                                              PackedAttachmentIndex packedAttachmentIndex);
    void finalizeDepthStencilImageLayoutAndLoadStore(Context *context);
    void finalizeFragmentShadingRateImageLayout(Context *context);

    void trackImagesWithEvent(Context *context, const ImageHelperPtr *images, size_t count);
    void executeSetEvents(Context *context, PrimaryCommandBuffer *primary);

    // When using Vulkan secondary command buffers, each subpass must be recorded in a separate
    // command buffer.  Currently ANGLE produces render passes with at most 2 subpasses.
    static constexpr size_t kMaxSubpassCount = 2;
    std::array<RenderPassCommandBuffer, kMaxSubpassCount> mCommandBuffers;
    uint32_t mCurrentSubpassCommandBufferIndex;

    // RenderPass state
    uint32_t mCounter;
    RenderPassDesc mRenderPassDesc;
    AttachmentOpsArray mAttachmentOps;
    RenderPassFramebuffer mFramebuffer;
    gl::Rectangle mRenderArea;
    PackedClearValuesArray mClearValues;
    bool mRenderPassStarted;

    // Transform feedback state
    gl::TransformFeedbackBuffersArray<VkBuffer> mTransformFeedbackCounterBuffers;
    gl::TransformFeedbackBuffersArray<VkDeviceSize> mTransformFeedbackCounterBufferOffsets;
    uint32_t mValidTransformFeedbackBufferCount;
    bool mRebindTransformFeedbackBuffers;
    bool mIsTransformFeedbackActiveUnpaused;

    // State tracking for whether to optimize the storeOp to DONT_CARE
    uint32_t mPreviousSubpassesCmdCount;

    // Keep track of the depth/stencil attachment index
    PackedAttachmentIndex mDepthStencilAttachmentIndex;

    // Array size of mColorAttachments
    PackedAttachmentCount mColorAttachmentsCount;
    // Attached render target images. Color and depth resolve images always come last.
    PackedRenderPassAttachmentArray mColorAttachments;
    PackedRenderPassAttachmentArray mColorResolveAttachments;

    RenderPassAttachment mDepthAttachment;
    RenderPassAttachment mDepthResolveAttachment;

    RenderPassAttachment mStencilAttachment;
    RenderPassAttachment mStencilResolveAttachment;

    RenderPassAttachment mFragmentShadingRateAtachment;

    // This is last renderpass before present and this is the image will be presented. We can use
    // final layout of the renderpass to transition it to the presentable layout
    ImageHelper *mImageOptimizeForPresent;

    friend class CommandBufferHelperCommon;
};

// The following class helps support both Vulkan and ANGLE secondary command buffers by
// encapsulating their differences.
template <typename CommandBufferHelperT>
class CommandBufferRecycler
{
  public:
    CommandBufferRecycler()  = default;
    ~CommandBufferRecycler() = default;

    void onDestroy();

    angle::Result getCommandBufferHelper(Context *context,
                                         SecondaryCommandPool *commandPool,
                                         SecondaryCommandMemoryAllocator *commandsAllocator,
                                         CommandBufferHelperT **commandBufferHelperOut);

    void recycleCommandBufferHelper(CommandBufferHelperT **commandBuffer);

  private:
    angle::SimpleMutex mMutex;
    std::vector<CommandBufferHelperT *> mCommandBufferHelperFreeList;
};

// The source of update to an ImageHelper
enum class UpdateSource
{
    // Clear an image subresource.
    Clear,
    // Clear only the emulated channels of the subresource.  This operation is more expensive than
    // Clear, and so is only used for emulated color formats and only for external images.  Color
    // only because depth or stencil clear is already per channel, so Clear works for them.
    // External only because they may contain data that needs to be preserved.  Additionally, this
    // is a one-time only clear.  Once the emulated channels are cleared, ANGLE ensures that they
    // remain untouched.
    ClearEmulatedChannelsOnly,
    // When an image with emulated channels is invalidated, a clear may be restaged to keep the
    // contents of the emulated channels defined.  This is given a dedicated enum value, so it can
    // be removed if the invalidate is undone at the end of the render pass.
    ClearAfterInvalidate,
    // The source of the copy is a buffer.
    Buffer,
    // The source of the copy is an image.
    Image,
};

enum class ApplyImageUpdate
{
    ImmediatelyInUnlockedTailCall,
    Immediately,
    Defer,
};

constexpr VkImageAspectFlagBits IMAGE_ASPECT_DEPTH_STENCIL =
    static_cast<VkImageAspectFlagBits>(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);

bool FormatHasNecessaryFeature(Renderer *renderer,
                               angle::FormatID formatID,
                               VkImageTiling tilingMode,
                               VkFormatFeatureFlags featureBits);

bool CanCopyWithTransfer(Renderer *renderer,
                         angle::FormatID srcFormatID,
                         VkImageTiling srcTilingMode,
                         angle::FormatID dstFormatID,
                         VkImageTiling dstTilingMode);

class ImageViewHelper;
class ImageHelper final : public Resource, public angle::Subject
{
  public:
    ImageHelper();
    ~ImageHelper() override;

    angle::Result init(Context *context,
                       gl::TextureType textureType,
                       const VkExtent3D &extents,
                       const Format &format,
                       GLint samples,
                       VkImageUsageFlags usage,
                       gl::LevelIndex firstLevel,
                       uint32_t mipLevels,
                       uint32_t layerCount,
                       bool isRobustResourceInitEnabled,
                       bool hasProtectedContent);
    angle::Result initMSAASwapchain(Context *context,
                                    gl::TextureType textureType,
                                    const VkExtent3D &extents,
                                    bool rotatedAspectRatio,
                                    const Format &format,
                                    GLint samples,
                                    VkImageUsageFlags usage,
                                    gl::LevelIndex firstLevel,
                                    uint32_t mipLevels,
                                    uint32_t layerCount,
                                    bool isRobustResourceInitEnabled,
                                    bool hasProtectedContent);
    angle::Result initExternal(Context *context,
                               gl::TextureType textureType,
                               const VkExtent3D &extents,
                               angle::FormatID intendedFormatID,
                               angle::FormatID actualFormatID,
                               GLint samples,
                               VkImageUsageFlags usage,
                               VkImageCreateFlags additionalCreateFlags,
                               ImageLayout initialLayout,
                               const void *externalImageCreateInfo,
                               gl::LevelIndex firstLevel,
                               uint32_t mipLevels,
                               uint32_t layerCount,
                               bool isRobustResourceInitEnabled,
                               bool hasProtectedContent,
                               YcbcrConversionDesc conversionDesc);
    VkResult initMemory(Context *context,
                        const MemoryProperties &memoryProperties,
                        VkMemoryPropertyFlags flags,
                        VkMemoryPropertyFlags excludedFlags,
                        const VkMemoryRequirements *memoryRequirements,
                        const bool allocateDedicatedMemory,
                        MemoryAllocationType allocationType,
                        VkMemoryPropertyFlags *flagsOut,
                        VkDeviceSize *sizeOut);
    angle::Result initMemoryAndNonZeroFillIfNeeded(Context *context,
                                                   bool hasProtectedContent,
                                                   const MemoryProperties &memoryProperties,
                                                   VkMemoryPropertyFlags flags,
                                                   MemoryAllocationType allocationType);
    angle::Result initExternalMemory(Context *context,
                                     const MemoryProperties &memoryProperties,
                                     const VkMemoryRequirements &memoryRequirements,
                                     uint32_t extraAllocationInfoCount,
                                     const void **extraAllocationInfo,
                                     uint32_t currentQueueFamilyIndex,
                                     VkMemoryPropertyFlags flags);

    static constexpr VkImageUsageFlags kDefaultImageViewUsageFlags = 0;
    angle::Result initLayerImageView(Context *context,
                                     gl::TextureType textureType,
                                     VkImageAspectFlags aspectMask,
                                     const gl::SwizzleState &swizzleMap,
                                     ImageView *imageViewOut,
                                     LevelIndex baseMipLevelVk,
                                     uint32_t levelCount,
                                     uint32_t baseArrayLayer,
                                     uint32_t layerCount,
                                     gl::SrgbWriteControlMode srgbWriteControlMode,
                                     gl::YuvSamplingMode yuvSamplingMode,
                                     VkImageUsageFlags imageUsageFlags) const;
    angle::Result initReinterpretedLayerImageView(Context *context,
                                                  gl::TextureType textureType,
                                                  VkImageAspectFlags aspectMask,
                                                  const gl::SwizzleState &swizzleMap,
                                                  ImageView *imageViewOut,
                                                  LevelIndex baseMipLevelVk,
                                                  uint32_t levelCount,
                                                  uint32_t baseArrayLayer,
                                                  uint32_t layerCount,
                                                  VkImageUsageFlags imageUsageFlags,
                                                  angle::FormatID imageViewFormat) const;
    angle::Result initImageView(Context *context,
                                gl::TextureType textureType,
                                VkImageAspectFlags aspectMask,
                                const gl::SwizzleState &swizzleMap,
                                ImageView *imageViewOut,
                                LevelIndex baseMipLevelVk,
                                uint32_t levelCount,
                                VkImageUsageFlags imageUsageFlags);
    // Create a 2D[Array] for staging purposes.  Used by:
    //
    // - TextureVk::copySubImageImplWithDraw
    // - FramebufferVk::readPixelsImpl
    //
    angle::Result init2DStaging(Context *context,
                                bool hasProtectedContent,
                                const MemoryProperties &memoryProperties,
                                const gl::Extents &glExtents,
                                angle::FormatID intendedFormatID,
                                angle::FormatID actualFormatID,
                                VkImageUsageFlags usage,
                                uint32_t layerCount);
    // Create an image for staging purposes.  Used by:
    //
    // - TextureVk::copyAndStageImageData
    //
    angle::Result initStaging(Context *context,
                              bool hasProtectedContent,
                              const MemoryProperties &memoryProperties,
                              VkImageType imageType,
                              const VkExtent3D &extents,
                              angle::FormatID intendedFormatID,
                              angle::FormatID actualFormatID,
                              GLint samples,
                              VkImageUsageFlags usage,
                              uint32_t mipLevels,
                              uint32_t layerCount);
    // Create a multisampled image for use as the implicit image in multisampled render to texture
    // rendering.  If LAZILY_ALLOCATED memory is available, it will prefer that.
    angle::Result initImplicitMultisampledRenderToTexture(Context *context,
                                                          bool hasProtectedContent,
                                                          const MemoryProperties &memoryProperties,
                                                          gl::TextureType textureType,
                                                          GLint samples,
                                                          const ImageHelper &resolveImage,
                                                          bool isRobustResourceInitEnabled);

    // Helper for initExternal and users to automatically derive the appropriate VkImageCreateInfo
    // pNext chain based on the given parameters, and adjust create flags.  In some cases, these
    // shouldn't be automatically derived, for example when importing images through
    // EXT_external_objects and ANGLE_external_objects_flags.
    static constexpr uint32_t kImageListFormatCount = 2;
    using ImageListFormats                          = std::array<VkFormat, kImageListFormatCount>;
    static const void *DeriveCreateInfoPNext(
        Context *context,
        angle::FormatID actualFormatID,
        const void *pNext,
        VkImageFormatListCreateInfoKHR *imageFormatListInfoStorage,
        ImageListFormats *imageListFormatsStorage,
        VkImageCreateFlags *createFlagsOut);

    // Check whether the given format supports the provided flags.
    enum class FormatSupportCheck
    {
        OnlyQuerySuccess,
        RequireMultisampling
    };
    static bool FormatSupportsUsage(Renderer *renderer,
                                    VkFormat format,
                                    VkImageType imageType,
                                    VkImageTiling tilingMode,
                                    VkImageUsageFlags usageFlags,
                                    VkImageCreateFlags createFlags,
                                    void *propertiesPNext,
                                    const FormatSupportCheck formatSupportCheck);

    // Image formats used for the creation of imageless framebuffers.
    using ImageFormats = angle::FixedVector<VkFormat, kImageListFormatCount>;
    ImageFormats &getViewFormats() { return mViewFormats; }
    const ImageFormats &getViewFormats() const { return mViewFormats; }

    // Helper for initExternal and users to extract the view formats of the image from the pNext
    // chain in VkImageCreateInfo.
    void deriveImageViewFormatFromCreateInfoPNext(VkImageCreateInfo &imageInfo,
                                                  ImageFormats &formatOut);

    // Release the underlining VkImage object for garbage collection.
    void releaseImage(Renderer *renderer);
    // Similar to releaseImage, but also notify all contexts in the same share group to stop
    // accessing to it.
    void releaseImageFromShareContexts(Renderer *renderer,
                                       ContextVk *contextVk,
                                       UniqueSerial imageSiblingSerial);
    void finalizeImageLayoutInShareContexts(Renderer *renderer,
                                            ContextVk *contextVk,
                                            UniqueSerial imageSiblingSerial);

    void releaseStagedUpdates(Renderer *renderer);

    bool valid() const { return mImage.valid(); }

    VkImageAspectFlags getAspectFlags() const;
    // True if image contains both depth & stencil aspects
    bool isCombinedDepthStencilFormat() const;
    void destroy(Renderer *renderer);
    void release(Renderer *renderer) { destroy(renderer); }

    void init2DWeakReference(Context *context,
                             VkImage handle,
                             const gl::Extents &glExtents,
                             bool rotatedAspectRatio,
                             angle::FormatID intendedFormatID,
                             angle::FormatID actualFormatID,
                             VkImageCreateFlags createFlags,
                             VkImageUsageFlags usage,
                             GLint samples,
                             bool isRobustResourceInitEnabled);
    void resetImageWeakReference();

    const Image &getImage() const { return mImage; }
    const DeviceMemory &getDeviceMemory() const { return mDeviceMemory; }
    const Allocation &getAllocation() const { return mVmaAllocation; }

    const VkImageCreateInfo &getVkImageCreateInfo() const { return mVkImageCreateInfo; }
    void setTilingMode(VkImageTiling tilingMode) { mTilingMode = tilingMode; }
    VkImageTiling getTilingMode() const { return mTilingMode; }
    VkImageCreateFlags getCreateFlags() const { return mCreateFlags; }
    VkImageUsageFlags getUsage() const { return mUsage; }
    VkImageType getType() const { return mImageType; }
    const VkExtent3D &getExtents() const { return mExtents; }
    const VkExtent3D getRotatedExtents() const;
    uint32_t getLayerCount() const
    {
        ASSERT(valid());
        return mLayerCount;
    }
    uint32_t getLevelCount() const
    {
        ASSERT(valid());
        return mLevelCount;
    }
    angle::FormatID getIntendedFormatID() const
    {
        ASSERT(valid());
        return mIntendedFormatID;
    }
    const angle::Format &getIntendedFormat() const
    {
        ASSERT(valid());
        return angle::Format::Get(mIntendedFormatID);
    }
    angle::FormatID getActualFormatID() const
    {
        ASSERT(valid());
        return mActualFormatID;
    }
    VkFormat getActualVkFormat() const
    {
        ASSERT(valid());
        return GetVkFormatFromFormatID(mActualFormatID);
    }
    const angle::Format &getActualFormat() const
    {
        ASSERT(valid());
        return angle::Format::Get(mActualFormatID);
    }
    bool hasEmulatedImageChannels() const;
    bool hasEmulatedDepthChannel() const;
    bool hasEmulatedStencilChannel() const;
    bool hasEmulatedImageFormat() const { return mActualFormatID != mIntendedFormatID; }
    bool hasInefficientlyEmulatedImageFormat() const;
    GLint getSamples() const { return mSamples; }

    ImageSerial getImageSerial() const
    {
        ASSERT(valid() && mImageSerial.valid());
        return mImageSerial;
    }

    void setCurrentImageLayout(ImageLayout newLayout)
    {
        // Once you transition to ImageLayout::SharedPresent, you never transition out of it.
        if (mCurrentLayout == ImageLayout::SharedPresent)
        {
            return;
        }
        mCurrentLayout = newLayout;
    }
    ImageLayout getCurrentImageLayout() const { return mCurrentLayout; }
    VkImageLayout getCurrentLayout(Context *context) const;
    const QueueSerial &getBarrierQueueSerial() const { return mBarrierQueueSerial; }

    gl::Extents getLevelExtents(LevelIndex levelVk) const;
    // Helper function to calculate the extents of a render target created for a certain mip of the
    // image.
    gl::Extents getLevelExtents2D(LevelIndex levelVk) const;
    gl::Extents getRotatedLevelExtents2D(LevelIndex levelVk) const;

    bool isDepthOrStencil() const;

    void setRenderPassUsageFlag(RenderPassUsage flag);
    void clearRenderPassUsageFlag(RenderPassUsage flag);
    void resetRenderPassUsageFlags();
    bool hasRenderPassUsageFlag(RenderPassUsage flag) const;
    bool usedByCurrentRenderPassAsAttachmentAndSampler(RenderPassUsage textureSamplerUsage) const;

    static void Copy(Context *context,
                     ImageHelper *srcImage,
                     ImageHelper *dstImage,
                     const gl::Offset &srcOffset,
                     const gl::Offset &dstOffset,
                     const gl::Extents &copySize,
                     const VkImageSubresourceLayers &srcSubresources,
                     const VkImageSubresourceLayers &dstSubresources,
                     OutsideRenderPassCommandBuffer *commandBuffer);

    static angle::Result CopyImageSubData(const gl::Context *context,
                                          ImageHelper *srcImage,
                                          GLint srcLevel,
                                          GLint srcX,
                                          GLint srcY,
                                          GLint srcZ,
                                          ImageHelper *dstImage,
                                          GLint dstLevel,
                                          GLint dstX,
                                          GLint dstY,
                                          GLint dstZ,
                                          GLsizei srcWidth,
                                          GLsizei srcHeight,
                                          GLsizei srcDepth);

    // Generate mipmap from level 0 into the rest of the levels with blit.
    angle::Result generateMipmapsWithBlit(ContextVk *contextVk,
                                          LevelIndex baseLevel,
                                          LevelIndex maxLevel);

    // Resolve this image into a destination image.  This image should be in the TransferSrc layout.
    // The destination image is automatically transitioned into TransferDst.
    void resolve(ImageHelper *dst,
                 const VkImageResolve &region,
                 OutsideRenderPassCommandBuffer *commandBuffer);

    // Data staging
    void removeSingleSubresourceStagedUpdates(ContextVk *contextVk,
                                              gl::LevelIndex levelIndexGL,
                                              uint32_t layerIndex,
                                              uint32_t layerCount);
    void removeSingleStagedClearAfterInvalidate(gl::LevelIndex levelIndexGL,
                                                uint32_t layerIndex,
                                                uint32_t layerCount);
    void removeStagedUpdates(Context *context,
                             gl::LevelIndex levelGLStart,
                             gl::LevelIndex levelGLEnd);

    angle::Result stageSubresourceUpdateImpl(ContextVk *contextVk,
                                             const gl::ImageIndex &index,
                                             const gl::Extents &glExtents,
                                             const gl::Offset &offset,
                                             const gl::InternalFormat &formatInfo,
                                             const gl::PixelUnpackState &unpack,
                                             GLenum type,
                                             const uint8_t *pixels,
                                             const Format &vkFormat,
                                             ImageAccess access,
                                             const GLuint inputRowPitch,
                                             const GLuint inputDepthPitch,
                                             const GLuint inputSkipBytes,
                                             ApplyImageUpdate applyUpdate,
                                             bool *updateAppliedImmediatelyOut);

    angle::Result stageSubresourceUpdate(ContextVk *contextVk,
                                         const gl::ImageIndex &index,
                                         const gl::Extents &glExtents,
                                         const gl::Offset &offset,
                                         const gl::InternalFormat &formatInfo,
                                         const gl::PixelUnpackState &unpack,
                                         GLenum type,
                                         const uint8_t *pixels,
                                         const Format &vkFormat,
                                         ImageAccess access,
                                         ApplyImageUpdate applyUpdate,
                                         bool *updateAppliedImmediatelyOut);

    angle::Result stageSubresourceUpdateAndGetData(ContextVk *contextVk,
                                                   size_t allocationSize,
                                                   const gl::ImageIndex &imageIndex,
                                                   const gl::Extents &glExtents,
                                                   const gl::Offset &offset,
                                                   uint8_t **destData,
                                                   angle::FormatID formatID);

    angle::Result stageSubresourceUpdateFromFramebuffer(const gl::Context *context,
                                                        const gl::ImageIndex &index,
                                                        const gl::Rectangle &sourceArea,
                                                        const gl::Offset &dstOffset,
                                                        const gl::Extents &dstExtent,
                                                        const gl::InternalFormat &formatInfo,
                                                        ImageAccess access,
                                                        FramebufferVk *framebufferVk);

    void stageSubresourceUpdateFromImage(RefCounted<ImageHelper> *image,
                                         const gl::ImageIndex &index,
                                         LevelIndex srcMipLevel,
                                         const gl::Offset &destOffset,
                                         const gl::Extents &glExtents,
                                         const VkImageType imageType);

    // Takes an image and stages a subresource update for each level of it, including its full
    // extent and all its layers, at the specified GL level.
    void stageSubresourceUpdatesFromAllImageLevels(RefCounted<ImageHelper> *image,
                                                   gl::LevelIndex baseLevel);

    // Stage a clear to an arbitrary value.
    void stageClear(const gl::ImageIndex &index,
                    VkImageAspectFlags aspectFlags,
                    const VkClearValue &clearValue);

    // Stage a clear based on robust resource init.
    angle::Result stageRobustResourceClearWithFormat(ContextVk *contextVk,
                                                     const gl::ImageIndex &index,
                                                     const gl::Extents &glExtents,
                                                     const angle::Format &intendedFormat,
                                                     const angle::Format &imageFormat);
    void stageRobustResourceClear(const gl::ImageIndex &index);

    angle::Result stageResourceClearWithFormat(ContextVk *contextVk,
                                               const gl::ImageIndex &index,
                                               const gl::Extents &glExtents,
                                               const angle::Format &intendedFormat,
                                               const angle::Format &imageFormat,
                                               const VkClearValue &clearValue);

    // Stage the currently allocated image as updates to base level and on, making this !valid().
    // This is used for:
    //
    // - Mipmap generation, where levelCount is 1 so only the base level is retained
    // - Image respecification, where every level (other than those explicitly skipped) is staged
    void stageSelfAsSubresourceUpdates(ContextVk *contextVk,
                                       uint32_t levelCount,
                                       gl::TextureType textureType,
                                       const gl::CubeFaceArray<gl::TexLevelMask> &skipLevels);

    // Flush staged updates for a single subresource. Can optionally take a parameter to defer
    // clears to a subsequent RenderPass load op.
    angle::Result flushSingleSubresourceStagedUpdates(ContextVk *contextVk,
                                                      gl::LevelIndex levelGL,
                                                      uint32_t layer,
                                                      uint32_t layerCount,
                                                      ClearValuesArray *deferredClears,
                                                      uint32_t deferredClearIndex);

    // Flushes staged updates to a range of levels and layers from start to (but not including) end.
    // Due to the nature of updates (done wholly to a VkImageSubresourceLayers), some unsolicited
    // layers may also be updated.
    angle::Result flushStagedUpdates(ContextVk *contextVk,
                                     gl::LevelIndex levelGLStart,
                                     gl::LevelIndex levelGLEnd,
                                     uint32_t layerStart,
                                     uint32_t layerEnd,
                                     const gl::CubeFaceArray<gl::TexLevelMask> &skipLevels);

    // Creates a command buffer and flushes all staged updates.  This is used for one-time
    // initialization of resources that we don't expect to accumulate further staged updates, such
    // as with renderbuffers or surface images.
    angle::Result flushAllStagedUpdates(ContextVk *contextVk);

    bool hasStagedUpdatesForSubresource(gl::LevelIndex levelGL,
                                        uint32_t layer,
                                        uint32_t layerCount) const;
    bool hasStagedUpdatesInAllocatedLevels() const;

    bool removeStagedClearUpdatesAndReturnColor(gl::LevelIndex levelGL,
                                                const VkClearColorValue **color);

    void recordWriteBarrier(Context *context,
                            VkImageAspectFlags aspectMask,
                            ImageLayout newLayout,
                            gl::LevelIndex levelStart,
                            uint32_t levelCount,
                            uint32_t layerStart,
                            uint32_t layerCount,
                            OutsideRenderPassCommandBufferHelper *commands);

    void recordReadSubresourceBarrier(Context *context,
                                      VkImageAspectFlags aspectMask,
                                      ImageLayout newLayout,
                                      gl::LevelIndex levelStart,
                                      uint32_t levelCount,
                                      uint32_t layerStart,
                                      uint32_t layerCount,
                                      OutsideRenderPassCommandBufferHelper *commands);

    void recordWriteBarrierOneOff(Context *context,
                                  ImageLayout newLayout,
                                  PrimaryCommandBuffer *commandBuffer,
                                  VkSemaphore *acquireNextImageSemaphoreOut)
    {
        // Since we are doing an out of order one off submission, there shouldn't be any pending
        // setEvent.
        ASSERT(!mCurrentEvent.valid());
        barrierImpl(context, getAspectFlags(), newLayout, mCurrentQueueFamilyIndex, nullptr,
                    commandBuffer, acquireNextImageSemaphoreOut);
    }

    // This function can be used to prevent issuing redundant layout transition commands.
    bool isReadBarrierNecessary(ImageLayout newLayout) const;
    bool isReadSubresourceBarrierNecessary(ImageLayout newLayout,
                                           gl::LevelIndex levelStart,
                                           uint32_t levelCount,
                                           uint32_t layerStart,
                                           uint32_t layerCount) const;
    bool isWriteBarrierNecessary(ImageLayout newLayout,
                                 gl::LevelIndex levelStart,
                                 uint32_t levelCount,
                                 uint32_t layerStart,
                                 uint32_t layerCount) const;

    void recordReadBarrier(Context *context,
                           VkImageAspectFlags aspectMask,
                           ImageLayout newLayout,
                           OutsideRenderPassCommandBufferHelper *commands);

    bool isQueueChangeNeccesary(uint32_t newQueueFamilyIndex) const
    {
        return mCurrentQueueFamilyIndex != newQueueFamilyIndex;
    }

    void changeLayoutAndQueue(Context *context,
                              VkImageAspectFlags aspectMask,
                              ImageLayout newLayout,
                              uint32_t newQueueFamilyIndex,
                              OutsideRenderPassCommandBuffer *commandBuffer);

    // Returns true if barrier has been generated
    void updateLayoutAndBarrier(Context *context,
                                VkImageAspectFlags aspectMask,
                                ImageLayout newLayout,
                                BarrierType barrierType,
                                const QueueSerial &queueSerial,
                                PipelineBarrierArray *pipelineBarriers,
                                EventBarrierArray *eventBarriers,
                                RefCountedEventCollector *eventCollector,
                                VkSemaphore *semaphoreOut);

    // Performs an ownership transfer from an external instance or API.
    void acquireFromExternal(Context *context,
                             uint32_t externalQueueFamilyIndex,
                             uint32_t rendererQueueFamilyIndex,
                             ImageLayout currentLayout,
                             OutsideRenderPassCommandBuffer *commandBuffer);

    // Performs an ownership transfer to an external instance or API.
    void releaseToExternal(Context *context,
                           uint32_t rendererQueueFamilyIndex,
                           uint32_t externalQueueFamilyIndex,
                           ImageLayout desiredLayout,
                           OutsideRenderPassCommandBuffer *commandBuffer);

    // Returns true if the image is owned by an external API or instance.
    bool isReleasedToExternal() const;

    gl::LevelIndex getFirstAllocatedLevel() const
    {
        ASSERT(valid());
        return mFirstAllocatedLevel;
    }
    gl::LevelIndex getLastAllocatedLevel() const;
    LevelIndex toVkLevel(gl::LevelIndex levelIndexGL) const;
    gl::LevelIndex toGLLevel(LevelIndex levelIndexVk) const;

    angle::Result copyImageDataToBuffer(ContextVk *contextVk,
                                        gl::LevelIndex sourceLevelGL,
                                        uint32_t layerCount,
                                        uint32_t baseLayer,
                                        const gl::Box &sourceArea,
                                        BufferHelper *dstBuffer,
                                        uint8_t **outDataPtr);

    angle::Result copySurfaceImageToBuffer(DisplayVk *displayVk,
                                           gl::LevelIndex sourceLevelGL,
                                           uint32_t layerCount,
                                           uint32_t baseLayer,
                                           const gl::Box &sourceArea,
                                           vk::BufferHelper *bufferHelperOut);

    angle::Result copyBufferToSurfaceImage(DisplayVk *displayVk,
                                           gl::LevelIndex destLevelGL,
                                           uint32_t layerCount,
                                           uint32_t baseLayer,
                                           const gl::Box &destArea,
                                           vk::BufferHelper *bufferHelper);

    static angle::Result GetReadPixelsParams(ContextVk *contextVk,
                                             const gl::PixelPackState &packState,
                                             gl::Buffer *packBuffer,
                                             GLenum format,
                                             GLenum type,
                                             const gl::Rectangle &area,
                                             const gl::Rectangle &clippedArea,
                                             PackPixelsParams *paramsOut,
                                             GLuint *skipBytesOut);

    angle::Result readPixelsForGetImage(ContextVk *contextVk,
                                        const gl::PixelPackState &packState,
                                        gl::Buffer *packBuffer,
                                        gl::LevelIndex levelGL,
                                        uint32_t layer,
                                        uint32_t layerCount,
                                        GLenum format,
                                        GLenum type,
                                        void *pixels);

    angle::Result readPixelsForCompressedGetImage(ContextVk *contextVk,
                                                  const gl::PixelPackState &packState,
                                                  gl::Buffer *packBuffer,
                                                  gl::LevelIndex levelGL,
                                                  uint32_t layer,
                                                  uint32_t layerCount,
                                                  void *pixels);

    angle::Result readPixelsWithCompute(ContextVk *contextVk,
                                        ImageHelper *src,
                                        const PackPixelsParams &packPixelsParams,
                                        const VkOffset3D &srcOffset,
                                        const VkExtent3D &srcExtent,
                                        ptrdiff_t pixelsOffset,
                                        const VkImageSubresourceLayers &srcSubresource);

    angle::Result readPixels(ContextVk *contextVk,
                             const gl::Rectangle &area,
                             const PackPixelsParams &packPixelsParams,
                             VkImageAspectFlagBits copyAspectFlags,
                             gl::LevelIndex levelGL,
                             uint32_t layer,
                             void *pixels);

    angle::Result CalculateBufferInfo(ContextVk *contextVk,
                                      const gl::Extents &glExtents,
                                      const gl::InternalFormat &formatInfo,
                                      const gl::PixelUnpackState &unpack,
                                      GLenum type,
                                      bool is3D,
                                      GLuint *inputRowPitch,
                                      GLuint *inputDepthPitch,
                                      GLuint *inputSkipBytes);

    // Mark a given subresource as written to.  The subresource is identified by [levelStart,
    // levelStart + levelCount) and [layerStart, layerStart + layerCount).
    void onWrite(gl::LevelIndex levelStart,
                 uint32_t levelCount,
                 uint32_t layerStart,
                 uint32_t layerCount,
                 VkImageAspectFlags aspectFlags);
    bool hasImmutableSampler() const { return mYcbcrConversionDesc.valid(); }
    uint64_t getExternalFormat() const { return mYcbcrConversionDesc.getExternalFormat(); }
    bool isYuvResolve() const { return mYcbcrConversionDesc.getExternalFormat() != 0; }
    bool updateChromaFilter(Renderer *renderer, VkFilter filter)
    {
        return mYcbcrConversionDesc.updateChromaFilter(renderer, filter);
    }
    const YcbcrConversionDesc &getYcbcrConversionDesc() const { return mYcbcrConversionDesc; }
    const YcbcrConversionDesc getY2YConversionDesc() const
    {
        YcbcrConversionDesc y2yDesc = mYcbcrConversionDesc;
        y2yDesc.updateConversionModel(VK_SAMPLER_YCBCR_MODEL_CONVERSION_RGB_IDENTITY);
        return y2yDesc;
    }

    static YcbcrConversionDesc deriveConversionDesc(Context *context,
                                                    angle::FormatID actualFormatID,
                                                    angle::FormatID intendedFormatID);

    // Used by framebuffer and render pass functions to decide loadOps and invalidate/un-invalidate
    // render target contents.
    bool hasSubresourceDefinedContent(gl::LevelIndex level,
                                      uint32_t layerIndex,
                                      uint32_t layerCount) const;
    bool hasSubresourceDefinedStencilContent(gl::LevelIndex level,
                                             uint32_t layerIndex,
                                             uint32_t layerCount) const;
    void invalidateSubresourceContent(ContextVk *contextVk,
                                      gl::LevelIndex level,
                                      uint32_t layerIndex,
                                      uint32_t layerCount,
                                      bool *preferToKeepContentsDefinedOut);
    void invalidateSubresourceStencilContent(ContextVk *contextVk,
                                             gl::LevelIndex level,
                                             uint32_t layerIndex,
                                             uint32_t layerCount,
                                             bool *preferToKeepContentsDefinedOut);
    void restoreSubresourceContent(gl::LevelIndex level, uint32_t layerIndex, uint32_t layerCount);
    void restoreSubresourceStencilContent(gl::LevelIndex level,
                                          uint32_t layerIndex,
                                          uint32_t layerCount);
    angle::Result reformatStagedBufferUpdates(ContextVk *contextVk,
                                              angle::FormatID srcFormatID,
                                              angle::FormatID dstFormatID);
    bool hasStagedImageUpdatesWithMismatchedFormat(gl::LevelIndex levelStart,
                                                   gl::LevelIndex levelEnd,
                                                   angle::FormatID formatID) const;

    void setAcquireNextImageSemaphore(VkSemaphore semaphore)
    {
        ASSERT(semaphore != VK_NULL_HANDLE);
        ASSERT(!mAcquireNextImageSemaphore.valid());
        mAcquireNextImageSemaphore.setHandle(semaphore);
    }
    const Semaphore &getAcquireNextImageSemaphore() const { return mAcquireNextImageSemaphore; }
    void resetAcquireNextImageSemaphore() { mAcquireNextImageSemaphore.release(); }
    bool isBackedByExternalMemory() const
    {
        return mMemoryAllocationType == MemoryAllocationType::ImageExternal;
    }

    angle::Result initializeNonZeroMemory(Context *context,
                                          bool hasProtectedContent,
                                          VkMemoryPropertyFlags flags,
                                          VkDeviceSize size);

    size_t getLevelUpdateCount(gl::LevelIndex level) const;

    // Create event if needed and record the event in ImageHelper::mCurrentEvent.
    void setCurrentRefCountedEvent(Context *context, ImageLayoutEventMaps &layoutEventMaps);

  private:
    ANGLE_ENABLE_STRUCT_PADDING_WARNINGS
    struct ClearUpdate
    {
        bool operator==(const ClearUpdate &rhs) const
        {
            return memcmp(this, &rhs, sizeof(ClearUpdate)) == 0;
        }
        VkImageAspectFlags aspectFlags;
        VkClearValue value;
        uint32_t levelIndex;
        uint32_t layerIndex;
        uint32_t layerCount;
        // For ClearEmulatedChannelsOnly, mask of which channels to clear.
        VkColorComponentFlags colorMaskFlags;
    };
    ANGLE_DISABLE_STRUCT_PADDING_WARNINGS
    struct BufferUpdate
    {
        BufferHelper *bufferHelper;
        VkBufferImageCopy copyRegion;
        angle::FormatID formatID;
    };
    struct ImageUpdate
    {
        VkImageCopy copyRegion;
        angle::FormatID formatID;
    };

    struct SubresourceUpdate : angle::NonCopyable
    {
        SubresourceUpdate();
        ~SubresourceUpdate();
        SubresourceUpdate(RefCounted<BufferHelper> *bufferIn,
                          BufferHelper *bufferHelperIn,
                          const VkBufferImageCopy &copyRegion,
                          angle::FormatID formatID);
        SubresourceUpdate(RefCounted<ImageHelper> *imageIn,
                          const VkImageCopy &copyRegion,
                          angle::FormatID formatID);
        SubresourceUpdate(VkImageAspectFlags aspectFlags,
                          const VkClearValue &clearValue,
                          const gl::ImageIndex &imageIndex);
        SubresourceUpdate(VkImageAspectFlags aspectFlags,
                          const VkClearValue &clearValue,
                          gl::LevelIndex level,
                          uint32_t layerIndex,
                          uint32_t layerCount);
        SubresourceUpdate(VkColorComponentFlags colorMaskFlags,
                          const VkClearColorValue &clearValue,
                          const gl::ImageIndex &imageIndex);
        SubresourceUpdate(SubresourceUpdate &&other);

        SubresourceUpdate &operator=(SubresourceUpdate &&other);

        void release(Renderer *renderer);

        bool isUpdateToLayers(uint32_t layerIndex, uint32_t layerCount) const;
        void getDestSubresource(uint32_t imageLayerCount,
                                uint32_t *baseLayerOut,
                                uint32_t *layerCountOut) const;
        VkImageAspectFlags getDestAspectFlags() const;

        UpdateSource updateSource;
        union
        {
            ClearUpdate clear;
            BufferUpdate buffer;
            ImageUpdate image;
        } data;
        union
        {
            RefCounted<ImageHelper> *image;
            RefCounted<BufferHelper> *buffer;
        } refCounted;
    };

    // Up to 8 layers are tracked per level for whether contents are defined, above which the
    // contents are considered unconditionally defined.  This handles the more likely scenarios of:
    //
    // - Single layer framebuffer attachments,
    // - Cube map framebuffer attachments,
    // - Multi-view rendering.
    //
    // If there arises a need to optimize an application that invalidates layer >= 8, this can
    // easily be raised to 32 to 64 bits.  Beyond that, an additional hash map can be used to track
    // such subresources.
    static constexpr uint32_t kMaxContentDefinedLayerCount = 8;
    using LevelContentDefinedMask = angle::BitSet8<kMaxContentDefinedLayerCount>;

    void deriveExternalImageTiling(const void *createInfoChain);

    // Used to initialize ImageFormats from actual format, with no pNext from a VkImageCreateInfo
    // object.
    void setImageFormatsFromActualFormat(VkFormat actualFormat, ImageFormats &imageFormatsOut);

    // Called from flushStagedUpdates, removes updates that are later superseded by another.  This
    // cannot be done at the time the updates were staged, as the image is not created (and thus the
    // extents are not known).
    void removeSupersededUpdates(ContextVk *contextVk, const gl::TexLevelMask skipLevelsAllFaces);

    void initImageMemoryBarrierStruct(Context *context,
                                      VkImageAspectFlags aspectMask,
                                      ImageLayout newLayout,
                                      uint32_t newQueueFamilyIndex,
                                      VkImageMemoryBarrier *imageMemoryBarrier) const;

    // Generalized to accept both "primary" and "secondary" command buffers.
    template <typename CommandBufferT>
    void barrierImpl(Context *context,
                     VkImageAspectFlags aspectMask,
                     ImageLayout newLayout,
                     uint32_t newQueueFamilyIndex,
                     RefCountedEventCollector *eventCollector,
                     CommandBufferT *commandBuffer,
                     VkSemaphore *acquireNextImageSemaphoreOut);

    void setSubresourcesWrittenSinceBarrier(gl::LevelIndex levelStart,
                                            uint32_t levelCount,
                                            uint32_t layerStart,
                                            uint32_t layerCount);

    void resetSubresourcesWrittenSinceBarrier();
    bool areLevelSubresourcesWrittenWithinMaskRange(uint32_t level,
                                                    ImageLayerWriteMask &layerMask) const
    {
        return (mSubresourcesWrittenSinceBarrier[level] & layerMask) != 0;
    }

    // If the image has emulated channels, we clear them once so as not to leave garbage on those
    // channels.
    VkColorComponentFlags getEmulatedChannelsMask() const;
    void stageClearIfEmulatedFormat(bool isRobustResourceInitEnabled, bool isExternalImage);
    bool verifyEmulatedClearsAreBeforeOtherUpdates(const std::vector<SubresourceUpdate> &updates);

    // Clear either color or depth/stencil based on image format.
    void clear(Context *context,
               VkImageAspectFlags aspectFlags,
               const VkClearValue &value,
               LevelIndex mipLevel,
               uint32_t baseArrayLayer,
               uint32_t layerCount,
               OutsideRenderPassCommandBuffer *commandBuffer);

    void clearColor(Context *context,
                    const VkClearColorValue &color,
                    LevelIndex baseMipLevelVk,
                    uint32_t levelCount,
                    uint32_t baseArrayLayer,
                    uint32_t layerCount,
                    OutsideRenderPassCommandBuffer *commandBuffer);

    void clearDepthStencil(Context *context,
                           VkImageAspectFlags clearAspectFlags,
                           const VkClearDepthStencilValue &depthStencil,
                           LevelIndex baseMipLevelVk,
                           uint32_t levelCount,
                           uint32_t baseArrayLayer,
                           uint32_t layerCount,
                           OutsideRenderPassCommandBuffer *commandBuffer);

    angle::Result clearEmulatedChannels(ContextVk *contextVk,
                                        VkColorComponentFlags colorMaskFlags,
                                        const VkClearValue &value,
                                        LevelIndex mipLevel,
                                        uint32_t baseArrayLayer,
                                        uint32_t layerCount);

    angle::Result updateSubresourceOnHost(Context *context,
                                          ApplyImageUpdate applyUpdate,
                                          const gl::ImageIndex &index,
                                          const gl::Extents &glExtents,
                                          const gl::Offset &offset,
                                          const uint8_t *source,
                                          const GLuint rowPitch,
                                          const GLuint depthPitch,
                                          bool *copiedOut);

    // ClearEmulatedChannels updates are expected in the beginning of the level update list. They
    // can be processed first and removed. By doing so, if this is the only update for the image,
    // an unnecessary layout transition can be avoided.
    angle::Result flushStagedClearEmulatedChannelsUpdates(ContextVk *contextVk,
                                                          gl::LevelIndex levelGLStart,
                                                          gl::LevelIndex levelGLLimit,
                                                          bool *otherUpdatesToFlushOut);

    // Flushes staged updates to a range of levels and layers from start to end. The updates do not
    // include ClearEmulatedChannelsOnly, which are processed in a separate function.
    angle::Result flushStagedUpdatesImpl(ContextVk *contextVk,
                                         gl::LevelIndex levelGLStart,
                                         gl::LevelIndex levelGLEnd,
                                         uint32_t layerStart,
                                         uint32_t layerEnd,
                                         const gl::TexLevelMask &skipLevelsAllFaces);

    // Limit the input level to the number of levels in subresource update list.
    void clipLevelToUpdateListUpperLimit(gl::LevelIndex *level) const;

    std::vector<SubresourceUpdate> *getLevelUpdates(gl::LevelIndex level);
    const std::vector<SubresourceUpdate> *getLevelUpdates(gl::LevelIndex level) const;

    void appendSubresourceUpdate(gl::LevelIndex level, SubresourceUpdate &&update);
    void prependSubresourceUpdate(gl::LevelIndex level, SubresourceUpdate &&update);

    enum class PruneReason
    {
        MemoryOptimization,
        MinimizeWorkBeforeFlush
    };
    void pruneSupersededUpdatesForLevel(ContextVk *contextVk,
                                        const gl::LevelIndex level,
                                        const PruneReason reason);

    // Whether there are any updates in [start, end).
    bool hasStagedUpdatesInLevels(gl::LevelIndex levelStart, gl::LevelIndex levelEnd) const;

    // Used only for assertions, these functions verify that
    // SubresourceUpdate::refcountedObject::image or buffer references have the correct ref count.
    // This is to prevent accidental leaks.
    bool validateSubresourceUpdateImageRefConsistent(RefCounted<ImageHelper> *image) const;
    bool validateSubresourceUpdateBufferRefConsistent(RefCounted<BufferHelper> *buffer) const;
    bool validateSubresourceUpdateRefCountsConsistent() const;

    void resetCachedProperties();
    void setEntireContentDefined();
    void setEntireContentUndefined();
    void setContentDefined(LevelIndex levelStart,
                           uint32_t levelCount,
                           uint32_t layerStart,
                           uint32_t layerCount,
                           VkImageAspectFlags aspectFlags);
    void invalidateSubresourceContentImpl(ContextVk *contextVk,
                                          gl::LevelIndex level,
                                          uint32_t layerIndex,
                                          uint32_t layerCount,
                                          VkImageAspectFlagBits aspect,
                                          LevelContentDefinedMask *contentDefinedMask,
                                          bool *preferToKeepContentsDefinedOut);
    void restoreSubresourceContentImpl(gl::LevelIndex level,
                                       uint32_t layerIndex,
                                       uint32_t layerCount,
                                       VkImageAspectFlagBits aspect,
                                       LevelContentDefinedMask *contentDefinedMask);

    // Use the following functions to access m*ContentDefined to make sure the correct level index
    // is used (i.e. vk::LevelIndex and not gl::LevelIndex).
    LevelContentDefinedMask &getLevelContentDefined(LevelIndex level);
    LevelContentDefinedMask &getLevelStencilContentDefined(LevelIndex level);
    const LevelContentDefinedMask &getLevelContentDefined(LevelIndex level) const;
    const LevelContentDefinedMask &getLevelStencilContentDefined(LevelIndex level) const;

    angle::Result initLayerImageViewImpl(Context *context,
                                         gl::TextureType textureType,
                                         VkImageAspectFlags aspectMask,
                                         const gl::SwizzleState &swizzleMap,
                                         ImageView *imageViewOut,
                                         LevelIndex baseMipLevelVk,
                                         uint32_t levelCount,
                                         uint32_t baseArrayLayer,
                                         uint32_t layerCount,
                                         VkFormat imageFormat,
                                         VkImageUsageFlags usageFlags,
                                         gl::YuvSamplingMode yuvSamplingMode) const;

    angle::Result readPixelsImpl(ContextVk *contextVk,
                                 const gl::Rectangle &area,
                                 const PackPixelsParams &packPixelsParams,
                                 VkImageAspectFlagBits copyAspectFlags,
                                 gl::LevelIndex levelGL,
                                 uint32_t layer,
                                 void *pixels);

    angle::Result packReadPixelBuffer(ContextVk *contextVk,
                                      const gl::Rectangle &area,
                                      const PackPixelsParams &packPixelsParams,
                                      const angle::Format &readFormat,
                                      const angle::Format &aspectFormat,
                                      const uint8_t *readPixelBuffer,
                                      gl::LevelIndex levelGL,
                                      void *pixels);

    bool canCopyWithTransformForReadPixels(const PackPixelsParams &packPixelsParams,
                                           const angle::Format *readFormat,
                                           ptrdiff_t pixelsOffset);
    bool canCopyWithComputeForReadPixels(const PackPixelsParams &packPixelsParams,
                                         const angle::Format *readFormat,
                                         ptrdiff_t pixelsOffset);

    // Returns true if source data and actual image format matches except color space differences.
    bool isDataFormatMatchForCopy(angle::FormatID srcDataFormatID) const
    {
        if (mActualFormatID == srcDataFormatID)
        {
            return true;
        }
        angle::FormatID actualFormatLinear =
            getActualFormat().isSRGB ? ConvertToLinear(mActualFormatID) : mActualFormatID;
        angle::FormatID srcDataFormatIDLinear = angle::Format::Get(srcDataFormatID).isSRGB
                                                    ? ConvertToLinear(srcDataFormatID)
                                                    : srcDataFormatID;
        return actualFormatLinear == srcDataFormatIDLinear;
    }

    static constexpr int kThreadholdForComputeTransCoding = 4096;
    bool shouldUseComputeForTransCoding(LevelIndex level)
    {
        // Using texture size instead of extent size to simplify the problem.
        gl::Extents ext = getLevelExtents2D(level);
        return ext.width * ext.height > kThreadholdForComputeTransCoding;
    }

    // Vulkan objects.
    Image mImage;
    DeviceMemory mDeviceMemory;
    Allocation mVmaAllocation;

    // Image properties.
    VkImageCreateInfo mVkImageCreateInfo;
    VkImageType mImageType;
    VkImageTiling mTilingMode;
    VkImageCreateFlags mCreateFlags;
    VkImageUsageFlags mUsage;
    // For Android swapchain images, the Vulkan VkImage must be "rotated".  However, most of ANGLE
    // uses non-rotated extents (i.e. the way the application views the extents--see "Introduction
    // to Android rotation and pre-rotation" in "SurfaceVk.cpp").  Thus, mExtents are non-rotated.
    // The rotated extents are also stored along with a bool that indicates if the aspect ratio is
    // different between the rotated and non-rotated extents.
    VkExtent3D mExtents;
    bool mRotatedAspectRatio;
    angle::FormatID mIntendedFormatID;
    angle::FormatID mActualFormatID;
    GLint mSamples;
    ImageSerial mImageSerial;

    // Current state.
    ImageLayout mCurrentLayout;
    uint32_t mCurrentQueueFamilyIndex;
    // For optimizing transition between different shader readonly layouts
    ImageLayout mLastNonShaderReadOnlyLayout;
    VkPipelineStageFlags mCurrentShaderReadStageMask;
    // Track how it is being used by current open renderpass.
    RenderPassUsageFlags mRenderPassUsageFlags;
    // The QueueSerial that associated with the last barrier.
    QueueSerial mBarrierQueueSerial;

    // The current refCounted event. When barrier or layout change is needed, we should wait for
    // this event.
    RefCountedEvent mCurrentEvent;
    RefCountedEvent mLastNonShaderReadOnlyEvent;

    // Whether ANGLE currently has ownership of this resource or it's released to external.
    bool mIsReleasedToExternal;

    // For imported images
    YcbcrConversionDesc mYcbcrConversionDesc;

    // The first level that has been allocated. For mutable textures, this should be same as
    // mBaseLevel since we always reallocate VkImage based on mBaseLevel change. But for immutable
    // textures, we always allocate from level 0 regardless of mBaseLevel change.
    gl::LevelIndex mFirstAllocatedLevel;

    // Cached properties.
    uint32_t mLayerCount;
    uint32_t mLevelCount;

    // Image formats used for imageless framebuffers.
    ImageFormats mViewFormats;

    std::vector<std::vector<SubresourceUpdate>> mSubresourceUpdates;
    VkDeviceSize mTotalStagedBufferUpdateSize;

    // Optimization for repeated clear with the same value. If this pointer is not null, the entire
    // image it has been cleared to the specified clear value. If another clear call is made with
    // the exact same clear value, we will detect and skip the clear call.
    Optional<ClearUpdate> mCurrentSingleClearValue;

    // Track whether each subresource has defined contents.  Up to 8 layers are tracked per level,
    // above which the contents are considered unconditionally defined.
    gl::TexLevelArray<LevelContentDefinedMask> mContentDefined;
    gl::TexLevelArray<LevelContentDefinedMask> mStencilContentDefined;

    // Used for memory allocation tracking.
    // Memory size allocated for the image in the memory during the initialization.
    VkDeviceSize mAllocationSize;
    // Type of the memory allocation for the image (Image or ImageExternal).
    MemoryAllocationType mMemoryAllocationType;
    // Memory type index used for the allocation. It can be used to determine the heap index.
    uint32_t mMemoryTypeIndex;

    // Only used for swapChain images. This is set when an image is acquired and is waited on
    // by the next submission (which uses this image), at which point it is released.
    Semaphore mAcquireNextImageSemaphore;

    // Used to track subresource writes per level/layer. This can help parallelize writes to
    // different levels or layers of the image, such as data uploads.
    // See comment on kMaxParallelLayerWrites.
    gl::TexLevelArray<ImageLayerWriteMask> mSubresourcesWrittenSinceBarrier;
};

ANGLE_INLINE bool RenderPassCommandBufferHelper::usesImage(const ImageHelper &image) const
{
    return image.usedByCommandBuffer(mQueueSerial);
}

ANGLE_INLINE bool RenderPassCommandBufferHelper::startedAndUsesImageWithBarrier(
    const ImageHelper &image) const
{
    return mRenderPassStarted && image.getBarrierQueueSerial() == mQueueSerial;
}

// A vector of image views, such as one per level or one per layer.
using ImageViewVector = std::vector<ImageView>;

// A vector of vector of image views.  Primary index is layer, secondary index is level.
using LayerLevelImageViewVector = std::vector<ImageViewVector>;

// Address mode for layers: only possible to access either all layers, or up to
// IMPLEMENTATION_ANGLE_MULTIVIEW_MAX_VIEWS layers.  This enum uses 0 for all layers and the rest of
// the values conveniently alias the number of layers.
enum LayerMode
{
    All,
    _1,
    _2,
    _3,
    _4,
};
static_assert(gl::IMPLEMENTATION_ANGLE_MULTIVIEW_MAX_VIEWS == 4, "Update LayerMode");

LayerMode GetLayerMode(const vk::ImageHelper &image, uint32_t layerCount);

// Sampler decode mode indicating if an attachment needs to be decoded in linear colorspace or sRGB
enum class SrgbDecodeMode
{
    SkipDecode,
    SrgbDecode
};

class ImageViewHelper final : angle::NonCopyable
{
  public:
    ImageViewHelper();
    ImageViewHelper(ImageViewHelper &&other);
    ~ImageViewHelper();

    void init(Renderer *renderer);
    void destroy(VkDevice device);

    const ImageView &getLinearReadImageView() const
    {
        return getValidReadViewImpl(mPerLevelRangeLinearReadImageViews);
    }
    const ImageView &getSRGBReadImageView() const
    {
        return getValidReadViewImpl(mPerLevelRangeSRGBReadImageViews);
    }
    const ImageView &getLinearFetchImageView() const
    {
        return getValidReadViewImpl(mPerLevelRangeLinearFetchImageViews);
    }
    const ImageView &getSRGBFetchImageView() const
    {
        return getValidReadViewImpl(mPerLevelRangeSRGBFetchImageViews);
    }
    const ImageView &getLinearCopyImageView() const
    {
        return getValidReadViewImpl(mPerLevelRangeLinearCopyImageViews);
    }
    const ImageView &getSRGBCopyImageView() const
    {
        return getValidReadViewImpl(mPerLevelRangeSRGBCopyImageViews);
    }
    const ImageView &getStencilReadImageView() const
    {
        return getValidReadViewImpl(mPerLevelRangeStencilReadImageViews);
    }

    const ImageView &getReadImageView() const
    {
        return mLinearColorspace ? getReadViewImpl(mPerLevelRangeLinearReadImageViews)
                                 : getReadViewImpl(mPerLevelRangeSRGBReadImageViews);
    }

    const ImageView &getFetchImageView() const
    {
        return mLinearColorspace ? getReadViewImpl(mPerLevelRangeLinearFetchImageViews)
                                 : getReadViewImpl(mPerLevelRangeSRGBFetchImageViews);
    }

    const ImageView &getCopyImageView() const
    {
        return mLinearColorspace ? getReadViewImpl(mPerLevelRangeLinearCopyImageViews)
                                 : getReadViewImpl(mPerLevelRangeSRGBCopyImageViews);
    }

    ImageView &getSamplerExternal2DY2YEXTImageView()
    {
        return getReadViewImpl(mPerLevelRangeSamplerExternal2DY2YEXTImageViews);
    }

    const ImageView &getSamplerExternal2DY2YEXTImageView() const
    {
        return getValidReadViewImpl(mPerLevelRangeSamplerExternal2DY2YEXTImageViews);
    }

    const ImageView &getFragmentShadingRateImageView() const
    {
        return mFragmentShadingRateImageView;
    }

    // Used when initialized RenderTargets.
    bool hasStencilReadImageView() const
    {
        return mCurrentBaseMaxLevelHash < mPerLevelRangeStencilReadImageViews.size()
                   ? mPerLevelRangeStencilReadImageViews[mCurrentBaseMaxLevelHash].valid()
                   : false;
    }

    bool hasFetchImageView() const
    {
        if ((mLinearColorspace &&
             mCurrentBaseMaxLevelHash < mPerLevelRangeLinearFetchImageViews.size()) ||
            (!mLinearColorspace &&
             mCurrentBaseMaxLevelHash < mPerLevelRangeSRGBFetchImageViews.size()))
        {
            return getFetchImageView().valid();
        }
        else
        {
            return false;
        }
    }

    bool hasCopyImageView() const
    {
        if ((mLinearColorspace &&
             mCurrentBaseMaxLevelHash < mPerLevelRangeLinearCopyImageViews.size()) ||
            (!mLinearColorspace &&
             mCurrentBaseMaxLevelHash < mPerLevelRangeSRGBCopyImageViews.size()))
        {
            return getCopyImageView().valid();
        }
        else
        {
            return false;
        }
    }

    // For applications that frequently switch a texture's max level, and make no other changes to
    // the texture, change the currently-used max level, and potentially create new "read views"
    // for the new max-level
    angle::Result initReadViews(ContextVk *contextVk,
                                gl::TextureType viewType,
                                const ImageHelper &image,
                                const gl::SwizzleState &formatSwizzle,
                                const gl::SwizzleState &readSwizzle,
                                LevelIndex baseLevel,
                                uint32_t levelCount,
                                uint32_t baseLayer,
                                uint32_t layerCount,
                                bool requiresSRGBViews,
                                VkImageUsageFlags imageUsageFlags);

    // Creates a storage view with all layers of the level.
    angle::Result getLevelStorageImageView(Context *context,
                                           gl::TextureType viewType,
                                           const ImageHelper &image,
                                           LevelIndex levelVk,
                                           uint32_t layer,
                                           VkImageUsageFlags imageUsageFlags,
                                           angle::FormatID formatID,
                                           const ImageView **imageViewOut);

    // Creates a storage view with a single layer of the level.
    angle::Result getLevelLayerStorageImageView(Context *context,
                                                const ImageHelper &image,
                                                LevelIndex levelVk,
                                                uint32_t layer,
                                                VkImageUsageFlags imageUsageFlags,
                                                angle::FormatID formatID,
                                                const ImageView **imageViewOut);

    // Creates a draw view with a range of layers of the level.
    angle::Result getLevelDrawImageView(Context *context,
                                        const ImageHelper &image,
                                        LevelIndex levelVk,
                                        uint32_t layer,
                                        uint32_t layerCount,
                                        gl::SrgbWriteControlMode mode,
                                        const ImageView **imageViewOut);

    // Creates a draw view with a single layer of the level.
    angle::Result getLevelLayerDrawImageView(Context *context,
                                             const ImageHelper &image,
                                             LevelIndex levelVk,
                                             uint32_t layer,
                                             gl::SrgbWriteControlMode mode,
                                             const ImageView **imageViewOut);

    // Creates a fragment shading rate view.
    angle::Result initFragmentShadingRateView(ContextVk *contextVk, ImageHelper *image);

    // Return unique Serial for an imageView.
    ImageOrBufferViewSubresourceSerial getSubresourceSerial(
        gl::LevelIndex levelGL,
        uint32_t levelCount,
        uint32_t layer,
        LayerMode layerMode,
        SrgbDecodeMode srgbDecodeMode,
        gl::SrgbOverride srgbOverrideMode) const;

    bool isImageViewGarbageEmpty() const;

    void release(Renderer *renderer, const ResourceUse &use);

  private:
    ImageView &getReadImageView()
    {
        return mLinearColorspace ? getReadViewImpl(mPerLevelRangeLinearReadImageViews)
                                 : getReadViewImpl(mPerLevelRangeSRGBReadImageViews);
    }
    ImageView &getFetchImageView()
    {
        return mLinearColorspace ? getReadViewImpl(mPerLevelRangeLinearFetchImageViews)
                                 : getReadViewImpl(mPerLevelRangeSRGBFetchImageViews);
    }
    ImageView &getCopyImageView()
    {
        return mLinearColorspace ? getReadViewImpl(mPerLevelRangeLinearCopyImageViews)
                                 : getReadViewImpl(mPerLevelRangeSRGBCopyImageViews);
    }

    // Used by public get*ImageView() methods to do proper assert based on vector size and validity
    inline const ImageView &getValidReadViewImpl(const ImageViewVector &imageViewVector) const
    {
        ASSERT(mCurrentBaseMaxLevelHash < imageViewVector.size() &&
               imageViewVector[mCurrentBaseMaxLevelHash].valid());
        return imageViewVector[mCurrentBaseMaxLevelHash];
    }

    // Used by public get*ImageView() methods to do proper assert based on vector size
    inline const ImageView &getReadViewImpl(const ImageViewVector &imageViewVector) const
    {
        ASSERT(mCurrentBaseMaxLevelHash < imageViewVector.size());
        return imageViewVector[mCurrentBaseMaxLevelHash];
    }

    // Used by private get*ImageView() methods to do proper assert based on vector size
    inline ImageView &getReadViewImpl(ImageViewVector &imageViewVector)
    {
        ASSERT(mCurrentBaseMaxLevelHash < imageViewVector.size());
        return imageViewVector[mCurrentBaseMaxLevelHash];
    }

    // Creates views with multiple layers and levels.
    angle::Result initReadViewsImpl(ContextVk *contextVk,
                                    gl::TextureType viewType,
                                    const ImageHelper &image,
                                    const gl::SwizzleState &formatSwizzle,
                                    const gl::SwizzleState &readSwizzle,
                                    LevelIndex baseLevel,
                                    uint32_t levelCount,
                                    uint32_t baseLayer,
                                    uint32_t layerCount,
                                    VkImageUsageFlags imageUsageFlags);

    // Create SRGB-reinterpreted read views
    angle::Result initSRGBReadViewsImpl(ContextVk *contextVk,
                                        gl::TextureType viewType,
                                        const ImageHelper &image,
                                        const gl::SwizzleState &formatSwizzle,
                                        const gl::SwizzleState &readSwizzle,
                                        LevelIndex baseLevel,
                                        uint32_t levelCount,
                                        uint32_t baseLayer,
                                        uint32_t layerCount,
                                        VkImageUsageFlags imageUsageFlags);

    // For applications that frequently switch a texture's base/max level, and make no other changes
    // to the texture, keep track of the currently-used base and max levels, and keep one "read
    // view" per each combination.  The value stored here is base<<4|max, used to look up the view
    // in a vector.
    static_assert(gl::IMPLEMENTATION_MAX_TEXTURE_LEVELS <= 16,
                  "Not enough bits in mCurrentBaseMaxLevelHash");
    uint8_t mCurrentBaseMaxLevelHash;

    bool mLinearColorspace;

    // Read views (one per [base, max] level range)
    ImageViewVector mPerLevelRangeLinearReadImageViews;
    ImageViewVector mPerLevelRangeSRGBReadImageViews;
    ImageViewVector mPerLevelRangeLinearFetchImageViews;
    ImageViewVector mPerLevelRangeSRGBFetchImageViews;
    ImageViewVector mPerLevelRangeLinearCopyImageViews;
    ImageViewVector mPerLevelRangeSRGBCopyImageViews;
    ImageViewVector mPerLevelRangeStencilReadImageViews;
    ImageViewVector mPerLevelRangeSamplerExternal2DY2YEXTImageViews;

    // Draw views
    LayerLevelImageViewVector mLayerLevelDrawImageViews;
    LayerLevelImageViewVector mLayerLevelDrawImageViewsLinear;
    angle::HashMap<ImageSubresourceRange, std::unique_ptr<ImageView>> mSubresourceDrawImageViews;

    // Storage views
    ImageViewVector mLevelStorageImageViews;
    LayerLevelImageViewVector mLayerLevelStorageImageViews;

    // Fragment shading rate view
    ImageView mFragmentShadingRateImageView;

    // Serial for the image view set. getSubresourceSerial combines it with subresource info.
    ImageOrBufferViewSerial mImageViewSerial;
};

ImageSubresourceRange MakeImageSubresourceReadRange(gl::LevelIndex level,
                                                    uint32_t levelCount,
                                                    uint32_t layer,
                                                    LayerMode layerMode,
                                                    SrgbDecodeMode srgbDecodeMode,
                                                    gl::SrgbOverride srgbOverrideMode);
ImageSubresourceRange MakeImageSubresourceDrawRange(gl::LevelIndex level,
                                                    uint32_t layer,
                                                    LayerMode layerMode,
                                                    gl::SrgbWriteControlMode srgbWriteControlMode);

class BufferViewHelper final : public Resource
{
  public:
    BufferViewHelper();
    BufferViewHelper(BufferViewHelper &&other);
    ~BufferViewHelper() override;

    void init(Renderer *renderer, VkDeviceSize offset, VkDeviceSize size);
    bool isInitialized() const { return mInitialized; }
    void release(ContextVk *contextVk);
    void destroy(VkDevice device);

    angle::Result getView(Context *context,
                          const BufferHelper &buffer,
                          VkDeviceSize bufferOffset,
                          const Format &format,
                          const BufferView **viewOut);

    // Return unique Serial for a bufferView.
    ImageOrBufferViewSubresourceSerial getSerial() const;

  private:
    bool mInitialized;

    // To support format reinterpretation, additional views for formats other than the one specified
    // to glTexBuffer may need to be created.  On draw/dispatch, the format layout qualifier of the
    // imageBuffer is used (if provided) to create a potentially different view of the buffer.
    angle::HashMap<VkFormat, BufferView> mViews;

    // View properties:
    //
    // Offset and size specified to glTexBufferRange
    VkDeviceSize mOffset;
    VkDeviceSize mSize;

    // Serial for the buffer view.  An ImageOrBufferViewSerial is used for texture buffers so that
    // they fit together with the other texture types.
    ImageOrBufferViewSerial mViewSerial;
};

// Context state that can affect a compute pipeline
enum class ComputePipelineFlag : uint8_t
{
    // Whether VK_EXT_pipeline_robustness should be used to make the pipeline robust.  Note that
    // programs are allowed to be shared between robust and non-robust contexts, so different
    // pipelines can be created for the same compute program.
    Robust,
    // Whether VK_EXT_pipeline_protected_access should be used to make the pipeline protected-only.
    // Similar to robustness, EGL allows protected and unprotected to be in the same share group.
    Protected,

    InvalidEnum,
    EnumCount = InvalidEnum,
};

using ComputePipelineFlags = angle::PackedEnumBitSet<ComputePipelineFlag, uint8_t>;
using ComputePipelineCache = std::array<PipelineHelper, 1u << ComputePipelineFlags::size()>;

class ShaderProgramHelper : angle::NonCopyable
{
  public:
    ShaderProgramHelper();
    ~ShaderProgramHelper();

    bool valid(const gl::ShaderType shaderType) const;
    void destroy(Renderer *renderer);
    void release(ContextVk *contextVk);

    void setShader(gl::ShaderType shaderType, RefCounted<ShaderModule> *shader);

    // Create a graphics pipeline and place it in the cache.  Must not be called if the pipeline
    // exists in cache.
    template <typename PipelineHash>
    ANGLE_INLINE angle::Result createGraphicsPipeline(
        vk::Context *context,
        GraphicsPipelineCache<PipelineHash> *graphicsPipelines,
        PipelineCacheAccess *pipelineCache,
        const RenderPass &compatibleRenderPass,
        const PipelineLayout &pipelineLayout,
        PipelineSource source,
        const GraphicsPipelineDesc &pipelineDesc,
        const SpecializationConstants &specConsts,
        const GraphicsPipelineDesc **descPtrOut,
        PipelineHelper **pipelineOut) const
    {
        return graphicsPipelines->createPipeline(context, pipelineCache, compatibleRenderPass,
                                                 pipelineLayout, mShaders, specConsts, source,
                                                 pipelineDesc, descPtrOut, pipelineOut);
    }

    void createMonolithicPipelineCreationTask(vk::Context *context,
                                              PipelineCacheAccess *pipelineCache,
                                              const GraphicsPipelineDesc &desc,
                                              const PipelineLayout &pipelineLayout,
                                              const SpecializationConstants &specConsts,
                                              PipelineHelper *pipeline) const;

    angle::Result getOrCreateComputePipeline(vk::Context *context,
                                             ComputePipelineCache *computePipelines,
                                             PipelineCacheAccess *pipelineCache,
                                             const PipelineLayout &pipelineLayout,
                                             ComputePipelineFlags pipelineFlags,
                                             PipelineSource source,
                                             PipelineHelper **pipelineOut,
                                             const char *shaderName,
                                             VkSpecializationInfo *specializationInfo) const;

  private:
    ShaderModuleMap mShaders;
};

// Tracks current handle allocation counts in the back-end. Useful for debugging and profiling.
// Note: not all handle types are currently implemented.
class ActiveHandleCounter final : angle::NonCopyable
{
  public:
    ActiveHandleCounter();
    ~ActiveHandleCounter();

    void onAllocate(HandleType handleType)
    {
        mActiveCounts[handleType]++;
        mAllocatedCounts[handleType]++;
    }

    void onDeallocate(HandleType handleType) { mActiveCounts[handleType]--; }

    uint32_t getActive(HandleType handleType) const { return mActiveCounts[handleType]; }
    uint32_t getAllocated(HandleType handleType) const { return mAllocatedCounts[handleType]; }

  private:
    angle::PackedEnumMap<HandleType, uint32_t> mActiveCounts;
    angle::PackedEnumMap<HandleType, uint32_t> mAllocatedCounts;
};

// Sometimes ANGLE issues a command internally, such as copies, draws and dispatches that do not
// directly correspond to the application draw/dispatch call.  Before the command is recorded in the
// command buffer, the render pass may need to be broken and/or appropriate barriers may need to be
// inserted.  The following struct aggregates all resources that such internal commands need.
struct CommandBufferBufferAccess
{
    BufferHelper *buffer;
    VkAccessFlags accessType;
    PipelineStage stage;
};
struct CommandBufferImageAccess
{
    ImageHelper *image;
    VkImageAspectFlags aspectFlags;
    ImageLayout imageLayout;
};
struct CommandBufferImageSubresourceAccess
{
    CommandBufferImageAccess access;
    gl::LevelIndex levelStart;
    uint32_t levelCount;
    uint32_t layerStart;
    uint32_t layerCount;
};
struct CommandBufferBufferExternalAcquireRelease
{
    BufferHelper *buffer;
};
struct CommandBufferResourceAccess
{
    Resource *resource;
};
class CommandBufferAccess : angle::NonCopyable
{
  public:
    CommandBufferAccess();
    ~CommandBufferAccess();

    void onBufferTransferRead(BufferHelper *buffer)
    {
        onBufferRead(VK_ACCESS_TRANSFER_READ_BIT, PipelineStage::Transfer, buffer);
    }
    void onBufferTransferWrite(BufferHelper *buffer)
    {
        onBufferWrite(VK_ACCESS_TRANSFER_WRITE_BIT, PipelineStage::Transfer, buffer);
    }
    void onBufferSelfCopy(BufferHelper *buffer)
    {
        onBufferWrite(VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT,
                      PipelineStage::Transfer, buffer);
    }
    void onBufferComputeShaderRead(BufferHelper *buffer)
    {
        onBufferRead(VK_ACCESS_SHADER_READ_BIT, PipelineStage::ComputeShader, buffer);
    }
    void onBufferComputeShaderWrite(BufferHelper *buffer)
    {
        onBufferWrite(VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT,
                      PipelineStage::ComputeShader, buffer);
    }

    void onImageTransferRead(VkImageAspectFlags aspectFlags, ImageHelper *image)
    {
        onImageRead(aspectFlags, ImageLayout::TransferSrc, image);
    }
    void onImageTransferWrite(gl::LevelIndex levelStart,
                              uint32_t levelCount,
                              uint32_t layerStart,
                              uint32_t layerCount,
                              VkImageAspectFlags aspectFlags,
                              ImageHelper *image)
    {
        onImageWrite(levelStart, levelCount, layerStart, layerCount, aspectFlags,
                     ImageLayout::TransferDst, image);
    }
    void onImageSelfCopy(gl::LevelIndex readLevelStart,
                         uint32_t readLevelCount,
                         uint32_t readLayerStart,
                         uint32_t readLayerCount,
                         gl::LevelIndex writeLevelStart,
                         uint32_t writeLevelCount,
                         uint32_t writeLayerStart,
                         uint32_t writeLayerCount,
                         VkImageAspectFlags aspectFlags,
                         ImageHelper *image)
    {
        onImageReadSubresources(readLevelStart, readLevelCount, readLayerStart, readLayerCount,
                                aspectFlags, ImageLayout::TransferSrcDst, image);
        onImageWrite(writeLevelStart, writeLevelCount, writeLayerStart, writeLayerCount,
                     aspectFlags, ImageLayout::TransferSrcDst, image);
    }
    void onImageComputeShaderRead(VkImageAspectFlags aspectFlags, ImageHelper *image)
    {
        onImageRead(aspectFlags, ImageLayout::ComputeShaderReadOnly, image);
    }
    void onImageComputeMipmapGenerationRead(gl::LevelIndex levelStart,
                                            uint32_t levelCount,
                                            uint32_t layerStart,
                                            uint32_t layerCount,
                                            VkImageAspectFlags aspectFlags,
                                            ImageHelper *image)
    {
        onImageReadSubresources(levelStart, levelCount, layerStart, layerCount, aspectFlags,
                                ImageLayout::ComputeShaderWrite, image);
    }
    void onImageComputeShaderWrite(gl::LevelIndex levelStart,
                                   uint32_t levelCount,
                                   uint32_t layerStart,
                                   uint32_t layerCount,
                                   VkImageAspectFlags aspectFlags,
                                   ImageHelper *image)
    {
        onImageWrite(levelStart, levelCount, layerStart, layerCount, aspectFlags,
                     ImageLayout::ComputeShaderWrite, image);
    }
    void onImageTransferDstAndComputeWrite(gl::LevelIndex levelStart,
                                           uint32_t levelCount,
                                           uint32_t layerStart,
                                           uint32_t layerCount,
                                           VkImageAspectFlags aspectFlags,
                                           ImageHelper *image)
    {
        onImageWrite(levelStart, levelCount, layerStart, layerCount, aspectFlags,
                     ImageLayout::TransferDstAndComputeWrite, image);
    }
    void onExternalAcquireRelease(ImageHelper *image) { onResourceAccess(image); }
    void onQueryAccess(QueryHelper *query) { onResourceAccess(query); }
    void onBufferExternalAcquireRelease(BufferHelper *buffer);

    // The limits reflect the current maximum concurrent usage of each resource type.  ASSERTs will
    // fire if this limit is exceeded in the future.
    using ReadBuffers           = angle::FixedVector<CommandBufferBufferAccess, 2>;
    using WriteBuffers          = angle::FixedVector<CommandBufferBufferAccess, 2>;
    using ReadImages            = angle::FixedVector<CommandBufferImageAccess, 2>;
    using WriteImages           = angle::FixedVector<CommandBufferImageSubresourceAccess, 1>;
    using ReadImageSubresources = angle::FixedVector<CommandBufferImageSubresourceAccess, 1>;

    using ExternalAcquireReleaseBuffers =
        angle::FixedVector<CommandBufferBufferExternalAcquireRelease, 1>;
    using AccessResources = angle::FixedVector<CommandBufferResourceAccess, 1>;

    const ReadBuffers &getReadBuffers() const { return mReadBuffers; }
    const WriteBuffers &getWriteBuffers() const { return mWriteBuffers; }
    const ReadImages &getReadImages() const { return mReadImages; }
    const WriteImages &getWriteImages() const { return mWriteImages; }
    const ReadImageSubresources &getReadImageSubresources() const { return mReadImageSubresources; }
    const ExternalAcquireReleaseBuffers &getExternalAcquireReleaseBuffers() const
    {
        return mExternalAcquireReleaseBuffers;
    }
    const AccessResources &getAccessResources() const { return mAccessResources; }

  private:
    void onBufferRead(VkAccessFlags readAccessType, PipelineStage readStage, BufferHelper *buffer);
    void onBufferWrite(VkAccessFlags writeAccessType,
                       PipelineStage writeStage,
                       BufferHelper *buffer);

    void onImageRead(VkImageAspectFlags aspectFlags, ImageLayout imageLayout, ImageHelper *image);
    void onImageWrite(gl::LevelIndex levelStart,
                      uint32_t levelCount,
                      uint32_t layerStart,
                      uint32_t layerCount,
                      VkImageAspectFlags aspectFlags,
                      ImageLayout imageLayout,
                      ImageHelper *image);

    void onImageReadSubresources(gl::LevelIndex levelStart,
                                 uint32_t levelCount,
                                 uint32_t layerStart,
                                 uint32_t layerCount,
                                 VkImageAspectFlags aspectFlags,
                                 ImageLayout imageLayout,
                                 ImageHelper *image);

    void onResourceAccess(Resource *resource);

    ReadBuffers mReadBuffers;
    WriteBuffers mWriteBuffers;
    ReadImages mReadImages;
    WriteImages mWriteImages;
    ReadImageSubresources mReadImageSubresources;
    ExternalAcquireReleaseBuffers mExternalAcquireReleaseBuffers;
    AccessResources mAccessResources;
};

// This class' responsibility is to create index buffers needed to support line loops in Vulkan.
// In the setup phase of drawing, the createIndexBuffer method should be called with the
// current draw call parameters. If an element array buffer is bound for an indexed draw, use
// createIndexBufferFromElementArrayBuffer.
//
// If the user wants to draw a loop between [v1, v2, v3], we will create an indexed buffer with
// these indexes: [0, 1, 2, 3, 0] to emulate the loop.
class LineLoopHelper final : angle::NonCopyable
{
  public:
    LineLoopHelper(Renderer *renderer);
    ~LineLoopHelper();

    angle::Result getIndexBufferForDrawArrays(ContextVk *contextVk,
                                              uint32_t clampedVertexCount,
                                              GLint firstVertex,
                                              BufferHelper **bufferOut);

    angle::Result getIndexBufferForElementArrayBuffer(ContextVk *contextVk,
                                                      BufferVk *elementArrayBufferVk,
                                                      gl::DrawElementsType glIndexType,
                                                      int indexCount,
                                                      intptr_t elementArrayOffset,
                                                      BufferHelper **bufferOut,
                                                      uint32_t *indexCountOut);

    angle::Result streamIndices(ContextVk *contextVk,
                                gl::DrawElementsType glIndexType,
                                GLsizei indexCount,
                                const uint8_t *srcPtr,
                                BufferHelper **bufferOut,
                                uint32_t *indexCountOut);

    angle::Result streamIndicesIndirect(ContextVk *contextVk,
                                        gl::DrawElementsType glIndexType,
                                        BufferHelper *indexBuffer,
                                        BufferHelper *indirectBuffer,
                                        VkDeviceSize indirectBufferOffset,
                                        BufferHelper **indexBufferOut,
                                        BufferHelper **indirectBufferOut);

    angle::Result streamArrayIndirect(ContextVk *contextVk,
                                      size_t vertexCount,
                                      BufferHelper *arrayIndirectBuffer,
                                      VkDeviceSize arrayIndirectBufferOffset,
                                      BufferHelper **indexBufferOut,
                                      BufferHelper **indexIndirectBufferOut);

    void release(ContextVk *contextVk);
    void destroy(Renderer *renderer);

    static void Draw(uint32_t count, uint32_t baseVertex, RenderPassCommandBuffer *commandBuffer);

  private:
    BufferHelper mDynamicIndexBuffer;
    BufferHelper mDynamicIndirectBuffer;
};

enum class PresentMode
{
    ImmediateKHR               = VK_PRESENT_MODE_IMMEDIATE_KHR,
    MailboxKHR                 = VK_PRESENT_MODE_MAILBOX_KHR,
    FifoKHR                    = VK_PRESENT_MODE_FIFO_KHR,
    FifoRelaxedKHR             = VK_PRESENT_MODE_FIFO_RELAXED_KHR,
    SharedDemandRefreshKHR     = VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR,
    SharedContinuousRefreshKHR = VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR,

    InvalidEnum,
    EnumCount = InvalidEnum,
};

VkPresentModeKHR ConvertPresentModeToVkPresentMode(PresentMode presentMode);
PresentMode ConvertVkPresentModeToPresentMode(VkPresentModeKHR vkPresentMode);

}  // namespace vk
}  // namespace rx

#endif  // LIBANGLE_RENDERER_VULKAN_VK_HELPERS_H_
