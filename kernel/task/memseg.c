#include "memseg.h"
#include <stdint.h>
#include "paging.h"
#include "scheduler.h"
#include "string.h"

memseg *memseg_map(process *proc, uint64_t address,size_t size,uint64_t flags){
	//first convert all of that into pages
	address = PAGE_ALIGN_DOWN(address) / PAGE_SIZE;
	size = PAGE_ALIGN_UP(size) / PAGE_SIZE;

	memseg *new_memseg = kmalloc(sizeof(memseg));
	if(proc->first_memseg){
		proc->first_memseg->prev = new_memseg;
	}
	new_memseg->next = proc->first_memseg;
	proc->first_memseg = new_memseg;
	new_memseg->prev = NULL;

	new_memseg->offset = (void*)(address * PAGE_SIZE);
	new_memseg->size = size;
	new_memseg->flags = flags;

	while(size > 0){
		map_page(proc->cr3 + kernel->hhdm,allocate_page(&kernel->bitmap),address,flags);
		address++;
		size--;
	}

	return new_memseg;
}

void memseg_unmap(process *proc,memseg *seg){
	//link the two sides
	if(seg->prev){
		seg->prev->next = seg->next;
	}
	if(seg->next){
		seg->next->prev = seg->prev;
	}

	//unmap
	size_t size = seg->size;
	uint64_t virt_page = (uint64_t)seg->offset / PAGE_SIZE;
	while(size > 0){
		//get the phys page
		uint64_t phys_page = (uint64_t)PMLT4_virt2phys(proc->cr3 + kernel->hhdm,virt_page * PAGE_SIZE) / PAGE_SIZE;

		//unmap it
		unmap_page(proc->cr3 + kernel->hhdm,virt_page);

		//free it
		free_page(&kernel->bitmap,phys_page);
		
		virt_page++;
		size--;
	}

	kfree(seg);
}

void memseg_clone(process *parent,process *child,memseg *seg){
	memseg *new_seg = memseg_map(child,seg->offset,seg->size,seg->flags);
	
	size_t size = new_seg->size;
	uint64_t virt_page = (uint64_t)new_seg->offset / PAGE_SIZE;
	while(size > 0){
		//get the phys page
		void * phys_page = PMLT4_virt2phys(child->cr3 + kernel->hhdm,virt_page * PAGE_SIZE);

		memcpy((uint64_t)phys_page + kernel->hhdm,seg->offset + (virt_page * PAGE_SIZE - (uint64_t)new_seg->offset),PAGE_SIZE);
		
		virt_page++;
		size--;
	}

}