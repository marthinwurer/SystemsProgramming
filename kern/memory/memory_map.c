
#include <kern/memory/memory_map.h>
#include <kern/realmode.h>



memory_map_entry * get_memory_map(uint32_t * size){
	uint32_t num = 0;

	regs16_t registers = {0};
	registers.bx = 0;
	registers.es = (MY_MM_ADDRESS >> 4) & 0xF000;
	registers.di = MY_MM_ADDRESS & 0xFFFF;

	do{
		registers.ax = 0xE820;
		registers.cx = sizeof(memory_map_entry);
		registers.dx = 0x0534D4150;

		__int32(0x15, &registers);

		registers.di += sizeof(memory_map_entry);




	}while (registers.bx != 0);




	return (memory_map_entry *) MY_MM_ADDRESS;

}
