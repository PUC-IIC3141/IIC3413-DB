#include "page.h"

#include "storage/file_id.h"

Page::Page(PageId page_id, char* bytes)
    : page_id(page_id), pins(1), bytes(bytes), dirty(false) { }


Page::Page()
    : page_id(FileId(FileId::UNASSIGNED), 0), pins(0), bytes(nullptr), dirty(false) { }


Page::~Page() = default;


Page& Page::operator=(const Page& other) {
    this->flush();
    this->page_id = other.page_id;
    this->pins    = other.pins;
    this->dirty   = other.dirty;
    this->bytes   = other.bytes;
    return *this;
}


void Page::unpin() {
    if (pins == 0) {
        throw std::logic_error("Inconsistent unpin when pins == 0");
    }
    pins--;
}


void Page::make_dirty() {
    dirty = true;
}


void Page::pin() {
    pins++;
}


void Page::flush() {
    if (dirty) {
        file_manager.flush(page_id, bytes);
        dirty = false;
    }
}
