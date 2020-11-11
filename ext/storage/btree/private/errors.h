#ifndef DAO_STORAGE_BTREE_ERRORS_H_
#define DAO_STORAGE_BTREE_ERRORS_H_

#define DAO_STORAGE_BTREE_OK              0

#define DAO_STORAGE_BTREE_EFILE           0x101
#define DAO_STORAGE_BTREE_EFILEREAD_OOB   0x102
#define DAO_STORAGE_BTREE_EFILEREAD       0x103
#define DAO_STORAGE_BTREE_EFILEWRITE      0x104
#define DAO_STORAGE_BTREE_EFILEFLUSH      0x105
#define DAO_STORAGE_BTREE_EFILERENAME     0x106
#define DAO_STORAGE_BTREE_ECOMPACT_EXISTS 0x107

#define DAO_STORAGE_BTREE_ECOMP           0x201
#define DAO_STORAGE_BTREE_EDECOMP         0x202

#define DAO_STORAGE_BTREE_EALLOC          0x301
#define DAO_STORAGE_BTREE_EMUTEX          0x302
#define DAO_STORAGE_BTREE_ERWLOCK         0x303

#define DAO_STORAGE_BTREE_ENOTFOUND       0x401
#define DAO_STORAGE_BTREE_ESPLITPAGE      0x402
#define DAO_STORAGE_BTREE_EEMPTYPAGE      0x403
#define DAO_STORAGE_BTREE_EUPDATECONFLICT 0x404
#define DAO_STORAGE_BTREE_EREMOVECONFLICT 0x405

#endif /* DAO_STORAGE_BTREE_ERRORS_H_ */