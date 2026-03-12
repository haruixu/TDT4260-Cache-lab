#include "mem/cache/prefetch/next_line.hh"

#include "debug/HWPrefetch.hh"
#include "mem/cache/replacement_policies/base.hh"
#include "params/NextLinePrefetcher.hh"

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{

NextLinePrefetcher::NextLinePrefetcher(const NextLinePrefetcherParams &params)
    : Queued(params),
      pcTableInfo(params.table_assoc, params.table_entries,
                  params.table_indexing_policy, params.table_replacement_policy)
{
}

void NextLinePrefetcher::notifyFill(const CacheAccessProbeArg &arg)
{
    // A cache line has been filled in
}

void NextLinePrefetcher::calculatePrefetch(const PrefetchInfo &pfi,
                                           std::vector<AddrPriority> &addresses,
                                           const CacheAccessor &cache)
{
    if (!pfi.hasPC()) {
        DPRINTF(HWPrefetch, "Ignoring request with no PC.\n");
        return;
    }

    // access_addr is the memory address (of the cache line) requested
    Addr access_addr = pfi.getAddr();

    // Currently implemented prefetching algorithm: Next line prefetching
    // TODO: Implement something better!
    addresses.push_back(AddrPriority(access_addr + blkSize, 0));
}

} // namespace prefetch
} // namespace gem5
