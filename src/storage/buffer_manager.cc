#include "buffer_manager.h"

#include "storage/page.h"
#include "storage/file_manager.h"

#include <cassert>
#include <iostream>
#include <new>         // placement new
#include <type_traits> // aligned_storage

static int nifty_counter; // zero initialized at load time
static typename std::aligned_storage<sizeof (BufferManager), alignof (BufferManager)>::type
    buffer_manager_buf; // memory for the object
BufferManager& buffer_manager = reinterpret_cast<BufferManager&> (buffer_manager_buf);

using namespace std;

BufferManager::BufferManager() {
    buffer_pool = nullptr;
    bytes = nullptr;
}


BufferManager::~BufferManager() {
    // It's not necessary to delete buffer_pool or bytes,
    // this destructor is called only on program exit.
}

void BufferManager::init(int _buffer_pool_size) {
    buffer_pool_size = _buffer_pool_size;
    buffer_pool = new Page[buffer_pool_size];
    clock_pos = 0;
    bytes = new char[buffer_pool_size*PAGE_SIZE];
}


void BufferManager::flush() {
    // flush() is always called at FileManager destruction.
    // this is important to check to avoid segfault when program terminates before calling init()
    // for instance, when bad parameters are received
    if (buffer_pool == nullptr) {
        return;
    }
    for (int i = 0; i < buffer_pool_size; i++) {
        buffer_pool[i].flush();
    }
}


Page& BufferManager::get_last_page(FileId file_id) {
    return get_page(file_id, file_manager.count_pages(file_id)-1); // TODO: manejar caso que no existe
}


Page& BufferManager::append_page(FileId file_id) {
    return get_page(file_id, file_manager.count_pages(file_id));
}


int BufferManager::get_buffer_available() {
    int first_lookup = clock_pos;

    while (buffer_pool[clock_pos].pins != 0) {
        clock_pos = (clock_pos+1)%buffer_pool_size;
        if (clock_pos == first_lookup) {
            throw std::runtime_error("No buffer available in buffer pool.");
        }
    }
    int res = clock_pos;
    clock_pos = (clock_pos+1)%buffer_pool_size;
    return res;
}


Page& BufferManager::get_page(FileId file_id, uint_fast32_t page_number) {
    if (file_manager.count_pages(file_id) < page_number) {
        std::cout << "Page Number: " << page_number << ", FileId: " << file_id.id << "(" << file_manager.get_filename(file_id) <<  ")\n";
        throw std::logic_error("getting wrong page_number.");
    }
    auto page_id = PageId(file_id, page_number);
    auto it = pages.find(page_id);

    std::lock_guard<std::mutex> lck(pin_mutex);

    if (it == pages.end()) {
        int buffer_available = get_buffer_available();
        if (buffer_pool[buffer_available].page_id.file_id.id != FileId::UNASSIGNED) {
            auto old_page_id = buffer_pool[buffer_available].page_id;
            pages.erase(old_page_id);
        }
        buffer_pool[buffer_available] = Page(page_id, &bytes[buffer_available*PAGE_SIZE]);

        file_manager.read_page(page_id, buffer_pool[buffer_available].get_bytes());
        pages.insert(pair<PageId, int>(page_id, buffer_available));
        return buffer_pool[buffer_available];
    } else { // file is already open
        buffer_pool[it->second].pins++;
        return buffer_pool[it->second];
    }
    // lock is released
}


void BufferManager::unpin(Page& page) {
    std::lock_guard<std::mutex> lck(pin_mutex);
    assert(page.pins != 0 && "Must not unpin if pin count is equal to 0. There is a logic error.");
    page.pins--;
}


// Nifty counter trick
BufferManagerInitializer::BufferManagerInitializer () {
    if (nifty_counter++ == 0) new (&buffer_manager) BufferManager(); // placement new
}

BufferManagerInitializer::~BufferManagerInitializer () {
    if (--nifty_counter == 0) (&buffer_manager)->~BufferManager();
}
