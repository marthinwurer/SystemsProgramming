/**
 * \author Nathan C. Castle
 * \brief Provides run-time path manipulation support for the I/O system
 */

#include <baseline/common.h>
#include <string.h>

const static char IO_PATH_DELIMITER = '\\';
const static char IO_CHAN_DELIMITER = ':';

int32_t io_path_count_nodes(char* path);

void io_path_keep_n_nodes(char* path, unsigned int n, char* out);

void io_path_disc_n_nodes(char* path, unsigned int n, char* out);

int32_t io_path_canonicalized_compare(char* path1, char* path2);
