ENTRY(Reset_Handler)
_estack = 0x24080000;
_Min_Heap_Size = 0x400;
_Min_Stack_Size = 0x2000;
MEMORY {
 FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 2048K
 RAM (xrw) : ORIGIN = 0x24000000, LENGTH = 512K
 ETHRAM (xrw) : ORIGIN = 0x30000000, LENGTH = 32K
 LWIPRAM (xrw) : ORIGIN = 0x30008000, LENGTH = 48K
}
PHDRS { flash PT_LOAD FLAGS(5); ram PT_LOAD FLAGS(6); bss PT_LOAD FLAGS(6); eth PT_LOAD FLAGS(6); heap PT_LOAD FLAGS(6); }
SECTIONS {
 .isr_vector : { . = ALIGN(4); KEEP(*(.isr_vector)) } >FLASH :flash
 .text : { . = ALIGN(4); *(.text*) *(.rodata*) KEEP(*(.init)) KEEP(*(.fini)) . = ALIGN(4); _etext = .; } >FLASH :flash
 .ARM.extab : { *(.ARM.extab*) } >FLASH :flash
 .ARM.exidx : { __exidx_start = .; *(.ARM.exidx*) __exidx_end = .; } >FLASH :flash
 .preinit_array : { PROVIDE_HIDDEN(__preinit_array_start = .); KEEP(*(.preinit_array*)) PROVIDE_HIDDEN(__preinit_array_end = .); } >FLASH :flash
 .init_array : { PROVIDE_HIDDEN(__init_array_start = .); KEEP(*(SORT(.init_array.*))) KEEP(*(.init_array*)) PROVIDE_HIDDEN(__init_array_end = .); } >FLASH :flash
 .fini_array : { PROVIDE_HIDDEN(__fini_array_start = .); KEEP(*(SORT(.fini_array.*))) KEEP(*(.fini_array*)) PROVIDE_HIDDEN(__fini_array_end = .); } >FLASH :flash
 _sidata = LOADADDR(.data);
 .data : { . = ALIGN(4); _sdata = .; *(.data*) . = ALIGN(4); _edata = .; } >RAM AT>FLASH :ram
 .bss (NOLOAD) : AT(ADDR(.bss)) { . = ALIGN(4); _sbss = .; *(.bss*) *(COMMON) . = ALIGN(4); _ebss = .; } >RAM :bss
 ._user_heap_stack (NOLOAD) : AT(ADDR(._user_heap_stack)) { . = ALIGN(8); PROVIDE(end = .); PROVIDE(_end = .); . += _Min_Heap_Size; . += _Min_Stack_Size; . = ALIGN(8); } >RAM :bss
 .eth_dma (NOLOAD) : { . = ALIGN(32); *(.RxDescripSection) . = ALIGN(32); *(.TxDescripSection) . = ALIGN(32); *(.Rx_PoolSection) . = ALIGN(32); } >ETHRAM :eth
 .lwip_heap (NOLOAD) : { . += 40K; } >LWIPRAM :heap
 .ARM.attributes 0 : { *(.ARM.attributes) }
}
ASSERT(SIZEOF(.eth_dma) <= LENGTH(ETHRAM), "Ethernet descriptor/RX region overflow")
ASSERT(_ebss + _Min_Heap_Size + _Min_Stack_Size <= _estack, "RAM overflow")
