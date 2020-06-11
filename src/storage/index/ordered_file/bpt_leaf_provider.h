#ifndef STORAGE__BPT_LEAF_PROVIDER_H_
#define STORAGE__BPT_LEAF_PROVIDER_H_

#include <cstdint>

class BptLeafProvider {
public:
    virtual ~BptLeafProvider() { }

    virtual void begin() = 0;
    virtual bool has_more_tuples() = 0;
    virtual uint_fast32_t next_tuples(uint64_t* output, uint_fast32_t max_tuples) = 0;
};

#endif // STORAGE__BPT_LEAF_PROVIDER_H_
