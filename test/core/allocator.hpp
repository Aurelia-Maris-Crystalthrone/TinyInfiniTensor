// ============== 作业 ========================
// TODO: 可能需要设计一个数据结构来存储free block，以便于管理和合并
// HINT: 可以使用一个 map 来存储 free block, key 为 block 的起始/结尾地址，value 为 block 的大小

#include <map>

// 使用两个映射来管理空闲块
// freeBlocksByStart: key为起始地址, value为块大小
// freeBlocksBySize: key为块大小, value为起始地址集合（可能有多个相同大小的块）
std::map<size_t, size_t> freeBlocksByStart;
std::map<size_t, std::set<size_t>> freeBlocksBySize;

// ============== 作业 ========================
