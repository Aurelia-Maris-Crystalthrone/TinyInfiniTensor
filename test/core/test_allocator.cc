#include "core/allocator.h"
#include <iostream>

Allocator::Allocator() : ptr(nullptr), currentOffset(0), totalSize(0) {}

Allocator::~Allocator() {
    // 清理资源
}

size_t Allocator::getAlignedSize(size_t size) {
    const size_t alignment = 64;  // 64字节对齐
    return (size + alignment - 1) & ~(alignment - 1);
}

size_t Allocator::alloc(size_t size) {
    IT_ASSERT(this->ptr == nullptr);
    size = this->getAlignedSize(size);

    // 首先尝试在空闲块中查找合适大小的块
    auto it = freeBlocksBySize.lower_bound(size);
    
    if (it != freeBlocksBySize.end()) {
        size_t blockSize = it->first;
        auto& addrSet = it->second;
        
        size_t startAddr = *addrSet.begin();
        
        // 从空闲块集合中移除
        addrSet.erase(startAddr);
        if (addrSet.empty()) {
            freeBlocksBySize.erase(it);
        }
        
        freeBlocksByStart.erase(startAddr);
        
        // 如果块大小大于所需大小，分割块
        if (blockSize > size) {
            size_t remainingSize = blockSize - size;
            size_t remainingAddr = startAddr + size;
            
            freeBlocksByStart[remainingAddr] = remainingSize;
            freeBlocksBySize[remainingSize].insert(remainingAddr);
        }
        
        return startAddr;
    }
    
    // 从当前偏移量分配
    size_t startAddr = currentOffset;
    currentOffset += size;
    totalSize = std::max(totalSize, currentOffset);
    
    return startAddr;
}

void Allocator::free(size_t addr, size_t size) {
    IT_ASSERT(this->ptr == nullptr);
    size = getAlignedSize(size);
    
    // 添加释放的块到空闲块集合
    freeBlocksByStart[addr] = size;
    freeBlocksBySize[size].insert(addr);
    
    // 合并相邻的空闲块
    mergeFreeBlocks();
}

void Allocator::mergeFreeBlocks() {
    if (freeBlocksByStart.empty()) return;
    
    // 按起始地址排序，合并相邻块
    auto it = freeBlocksByStart.begin();
    
    while (it != freeBlocksByStart.end()) {
        auto nextIt = std::next(it);
        if (nextIt == freeBlocksByStart.end()) break;
        
        size_t currentAddr = it->first;
        size_t currentSize = it->second;
        size_t nextAddr = nextIt->first;
        size_t nextSize = nextIt->second;
        
        // 检查是否相邻
        if (currentAddr + currentSize == nextAddr) {
            // 合并两个块
            size_t mergedSize = currentSize + nextSize;
            
            // 从空闲块集合中移除
            freeBlocksBySize[currentSize].erase(currentAddr);
            if (freeBlocksBySize[currentSize].empty()) {
                freeBlocksBySize.erase(currentSize);
            }
            
            freeBlocksBySize[nextSize].erase(nextAddr);
            if (freeBlocksBySize[nextSize].empty()) {
                freeBlocksBySize.erase(nextSize);
            }
            
            freeBlocksByStart.erase(nextIt);
            
            // 更新当前块
            it->second = mergedSize;
            freeBlocksBySize[mergedSize].insert(currentAddr);
            
            // 继续检查合并后的块是否能和下一个块合并
            continue;
        }
        
        ++it;
    }
}

void* Allocator::getPtr() {
    return ptr;
}

void Allocator::info() {
    std::cout << "Allocator Info:" << std::endl;
    std::cout << "  Total allocated size: " << totalSize << std::endl;
    std::cout << "  Current offset: " << currentOffset << std::endl;
    std::cout << "  Free blocks count: " << freeBlocksByStart.size() << std::endl;
    
    for (const auto& [addr, size] : freeBlocksByStart) {
        std::cout << "    Free block: addr=" << addr << ", size=" << size << std::endl;
    }
}
