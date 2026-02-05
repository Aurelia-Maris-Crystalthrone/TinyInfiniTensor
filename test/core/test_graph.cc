#include "core/graph.h"
#include "core/kernel.h"
#include "core/runtime.h"
#include "operators/matmul.h"
#include "operators/transpose.h"

#include "test.h"

namespace infini
{
    TEST(Graph, Optimize)
    {
        Runtime runtime = NativeCpuRuntimeObj::getInstance();
        Graph g = make_ref<GraphObj>(runtime);
        Tensor i1 = g->addTensor({2, 3, 4, 5}, DataType::UInt32);
        Tensor i2 = g->addTensor({2, 3, 4, 5}, DataType::UInt32);
        Tensor t1 = g->addTensor({2, 3, 5, 4}, DataType::UInt32);
        Tensor t2 = g->addTensor({2, 3, 4, 5}, DataType::UInt32);
        Tensor t3 = g->addTensor({2, 3, 5, 4}, DataType::UInt32);
        Tensor o = g->addTensor({2, 3, 4, 4}, DataType::UInt32);
        g->addOpWithOutputs<TransposeObj>(i1, t1, Shape{0, 1, 3, 2});
        g->addOpWithOutputs<TransposeObj>(t1, t2, Shape{0, 1, 3, 2});
        g->addOpWithOutputs<TransposeObj>(i2, t3, Shape{0, 1, 3, 2});
        g->addOpWithOutputs<MatmulObj>(t2, t3, o);
        // 优化前
        g->print();
        g->optimize();
        // 优化后
        g->print();
        EXPECT_EQ(g->getOperators().size(), 1);
        EXPECT_EQ(g->getTensors().size(), 3);
        EXPECT_EQ(g->getOperators()[0]->getOpType().underlying(), 7);
        auto op = as<MatmulObj>(g->getOperators()[0]);
        EXPECT_EQ(op->getInputs(0)->getGuid(), 2);
        EXPECT_EQ(op->getInputs(1)->getGuid(), 3);
        EXPECT_EQ(op->getOutputs()[0], o);
        EXPECT_EQ(op->getTransA(), false);
        EXPECT_EQ(op->getTransB(), true);
    }
void GraphObj::dataMalloc()
{
    // topological sorting first
    IT_ASSERT(topo_sort() == true);

    // ====================== 作业 ============================
    // TODO: 利用 allocator 给计算图分配内存
    // HINT: 获取分配的内存指针后，可以调用 tensor 的 setDataBlob 函数给 tensor 绑定内存
    
    // 获取图中的所有张量
    auto allTensors = getTensors();
    
    // 遍历拓扑排序后的算子
    for (auto& op : this->ops) {
        // 为算子的输出张量分配内存
        for (auto& output : op->getOutputs()) {
            // 如果张量还没有分配内存
            if (!output->hasData()) {
                // 计算张量所需的内存大小
                size_t tensorSize = output->getBytes();
                
                // 使用分配器分配内存
                size_t offset = allocator.alloc(tensorSize);
                
                // 获取分配的内存基地址
                void* basePtr = allocator.getPtr();
                
                // 计算张量的实际内存地址
                void* tensorPtr = static_cast<char*>(basePtr) + offset;
                
                // 为张量设置数据指针
                output->setDataBlob(make_ref<Blob>(tensorPtr, tensorSize));
            }
        }
        
        // 注意：输入张量通常在前驱算子中已经分配了内存
        // 但如果输入张量是图的输入（没有前驱算子），也需要分配内存
        for (auto& input : op->getInputs()) {
            if (!input->hasData()) {
                size_t tensorSize = input->getBytes();
                size_t offset = allocator.alloc(tensorSize);
                void* basePtr = allocator.getPtr();
                void* tensorPtr = static_cast<char*>(basePtr) + offset;
                input->setDataBlob(make_ref<Blob>(tensorPtr, tensorSize));
            }
        }
    }
    
    // ====================== 作业 ============================

    allocator.info();
}
}
