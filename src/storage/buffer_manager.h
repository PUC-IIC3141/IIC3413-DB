/*
 * BufferManager contains all pages in memory and is used to get a page, making transparent if the page is
 * already in memory or needs to be readed from disk.
 *
 * `buffer_manager` is a global object and is available when this file is included. Before using it, somebody
 * must call the method BufferManager::init(), usually is the responsability of the model (e.g. RelationalModel)
 * to call it.
 *
 * A mutex object is used to prevent conflict between different threads when asking for page and
 * unpining a page.
 */

#ifndef STORAGE__BUFFER_MANAGER_H_
#define STORAGE__BUFFER_MANAGER_H_

#include "storage/file_id.h"
#include "storage/page_id.h"

#include <map>
#include <mutex>
#include <string>

class Page;

class BufferManager {
public:
    static constexpr int DEFAULT_BUFFER_POOL_SIZE = 1024;

    ~BufferManager();

    // necesary to be called before first usage
    static void init(int buffer_pool_size);

    // Get a page. It will search in the buffer and if it is not on it, it will read from disk and put in the buffer.
    // Also it will pin the page, so calling buffer_manager.unpin(page) is expected when the caller doesn't need
    // the returned page anymore.
    Page& get_page(FileId file_id, uint_fast32_t page_number);

    // Similar to get_page, but the page_number is the greatest number such that page number exist on disk.
    Page& get_last_page(FileId file_id);

    // Similar to get_page, but the page_number is the smallest number such that page number does not exist on disk.
    // The page returned has all its bytes initialized to 0. This operation perform a disk write inmediately
    // so 2 append_page in a row will work as expected.
    Page& append_page(FileId file_id);

    // write all dirty pages to disk
    void flush();

    // reduces the count of objects using the page. Should be called when a object using the page is destroyed.
    void unpin(Page& page);

private:
    BufferManager(int _buffer_pool_size);

    // maximum pages the buffer can have
    int buffer_pool_size;

    // map used to search the index in the `buffer_pool` of a certain page
    std::map<PageId, int> pages;

    // array of `BUFFER_POOL_SIZE` pages
    Page* buffer_pool;

    // begining of the allocated memory for the pages
    char* bytes;

    // simple clock used to page replacement
    int clock_pos;

    // to avoid pin/unpin synchronization problems
    std::mutex pin_mutex;

    // returns the index of an unpined page from `buffer_pool`
    int get_buffer_available();
};

extern BufferManager& buffer_manager; // global object

#endif // STORAGE__BUFFER_MANAGER_H_
