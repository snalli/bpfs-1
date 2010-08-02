/* This file is part of BPFS. BPFS is copyright 2009-2010 The Regents of the
 * University of California. It is distributed under the terms of version 2
 * of the GNU GPL. See the file LICENSE for details. */

#ifndef BPFS_H
#define BPFS_H

#include "bpfs_structs.h"

#include <stdint.h>


// Set to 0 to use shadow paging, 1 to use short-circuit shadow paging
#define SCSP_ENABLED 1

// Set to 1 to optimize away some COWs
#define COW_OPT SCSP_ENABLED

#define BPFS_EOF UINT64_MAX

// TODO: rephrase this as you-see-everything-p?
// NOTE: this doesn't describe situations where the top block is already COWed
//       but child blocks are refed by the original top block.
enum commit {
	COMMIT_NONE,   // no writes allowed
	COMMIT_COPY,   // writes only to copies
#if COW_OPT
	COMMIT_ATOMIC, // write in-place if write is atomic; otherwise, copy
#else
	COMMIT_ATOMIC = COMMIT_COPY,
#endif
	COMMIT_FREE,   // no restrictions on writes (e.g., region is not yet refed)
};


uint64_t cow_block(uint64_t old_blockno,
                   unsigned off, unsigned size, unsigned valid);
uint64_t cow_block_hole(unsigned off, unsigned size, unsigned valid);
uint64_t cow_block_entire(uint64_t old_blockno);

uint64_t tree_max_nblocks(uint64_t height);
uint64_t tree_height(uint64_t nblocks);
int tree_change_height(struct bpfs_tree_root *root,
                       unsigned new_height,
                       enum commit commit, uint64_t *blockno);

struct bpfs_super* get_super(void);

char* get_block(uint64_t blockno);
static __inline
unsigned block_offset(const void *x) __attribute__((always_inline));

struct bpfs_tree_root* get_inode_root(void);
int get_inode_offset(uint64_t ino, uint64_t *poffset);

void ha_set_addr(struct height_addr *pha, uint64_t addr);
void ha_set(struct height_addr *pha, uint64_t height, uint64_t addr);

int truncate_block_zero(struct bpfs_tree_root *root,
                        uint64_t begin, uint64_t end, uint64_t valid,
                        uint64_t *blockno);


static __inline
unsigned block_offset(const void *x)
{
	return ((uintptr_t) x) % BPFS_BLOCK_SIZE;
}

#endif
