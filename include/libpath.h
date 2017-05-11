/**
 * \author Nathan C. Castle
 * \brief Provides run-time path manipulation support for the I/O system
 */

#include <baseline/common.h>
#include <string.h>

#define IO_PATH_DELIMITER '\\'
#define IO_CHAN_DELIMITER ':'

/**
 * \brief counts the nodes in a path
 * \param path [in]
 * \returns int32_t number of nodes in path, excluding root
 */
int32_t io_path_count_nodes(char* path);

/**
 * \brief copies the first n nodes of path to out
 * \param path [in] path to slice
 * \param n [in] number of nodes to keep in returned path
 * \param out [out] path with only n nodes remaining
 */
void io_path_keep_n_nodes(char* path, unsigned int n, char* out);

/**
 * \brief copies all but n nodes from path to out
 * Note: this is fundamentally the same as io_path_keep_nodes
 * \param path [in] path to slice up
 * \param n [in] number of nodes that will not be copied over
 * \param out [out] path with all but n nodes copied over
 */
void io_path_disc_n_nodes(char* path, unsigned int n, char* out);

/**
 * \brief compares two paths, returns 0 if they're the same.
 *          canonicalized in this case means the paths will
 *          be converted to a standard format before comparison.
 * \param path1 [in]
 * \param path2 [in]
 * \returns int32_t 0 if same, not zero otherwise
 */
int32_t io_path_canonicalized_compare(char* path1, char* path2);
