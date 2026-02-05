#pragma once

#include <map>
#include <set>
#include <cstddef>

class Allocator {
private:
    void* ptr;  // 实际分配的内存指针
    size_t currentOffset;  // 当前分配偏移量
    size_t totalSize;  // 总分配大小
    
    // 空闲块管理
    std::map<size_t, size_t> freeBlocksByStart;  // key: 起始地址, value: 块大小
    std::map<size_t, std::set<size_t>> freeBlocksBySize;  // key: 块大小, value: 起始地址集合
    
public:
    Allocator();
    ~Allocator();
    
    // 分配内存，返回起始地址偏移量
    size_t alloc(size_t size);
    
    // 释放内存
    void free(size_t addr, size_t size);
    
    // 获取对齐后的尺寸
    size_t getAlignedSize(size_t size);
    
    // 获取内存指针
    void* getPtr();
    
    // 打印分配信息
    void info();
    
private:
    // 合并相邻的空闲块
    void mergeFreeBlocks();
};
