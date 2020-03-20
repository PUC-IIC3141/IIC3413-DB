/* !!!! IMPORTANT !!!! DON'T include this file in a header file
 *
 * BufferManager contains all pages in memory and is used to get a page, making transparent if the page is
 * already in memory or needs to be readed from disk.
 *
 * `buffer_manager` is a global object and is available when this file is included. `buffer_manager` is only
 * constructed if it will be used in the execution. Construction is performed before the first line of main()
 * is executed and destrcution is performed after the last line of main() is executed.
 */

#ifndef STORAGE__BUFFER_MANAGER_H_
#define STORAGE__BUFFER_MANAGER_H_

#include "storage/file_id.h"
#include "storage/page_id.h"

#include <map>
#include <string>

const int BUFFER_POOL_SIZE = 65536;

class Page;
class Buffer;

class BufferManager {
friend class BufferManagerInitializer;  // needed to access private constructor
public:
    // Get a page. It will search in the buffer and if it is not on it, it will read from disk and put in the buffer.
    // Also it will pin the page, so calling unpin() is expected when the caller doesn't need the returned page
    // anymore.
    Page& get_page(FileId file_id, uint_fast32_t page_number);

    // Similar to get_page, but the page_number is the smallest number such that page number does not exist on disk.
    // The page returned has all its bytes initialized to 0. This operation perform a disk write inmediately
    // so 2 append_page in a row will work as expected.
    Page& append_page(FileId file_id);

    // write all dirty pages to disk
    void flush();

private:
    // map used to search the index in the `buffer_pool` of a certain page
    std::map<PageId, int> pages;

    // array of `BUFFER_POOL_SIZE` pages
    Page* buffer_pool;
    // begining of the allocated memory for the pages
    char* bytes;
    // simple clock used to page replacement
    int clock_pos;

    BufferManager();
    ~BufferManager();

    // returs the index of an unpined page from `buffer_pool`
    int get_buffer_available();
};

extern BufferManager& buffer_manager; // global object

static struct BufferManagerInitializer {
    BufferManagerInitializer();
    ~BufferManagerInitializer();
} bufferManager_initializer; // static initializer for every translation unit

#endif // STORAGE__BUFFER_MANAGER_H_
