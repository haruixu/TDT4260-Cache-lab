#ifndef __MEM_CACHE_PREFETCH_TDT_PREFETCHER_HH__
#define __MEM_CACHE_PREFETCH_TDT_PREFETCHER_HH__

#include <string>
#include <unordered_map>
#include <vector>

#include "base/cache/associative_cache.hh"
#include "base/sat_counter.hh"
#include "base/types.hh"
#include "mem/cache/prefetch/queued.hh"
#include "mem/cache/replacement_policies/replaceable_entry.hh"
#include "mem/cache/tags/indexing_policies/set_associative.hh"
#include "mem/cache/tags/tagged_entry.hh"
#include "mem/packet.hh"
#include "params/TDTPrefetcherHashedSetAssociative.hh"

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(ReplacementPolicy, replacement_policy);
namespace replacement_policy
{
class Base;
}

struct NextLinePrefetcherParams;

GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{

class NextLinePrefetcher : public Queued
{

  protected:
    const struct PCTableInfo {
        const int assoc;
        const int numEntries;

        TaggedIndexingPolicy *const indexingPolicy;
        replacement_policy::Base *const replacementPolicy;

        PCTableInfo(int assoc, int num_entries,
                    TaggedIndexingPolicy *indexing_policy,
                    replacement_policy::Base *repl_policy)
            : assoc(assoc), numEntries(num_entries),
              indexingPolicy(indexing_policy), replacementPolicy(repl_policy)
        {
        }
    } pcTableInfo;

    // A basic entry you can use for designing your prefetcher
    // You can extend this type of entry with more data like cycle accessed
    // Recommend to store this in a a map and look through to find matching
    // entries

    void notifyFill(const CacheAccessProbeArg &arg) override;

  public:
    NextLinePrefetcher(const NextLinePrefetcherParams &p);

    void calculatePrefetch(const PrefetchInfo &pf1,
                           std::vector<AddrPriority> &addresses,
                           const CacheAccessor &cache) override;
};

} // namespace prefetch
} // namespace gem5

#endif //_MEM_CACHE_PREFETCH_TDT_PREFETCHER_HH__
