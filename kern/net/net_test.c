#include <kern/net/net_test.h>
#include <kern/net/intel.h>
#include <kern/pci/pci.h>
#include <baseline/c_io.h>

int32_t net_test_main(void* args) {
	(void) args; // surpress warnings
	
	// c_printf("in net_test_main()\n");
	// pci_enumerate(8, 16);

	intel_nic_init();

	return 0;
}

