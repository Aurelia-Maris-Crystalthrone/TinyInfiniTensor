#include "operators/concat.h"
#include "utils/operator_utils.h"

namespace infini {
ConcatObj::ConcatObj(GraphObj *graph, TensorVec inputs, Tensor output, int _dim)
    : OperatorObj(OpType::Concat, inputs, {output}) {
    int rank = inputs[0]->getRank();
    dim = get_real_axis(_dim, rank);
    IT_ASSERT(checkValid(graph));
}

optional<vector<Shape>> ConcatObj::inferShape(const TensorVec &inputs) {
    Shape dims = inputs[0]->getDims();
    auto rank = inputs[0]->getRank();

    // =================================== 作业 ===================================
    // TODO：修改 dims，返回正确的 concat 后的 shape
    // REF: https://onnx.ai/onnx/operators/onnx__Concat.html#concat-13
    // =================================== 作业 ===================================

    return {{dims}};
}

std::string ConcatObj::toString() const {
    std::ostringstream os;
    os << "Concat[" << getGuid() << "]";
    os << "(";
    for (auto input : inputs)
        os << vecToString(input->getDims()) << ",";
    os << "dim=" << dim << ",";
    os << "input=";
    for (auto input : inputs)
        os << input->getGuid() << ",";
    os << "output=" << outputs[0]->getGuid() << ")";
    return os.str();
}

} // namespace infini
#include "core/graph.h"
#include "core/kernel.h"
#include "core/runtime.h"
#include "operators/unary.h"

#include "test.h"

namespace infini
{
    TEST(Allocator, testAlloc)
    {
        Shape shape = Shape{1, 2, 2, 3};
        Runtime runtime = NativeCpuRuntimeObj::getInstance();
        Tensor a = make_ref<TensorObj>(shape, DataType::Float32, runtime);
        Tensor b = make_ref<TensorObj>(shape, DataType::Float32, runtime);
        Tensor c = make_ref<TensorObj>(shape, DataType::Float32, runtime);
        Tensor d = make_ref<TensorObj>(shape, DataType::Float32, runtime);
        Allocator allocator = Allocator(runtime);
        // allocate a->b->c
        size_t offsetA = allocator.alloc(a->getBytes());
        size_t offsetB = allocator.alloc(b->getBytes());
        size_t offsetC = allocator.alloc(c->getBytes());
        // free b, then allocate d
        allocator.free(offsetB, b->getBytes());
        size_t offsetD = allocator.alloc(d->getBytes());
        // expected to be a->d->c
        EXPECT_EQ(offsetB, offsetD);
        ASSERT_FALSE(offsetA == 0 && offsetB == 0 && offsetC == 0 && offsetD == 0);
    }

    TEST(Allocator, testAllocWithEndFreeBlock)
    {
        Shape shape = Shape{1, 2, 2, 3};
        Runtime runtime = NativeCpuRuntimeObj::getInstance();
        Tensor a = make_ref<TensorObj>(shape, DataType::Float32, runtime);
        Tensor b = make_ref<TensorObj>(shape, DataType::Float32, runtime);
        Tensor c = make_ref<TensorObj>(shape, DataType::Float32, runtime);
        Tensor d =
            make_ref<TensorObj>(Shape{2, 2, 2, 3}, DataType::Float32, runtime);
        Allocator allocator = Allocator(runtime);
        // allocate a->b->c
        allocator.alloc(a->getBytes());
        allocator.alloc(b->getBytes());
        size_t offsetC = allocator.alloc(c->getBytes());
        allocator.info();
        // free c, then allocate d
        allocator.free(offsetC, c->getBytes());
        size_t offsetD = allocator.alloc(d->getBytes());
        allocator.info();
        // expected to be a->b->d, with no free block between b and c
        EXPECT_EQ(offsetC, offsetD);
    }

    TEST(Allocator, testGetPtr)
    {
        Shape shape = Shape{1, 2, 2, 3};
        Runtime runtime = NativeCpuRuntimeObj::getInstance();
        Tensor a = make_ref<TensorObj>(shape, DataType::Float32, runtime);
        Tensor b = make_ref<TensorObj>(shape, DataType::Float32, runtime);
        Tensor c = make_ref<TensorObj>(shape, DataType::Float32, runtime);
        Tensor d = make_ref<TensorObj>(shape, DataType::Float32, runtime);
        Allocator allocator = Allocator(runtime);
        // allocate a->b->c->d
        allocator.alloc(a->getBytes());
        allocator.alloc(b->getBytes());
        allocator.alloc(c->getBytes());
        allocator.alloc(d->getBytes());
        // multiple calls to the getPtr() function should return the same pointer
        void *ptr1 = allocator.getPtr();
        void *ptr2 = allocator.getPtr();
        EXPECT_EQ(ptr1, ptr2);
    }

} // namespace infini
