void GraphObj::dataMalloc() {
    // topological sorting first
    IT_ASSERT(topo_sort() == true);

    // ============== 作业 ========================
    // TODO: 利用 allocator 给计算图分配内存
    // HINT: 获取分配好的内存指针后，可以调用 tensor 的 setDataBlob 函数给 tensor 绑定内存
    
    // 首先计算所有张量的总大小
    size_t totalSize = 0;
    std::vector<Tensor> allTensors;
    
    // 收集所有张量
    for (const auto& op : operators) {
        for (const auto& input : op->getInputs()) {
            allTensors.push_back(input);
        }
        for (const auto& output : op->getOutputs()) {
            allTensors.push_back(output);
        }
    }
    
    // 移除重复的张量
    std::sort(allTensors.begin(), allTensors.end());
    allTensors.erase(std::unique(allTensors.begin(), allTensors.end()), allTensors.end());
    
    // 计算总大小
    for (const auto& tensor : allTensors) {
        totalSize += tensor->getBytes();
    }
    
    // 使用运行时分配内存
    void* blob = runtime->alloc(totalSize);
    
    // 使用allocator管理内存分配
    Allocator allocator(runtime);
    
    // 为每个张量分配内存偏移量
    std::map<Tensor, size_t> tensorOffsets;
    for (const auto& tensor : allTensors) {
        size_t offset = allocator.alloc(tensor->getBytes());
        tensorOffsets[tensor] = offset;
    }
    
    // 获取分配的内存指针并绑定到张量
    void* basePtr = allocator.getPtr();
    for (const auto& [tensor, offset] : tensorOffsets) {
        void* tensorPtr = static_cast<char*>(basePtr) + offset;
        tensor->setDataBlob(tensorPtr);
    }
    
    // ============== 作业 ========================

    allocator.info();
}
