
/*
  +------------------------------------------------------------------------+
  | Dao Framework                                                          |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2014 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  |          Vladimir Kolesnikov <vladimir@extrememember.com>              |
  |          ZhuZongXin <dreamsxin@qq.com>                                 |
  +------------------------------------------------------------------------+
*/

#ifndef DAO_KERNEL_BLOOMFILTER_H
#define DAO_KERNEL_BLOOMFILTER_H

#include "kernel/memory.h"
#include "kernel/murmurhash.h"

#define DAO_BLOOMFILTER_FILE_MAGIC_CODE		0x44616F37

#define DAO_BLOOMFILTER_SETBIT(filter, n)   (filter->data[n/8] |= (1 << (n%8)))
#define DAO_BLOOMFILTER_GETBIT(filter, n)   (filter->data[n/8] & (1 << (n%8)))

typedef struct {
    uint8_t flag;				// 初始化标志
	unsigned char *data;		// 数据存储指针
    uint32_t *hash_value;		// 上次 hash 得到的值
    uint32_t max_items;			// 最大元素个数
    uint32_t bits;				// 需要的比特数
	uint32_t bytes;				// 需要的字节数
    uint32_t hash_num;			// 哈希函数个数
    uint32_t seed;				// 种子偏移量
    uint32_t count;				// 已存元素个数
    double false_positive;		// 误报概率
} dao_bloomfilter;

typedef struct {
    uint32_t file_magic_code;	// 文件头部标识
    uint32_t max_items;
    uint32_t bits;
	uint32_t bytes;
    uint32_t hash_num;
    uint32_t seed;
    uint32_t count;
	double false_positive;
} dao_bloomfilter_file_head;

int dao_bloomfilter_init(dao_bloomfilter *bloomfilter, uint32_t seed, uint32_t max_items, double false_positive);
int dao_bloomfilter_add(dao_bloomfilter *bloomfilter, const void * key, int len);
int dao_bloomfilter_check(dao_bloomfilter *bloomfilter, const void * key, int len);
int dao_bloomfilter_free(dao_bloomfilter *bloomfilter);

static zend_always_inline int dao_bloomfilter_reset(dao_bloomfilter *bloomfilter)
{
    if (bloomfilter == NULL)
        return -1;

    if (bloomfilter->data) {
        memset(bloomfilter->data, 0, bloomfilter->bytes);
    }
    bloomfilter->count = 0;
    return 0;
}

int dao_bloomfilter_load(dao_bloomfilter *bloomfilter, char *filename);
int dao_bloomfilter_save(dao_bloomfilter *bloomfilter, char *filename);

#endif /* DAO_KERNEL_BLOOMFILTER_H */
