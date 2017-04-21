
#include <kern/memory/memory_map.h>
#include <kern/early/realmode.h>
#include <kern/early/memory_map_setup.h>



void get_memory_map(void){
	uint32_t num = 0;

	memory_map_entry * start = (memory_map_entry *) MY_MM_ADDRESS;

	regs16_t registers = {0};
	registers.ebx = 0;
	registers.es = (MY_MM_ADDRESS >> 4) & 0xF000;
	registers.edi = MY_MM_ADDRESS & 0xFFFF;

	do{
		registers.eax = 0xE820;
		registers.ecx = sizeof(memory_map_entry);
		registers.edx = 0x0534D4150;
		registers.edi += sizeof(memory_map_entry); // reserve the first slot for the count

		__int(0x15, &registers);

		num++;




	}while (registers.ebx != 0);

	start->base_l = num;





}
