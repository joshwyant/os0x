#include "kernel/paging.h"

using namespace k;

status_code RecursivePageTables::map(uintptr_t virt_addr, uintptr_t phys_addr, PageAttr attributes) const
{
    status_code status;
    PageEntry entry;
    uintptr_t page_addr;

    // Recurse page tables and make sure they exist; find the final page table
    for (auto level = PageLevel::PML4; level >= PageLevel::PD; level--)
    {
        // Get the table and index
        auto volatile table = (PageTable *)table_address(virt_addr, level);
        auto idx = entry_index(virt_addr, level);

        // Get the entry and check if it's present
        status = table->getEntry(idx, entry);
        CHECK_STATUS(); // bounds check

        // Create a new subtable
        if (!entry.present())
        {
            // Allocate a page
            status = _pallocator.allocate_page(&page_addr);
            CHECK_STATUS(); // out of mem?

            // Poke it into scratch space (calls invlpg)
            auto temp_page = const_cast<volatile PageTable *>(_scratch_space);
            map_notables((uintptr_t)temp_page, page_addr, PageAttr::Present | PageAttr::RW | PageAttr::NX);

            // Clear the new page table
            temp_page->clear();

            // We could have done the above after mapping it in, but by clearing it before it
            // officially becomes an MMU structure could avoid undefined behavior.

            // Set the entry
            status = table->setEntry(idx, {page_addr, PageAttr::Present | PageAttr::RW});
            CHECK_STATUS(); // bounds check

            // Invalidate the page in the virtual page tables for the current level
            invalidate_page((uintptr_t)table);
        }
    }

    // Set the new entry in the lowest page table (calls invlpg)
    map_notables(virt_addr, phys_addr, attributes);

    return status_code::ok;
}