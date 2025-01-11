#ifndef PAGE_H
#define PAGE_H

#define PAGE_SIZE 0x1000

#define PAGE_ALIGN_DOWN(address) address/PAGE_SIZE
#define PAGE_ALIGN_UP(address)  (address + PAGE_SIZE -1)/PAGE_SIZE

#endif