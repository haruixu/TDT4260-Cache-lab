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

struct TDTPrefetcherParams;

GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{

class TDTPrefetcherHashedSetAssociative : public TaggedSetAssociative
{
  protected:
    uint32_t extractSet(const KeyType &key) const override;
    Addr extractTag(const Addr addr) const override;

  public:
    TDTPrefetcherHashedSetAssociative(
        const TDTPrefetcherHashedSetAssociativeParams &p)
        : TaggedSetAssociative(p)
    {
    }

    ~TDTPrefetcherHashedSetAssociative() = default;
};

class TDTPrefetcher : public Queued
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
    struct TDTEntry : public TaggedEntry {
        TDTEntry(TagExtractor ext);
        void invalidate() override;

        Addr lastAddr = 0;
    };

    // This redefines an associative set as the PC Table, a Set can be indexed
    // into by a PC, so a PC will go to a single TDTEntry (or return nullptr)
    using PCTable = AssociativeCache<TDTEntry>;

    // The following can safely be ignored
    std::unordered_map<int, std::unique_ptr<PCTable>> pcTables;

    PCTable &findTable(int context);
    PCTable &allocateNewContext(int context);
    // The preceding can safely be ignored

    void notifyFill(const CacheAccessProbeArg &arg) override;

    // Offset array
#define OFFSET_ARRAY_SIZE 52
    int offsetArray[OFFSET_ARRAY_SIZE] = {
        1,   2,   3,   4,   5,   6,   8,   9,   10,  12,  15,  16,  18,
        20,  24,  25,  27,  30,  32,  36,  40,  45,  48,  50,  54,  60,
        64,  72,  75,  80,  81,  90,  96,  100, 108, 120, 125, 128, 135,
        144, 150, 160, 162, 180, 192, 200, 216, 225, 240, 243, 250, 256};
    int offsetIndex;

    // Scoring system
    int scoreTable[OFFSET_ARRAY_SIZE] = {0};
    int roundCount; // Define max rounds, max score etc
    int bestOffset;
    bool disablePrefetching;

    // Recent Requests table
#define RR_TABLE_SIZE 256
    Addr RRTable[RR_TABLE_SIZE] = {0};

    bool testAddressWithOffset(Addr address, int offset);
    int calculateHash(Addr address);

    bool hasAddressBeenPrefetched(Addr address);

    void trainPrefetcher(Addr accessAddress);
    void issuePrefetch(Addr accessAddress,
                       std::vector<AddrPriority> &addresses);

  public:
    TDTPrefetcher(const TDTPrefetcherParams &p);

    void calculatePrefetch(const PrefetchInfo &pf1,
                           std::vector<AddrPriority> &addresses,
                           const CacheAccessor &cache) override;
};

} // namespace prefetch
} // namespace gem5

#endif //_MEM_CACHE_PREFETCH_TDT_PREFETCHER_HH__
