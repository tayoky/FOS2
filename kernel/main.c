#include "limine.h"
#include "serial.h"
#include "gdt.h"
#include "idt.h"
#include "kernel.h"
#include "asm.h"
#include "print.h"

kernel_table master_kernel_table;

//the entry point
void kmain(){
        disable_interrupt();
        init_serial();
        init_gdt(&master_kernel_table);
        init_idt(&master_kernel_table);
        enable_interrupt()
        kprintf("finish init kernel\n");
        //infinite loop
        while (1){
                
        }
}