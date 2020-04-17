/* Page represents the content of a disk block in memory.
 * A page is treated as an array of `PAGE_SIZE` bytes (pointed by `bytes`).
 * For better performance, `PAGE_SIZE` should be multiple of the operating system's page size.
 * BufferManager is the only class who can construct a Page object. Other classes must get a Page
 * through BufferManager.
 */
#ifndef STORAGE__PAGE_H_
#define STORAGE__PAGE_H_

#include <iostream>
#include <string>

#include "storage/page_id.h"
#include "storage/file_manager.h"

const int PAGE_SIZE = 4096;

class Page {
friend class BufferManager; // needed to access private constructor
public:
    // reduces the count of objects using this page. Should be called when a object using this page is destroyed.
    void unpin();

    // mark as dirty so when page is replaced it is written back to disk.
    void make_dirty();

    // get the start memory position of `PAGE_SIZE` allocated bytes
    char* get_bytes() const { return bytes; };

    // get page number
    uint_fast32_t get_page_number() const { return page_id.page_number; };

private:
    PageId page_id;                 // contains file_id and page_number of this page
    uint_fast32_t pins;             // count of objects using this page
    char* bytes;                    // start memory address of the page, of size `PAGE_SIZE`
    bool dirty;                     // true if data in memory is different from disk

    Page();
    Page(PageId page_id, char* bytes);
    ~Page();
    Page& operator=(const Page& other);

     // reduces the count of objects using this page. Only buffer manager should call pin()
    void pin();

    // write to disk if this page is dirty
    void flush();
};

#endif // STORAGE__PAGE_H_
