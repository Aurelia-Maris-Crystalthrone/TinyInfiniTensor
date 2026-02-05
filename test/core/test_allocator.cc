size_t Allocator::alloc(size_t size) {
    IT_ASSERT(this->ptr == nullptr);
    // pad the size to the multiple of alignment
    size = this->getAlignedSize(size);

    // ============== 作业 ========================
    // TODO: 设计一个算法来分配内存，返回起始地址偏移量
    
    // 首先尝试在空闲块中查找合适大小的块
    // 使用最佳适配算法：找到大于等于所需大小的最小块
    auto it = freeBlocksBySize.lower_bound(size);
    
    if (it != freeBlocksBySize.end()) {
        // 找到合适的空闲块
        size_t blockSize = it->first;
        auto& addrSet = it->second;
        
        // 获取第一个可用的起始地址
        size_t startAddr = *addrSet.begin();
        
        // 从空闲块集合中移除这个块
        addrSet.erase(startAddr);
        if (addrSet.empty()) {
            freeBlocksBySize.erase(it);
        }
        
        // 从起始地址映射中移除
        freeBlocksByStart.erase(startAddr);
        
        // 如果块大小大于所需大小，将剩余部分作为新的空闲块
        if (blockSize > size) {
            size_t remainingSize = blockSize - size;
            size_t remainingAddr = startAddr + size;
            
            // 添加剩余部分到空闲块集合
            freeBlocksByStart[remainingAddr] = remainingSize;
            freeBlocksBySize[remainingSize].insert(remainingAddr);
        }
        
        return startAddr;
    }
    
    // 如果没有合适的空闲块，从当前偏移量分配
    size_t startAddr = currentOffset;
    currentOffset += size;
    
    return startAddr;
    
    // ============== 作业 ========================
}

void Allocator::free(size_t addr, size_t size) {
    IT_ASSERT(this->ptr == nullptr);
    size = getAlignedSize(size);

    // ============== 作业 ========================
    // TODO: 设计一个算法来回收内存
    
    // 将释放的块添加到空闲块集合
    freeBlocksByStart[addr] = size;
    freeBlocksBySize[size].insert(addr);
    
    // 合并相邻的空闲块
    mergeFreeBlocks();
    
    // ============== 作业 ========================
}

void Allocator::mergeFreeBlocks() {
    if (freeBlocksByStart.empty()) return;
    
    // 按起始地址排序后，合并相邻的空闲块
    auto it = freeBlocksByStart.begin();
    auto nextIt = std::next(it);
    
    while (nextIt != freeBlocksByStart.end()) {
        size_t currentAddr = it->first;
        size_t currentSize = it->second;
        size_t nextAddr = nextIt->first;
        size_t nextSize = nextIt->second;
        
        // 检查当前块和下一个块是否相邻
        if (currentAddr + currentSize == nextAddr) {
            // 合并两个块
            size_t mergedSize = currentSize + nextSize;
            
            // 从空闲块集合中移除原来的两个块
            // 从大小映射中移除
            freeBlocksBySize[currentSize].erase(currentAddr);
            if (freeBlocksBySize[currentSize].empty()) {
                freeBlocksBySize.erase(currentSize);
            }
            
            freeBlocksBySize[nextSize].erase(nextAddr);
            if (freeBlocksBySize[nextSize].empty()) {
                freeBlocksBySize.erase(nextSize);
            }
            
            // 从起始地址映射中移除
            freeBlocksByStart.erase(nextAddr);
            
            // 更新当前块的大小
            it->second = mergedSize;
            
            // 添加到大小映射
            freeBlocksBySize[mergedSize].insert(currentAddr);
            
            // 重新开始合并，因为合并后可能还能和下一个块合并
            nextIt = std::next(it);
        } else {
            // 不合并，继续检查下一对
            ++it;
            ++nextIt;
        }
    }
}
