//
// Copyright © 2017 Arm Ltd. All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "backends/RefWorkloadFactory.hpp"
#include "backends/RefWorkloads.hpp"
#include "backends/CpuTensorHandle.hpp"

#include "test/CreateWorkload.hpp"

namespace
{

template<typename Workload>
void CheckInputOutput(std::unique_ptr<Workload> workload, const TensorInfo& inputInfo, const TensorInfo& outputInfo)
{
    auto queueDescriptor = workload->GetData();
    auto inputHandle  = boost::polymorphic_downcast<ConstCpuTensorHandle*>(queueDescriptor.m_Inputs[0]);
    auto outputHandle = boost::polymorphic_downcast<CpuTensorHandle*>(queueDescriptor.m_Outputs[0]);
    BOOST_TEST((inputHandle->GetTensorInfo() == inputInfo));
    BOOST_TEST((outputHandle->GetTensorInfo() == outputInfo));
}

template <typename Workload>
void CheckInputsOutput(std::unique_ptr<Workload> workload,
                       const TensorInfo&         inputInfo0,
                       const TensorInfo&         inputInfo1,
                       const TensorInfo&         outputInfo)
{
    auto queueDescriptor = workload->GetData();
    auto inputHandle0     = boost::polymorphic_downcast<ConstCpuTensorHandle*>(queueDescriptor.m_Inputs[0]);
    auto inputHandle1     = boost::polymorphic_downcast<ConstCpuTensorHandle*>(queueDescriptor.m_Inputs[1]);
    auto outputHandle    = boost::polymorphic_downcast<CpuTensorHandle*>(queueDescriptor.m_Outputs[0]);
    BOOST_TEST((inputHandle0->GetTensorInfo() == inputInfo0));
    BOOST_TEST((inputHandle1->GetTensorInfo() == inputInfo1));
    BOOST_TEST((outputHandle->GetTensorInfo() == outputInfo));
}
}

BOOST_AUTO_TEST_SUITE(CreateWorkloadRef)

template <typename ActivationWorkloadType>
static void RefCreateActivationWorkloadTest()
{
    Graph graph;
    RefWorkloadFactory factory;
    auto workload = CreateActivationWorkloadTest<ActivationWorkloadType>(factory, graph);

    // check that outputs are as we expect them (see definition of CreateActivationWorkloadTest)
    CheckInputOutput(std::move(workload),
        TensorInfo({ 1, 1 }, ActivationWorkloadType::ms_DataType),
        TensorInfo({ 1, 1 }, ActivationWorkloadType::ms_DataType));
}

BOOST_AUTO_TEST_CASE(CreateActivationFloat32Workload)
{
    RefCreateActivationWorkloadTest<RefActivationFloat32Workload>();
}

BOOST_AUTO_TEST_CASE(CreateActivationUint8Workload)
{
    RefCreateActivationWorkloadTest<RefActivationUint8Workload>();
}

template <typename AdditionWorkloadType>
static void RefCreateAdditionWorkloadTest()
{
    Graph graph;
    RefWorkloadFactory factory;
    auto workload = CreateAdditionWorkloadTest<AdditionWorkloadType>(factory, graph);

    // check that outputs are as we expect them (see definition of CreateAdditionWorkloadTest)
    CheckInputsOutput(std::move(workload),
        TensorInfo({ 2, 3 }, AdditionWorkloadType::ms_DataType),
        TensorInfo({ 2, 3 }, AdditionWorkloadType::ms_DataType),
        TensorInfo({ 2, 3 }, AdditionWorkloadType::ms_DataType));
}

BOOST_AUTO_TEST_CASE(CreateAdditionFloatWorkload)
{
    RefCreateAdditionWorkloadTest<RefAdditionFloat32Workload>();
}

BOOST_AUTO_TEST_CASE(CreateAdditionUint8Workload)
{
    RefCreateAdditionWorkloadTest<RefAdditionUint8Workload>();
}

BOOST_AUTO_TEST_CASE(CreateBatchNormalizationWorkload)
{
    Graph                graph;
    RefWorkloadFactory factory;
    auto workload = CreateBatchNormalizationWorkloadTest<RefBatchNormalizationFloat32Workload>(factory, graph);

    // check that outputs and inputs are as we expect them (see definition of CreateBatchNormalizationWorkloadTest)
    CheckInputOutput(
        std::move(workload), TensorInfo({2, 3, 1, 1}, DataType::Float32), TensorInfo({2, 3, 1, 1}, DataType::Float32));
}

BOOST_AUTO_TEST_CASE(CreateConvolution2dWorkload)
{
    Graph                graph;
    RefWorkloadFactory factory;
    auto                 workload = CreateConvolution2dWorkloadTest<RefConvolution2dFloat32Workload>(factory, graph);

    // check that outputs and inputs are as we expect them (see definition of CreateConvolution2dWorkloadTest)
    CheckInputOutput(std::move(workload),
                     TensorInfo({2, 3, 8, 16}, DataType::Float32),
                     TensorInfo({2, 2, 2, 10}, DataType::Float32));
}

BOOST_AUTO_TEST_CASE(CreateDepthwiseConvolution2dWorkload)
{
    Graph                graph;
    RefWorkloadFactory factory;
    auto                 workload =
        CreateDepthwiseConvolution2dWorkloadTest<RefDepthwiseConvolution2dFloat32Workload>(factory, graph);

    // check that outputs and inputs are as we expect them (see definition of CreateConvolution2dWorkloadTest)
    CheckInputOutput(std::move(workload),
                     TensorInfo({2, 3, 8, 16}, DataType::Float32),
                     TensorInfo({2, 9, 2, 10}, DataType::Float32));
}

template <typename FullyConnectedWorkloadType>
static void RefCreateFullyConnectedWorkloadTest()
{
    Graph graph;
    RefWorkloadFactory factory;
    auto workload = CreateFullyConnectedWorkloadTest<FullyConnectedWorkloadType>(factory, graph);

    // check that outputs and inputs are as we expect them (see definition of CreateFullyConnectedWorkloadTest)
    float inputsQScale = FullyConnectedWorkloadType::ms_DataType == DataType::QuantisedAsymm8 ? 1.0f : 0.0;
    float outputQScale = FullyConnectedWorkloadType::ms_DataType == DataType::QuantisedAsymm8 ? 2.0f : 0.0;
    CheckInputOutput(std::move(workload),
        TensorInfo({ 3, 1, 4, 5 }, FullyConnectedWorkloadType::ms_DataType, inputsQScale),
        TensorInfo({ 3, 7 }, FullyConnectedWorkloadType::ms_DataType, outputQScale));
}

BOOST_AUTO_TEST_CASE(CreateFullyConnectedFloat32Workload)
{
    RefCreateFullyConnectedWorkloadTest<RefFullyConnectedFloat32Workload>();
}

BOOST_AUTO_TEST_CASE(CreateFullyConnectedUint8Workload)
{
    RefCreateFullyConnectedWorkloadTest<RefFullyConnectedUint8Workload>();
}

template <typename MultiplicationWorkloadType>
static void RefCreateMultiplicationWorkloadTest()
{
    Graph graph;
    RefWorkloadFactory factory;
    auto workload = CreateMultiplicationWorkloadTest<MultiplicationWorkloadType>(factory, graph);

    // check that outputs are as we expect them (see definition of CreateMultiplicationWorkloadTest)
    CheckInputsOutput(std::move(workload),
        TensorInfo({ 2, 3 }, MultiplicationWorkloadType::ms_DataType),
        TensorInfo({ 2, 3 }, MultiplicationWorkloadType::ms_DataType),
        TensorInfo({ 2, 3 }, MultiplicationWorkloadType::ms_DataType));
}

BOOST_AUTO_TEST_CASE(CreateMultiplicationFloatWorkload)
{
    RefCreateMultiplicationWorkloadTest<RefMultiplicationFloat32Workload>();
}

BOOST_AUTO_TEST_CASE(CreateMultiplicationUint8Workload)
{
    RefCreateMultiplicationWorkloadTest<RefMultiplicationUint8Workload>();
}

BOOST_AUTO_TEST_CASE(CreateNormalizationWorkload)
{
    Graph                graph;
    RefWorkloadFactory factory;
    auto                 workload = CreateNormalizationWorkloadTest<RefNormalizationFloat32Workload>(factory, graph);

    // check that outputs and inputs are as we expect them (see definition of CreateNormalizationWorkloadTest)
    CheckInputOutput(std::move(workload),
                     TensorInfo({3, 5, 5, 1}, DataType::Float32),
                     TensorInfo({3, 5, 5, 1}, DataType::Float32));
}

template <typename Pooling2dWorkloadType>
static void RefCreatePooling2dWorkloadTest()
{
    Graph graph;
    RefWorkloadFactory factory;
    auto workload = CreatePooling2dWorkloadTest<Pooling2dWorkloadType>(factory, graph);

    // check that outputs and inputs are as we expect them (see definition of CreatePooling2dWorkloadTest)
    CheckInputOutput(
        std::move(workload),
        TensorInfo({3, 2, 5, 5}, Pooling2dWorkloadType::ms_DataType),
        TensorInfo({3, 2, 2, 4}, Pooling2dWorkloadType::ms_DataType));
}

BOOST_AUTO_TEST_CASE(CreatePooling2dFloat32Workload)
{
    RefCreatePooling2dWorkloadTest<RefPooling2dFloat32Workload>();
}

BOOST_AUTO_TEST_CASE(CreatePooling2dUint8Workload)
{
    RefCreatePooling2dWorkloadTest<RefPooling2dUint8Workload>();
}

template <typename SoftmaxWorkloadType>
static void RefCreateSoftmaxWorkloadTest()
{
    Graph graph;
    RefWorkloadFactory factory;
    auto workload = CreateSoftmaxWorkloadTest<SoftmaxWorkloadType>(factory, graph);

    // check that outputs and inputs are as we expect them (see definition of CreateSoftmaxWorkloadTest)
    CheckInputOutput(
        std::move(workload),
        TensorInfo({4, 1}, SoftmaxWorkloadType::ms_DataType),
        TensorInfo({4, 1}, SoftmaxWorkloadType::ms_DataType));
}

BOOST_AUTO_TEST_CASE(CreateSoftmaxFloat32Workload)
{
    RefCreateSoftmaxWorkloadTest<RefSoftmaxFloat32Workload>();
}

BOOST_AUTO_TEST_CASE(CreateSoftmaxUint8Workload)
{
    RefCreateSoftmaxWorkloadTest<RefSoftmaxUint8Workload>();
}

template <typename SplitterWorkloadType>
static void RefCreateSplitterWorkloadTest()
{
    Graph graph;
    RefWorkloadFactory factory;
    auto workload = CreateSplitterWorkloadTest<SplitterWorkloadType>(factory, graph);

    // check that outputs are as we expect them (see definition of CreateSplitterWorkloadTest)
    SplitterQueueDescriptor queueDescriptor = workload->GetData();
    auto inputHandle = boost::polymorphic_downcast<ConstCpuTensorHandle*>(queueDescriptor.m_Inputs[0]);
    BOOST_TEST((inputHandle->GetTensorInfo() == TensorInfo({ 5, 7, 7 }, SplitterWorkloadType::ms_DataType)));

    auto outputHandle0 = boost::polymorphic_downcast<CpuTensorHandle*>(queueDescriptor.m_Outputs[0]);
    BOOST_TEST((outputHandle0->GetTensorInfo() == TensorInfo({ 1, 7, 7 }, SplitterWorkloadType::ms_DataType)));

    auto outputHandle1 = boost::polymorphic_downcast<CpuTensorHandle*>(queueDescriptor.m_Outputs[1]);
    BOOST_TEST((outputHandle1->GetTensorInfo() == TensorInfo({ 2, 7, 7 }, SplitterWorkloadType::ms_DataType)));

    auto outputHandle2 = boost::polymorphic_downcast<CpuTensorHandle*>(queueDescriptor.m_Outputs[2]);
    BOOST_TEST((outputHandle2->GetTensorInfo() == TensorInfo({ 2, 7, 7 }, SplitterWorkloadType::ms_DataType)));
}

BOOST_AUTO_TEST_CASE(CreateSplitterFloat32Workload)
{
    RefCreateSplitterWorkloadTest<RefSplitterFloat32Workload>();
}

BOOST_AUTO_TEST_CASE(CreateSplitterUint8Workload)
{
    RefCreateSplitterWorkloadTest<RefSplitterUint8Workload>();
}

template <typename SplitterWorkloadType, typename MergerWorkloadType>
static void RefCreateSplitterMergerWorkloadTest()
{
    // Test that it is possible to decide which output of the splitter layer
    // should be lined to which input of the merger layer
    // We test that is is possible to specify 0th output
    // of the splitter to be the 1st input to the merger and the 1st output of the splitter  to be 0th input
    // of the merger.

    Graph graph;
    RefWorkloadFactory factory;
    auto workloads = CreateSplitterMergerWorkloadTest<SplitterWorkloadType, MergerWorkloadType>(factory, graph);

    auto wlSplitter = std::move(workloads.first);
    auto wlMerger = std::move(workloads.second);

    //check that the index of inputs/outputs matches what we declared on InputDescriptor construction.
    armnn::CpuTensorHandle* sOut0 = dynamic_cast<armnn::CpuTensorHandle*>(wlSplitter->GetData().m_Outputs[0]);
    armnn::CpuTensorHandle* sOut1 = dynamic_cast<armnn::CpuTensorHandle*>(wlSplitter->GetData().m_Outputs[1]);
    armnn::CpuTensorHandle* mIn0 = dynamic_cast<armnn::CpuTensorHandle*>(wlMerger->GetData().m_Inputs[0]);
    armnn::CpuTensorHandle* mIn1 = dynamic_cast<armnn::CpuTensorHandle*>(wlMerger->GetData().m_Inputs[1]);

    BOOST_TEST(sOut0);
    BOOST_TEST(sOut1);
    BOOST_TEST(mIn0);
    BOOST_TEST(mIn1);

    bool validDataPointers = (sOut0 == mIn1) && (sOut1 == mIn0);

    BOOST_TEST(validDataPointers);
}

BOOST_AUTO_TEST_CASE(CreateSplitterMergerFloat32)
{
    RefCreateSplitterMergerWorkloadTest<RefSplitterFloat32Workload, RefMergerFloat32Workload>();
}

BOOST_AUTO_TEST_CASE(CreateSplitterMergerUint8)
{
    RefCreateSplitterMergerWorkloadTest<RefSplitterUint8Workload, RefMergerUint8Workload>();
}

template <typename SplitterWorkloadType, typename ActivationWorkloadType>
static void RefCreateSingleOutputMultipleInputsTest()
{
    // Test that it is possible to assign multiple (two) different layers to each of the outputs of a splitter layer.
    // We create a splitter with two outputs. That each of those outputs is used by two different activation layers

    Graph graph;
    RefWorkloadFactory factory;
    std::unique_ptr<SplitterWorkloadType> wlSplitter;
    std::unique_ptr<ActivationWorkloadType> wlActiv0_0;
    std::unique_ptr<ActivationWorkloadType> wlActiv0_1;
    std::unique_ptr<ActivationWorkloadType> wlActiv1_0;
    std::unique_ptr<ActivationWorkloadType> wlActiv1_1;

    CreateSplitterMultipleInputsOneOutputWorkloadTest<SplitterWorkloadType,
        ActivationWorkloadType>(factory, graph, wlSplitter, wlActiv0_0, wlActiv0_1, wlActiv1_0, wlActiv1_1);

    armnn::CpuTensorHandle* sOut0 = dynamic_cast<armnn::CpuTensorHandle*>(wlSplitter->GetData().m_Outputs[0]);
    armnn::CpuTensorHandle* sOut1 = dynamic_cast<armnn::CpuTensorHandle*>(wlSplitter->GetData().m_Outputs[1]);
    armnn::CpuTensorHandle* activ0_0Im = dynamic_cast<armnn::CpuTensorHandle*>(wlActiv0_0->GetData().m_Inputs[0]);
    armnn::CpuTensorHandle* activ0_1Im = dynamic_cast<armnn::CpuTensorHandle*>(wlActiv0_1->GetData().m_Inputs[0]);
    armnn::CpuTensorHandle* activ1_0Im = dynamic_cast<armnn::CpuTensorHandle*>(wlActiv1_0->GetData().m_Inputs[0]);
    armnn::CpuTensorHandle* activ1_1Im = dynamic_cast<armnn::CpuTensorHandle*>(wlActiv1_1->GetData().m_Inputs[0]);


    BOOST_TEST(sOut0);
    BOOST_TEST(sOut1);
    BOOST_TEST(activ0_0Im);
    BOOST_TEST(activ0_1Im);
    BOOST_TEST(activ1_0Im);
    BOOST_TEST(activ1_1Im);

    bool validDataPointers = (sOut0 == activ0_0Im) && (sOut0 == activ0_1Im) &&
                             (sOut1 == activ1_0Im) && (sOut1 == activ1_1Im);

    BOOST_TEST(validDataPointers);
}

BOOST_AUTO_TEST_CASE(CreateSingleOutputMultipleInputsFloat32)
{
    RefCreateSingleOutputMultipleInputsTest<RefSplitterFloat32Workload, RefActivationFloat32Workload>();
}

BOOST_AUTO_TEST_CASE(CreateSingleOutputMultipleInputsUint8)
{
    RefCreateSingleOutputMultipleInputsTest<RefSplitterUint8Workload, RefActivationUint8Workload>();
}

template <typename ResizeBilinearWorkloadType>
static void RefCreateResizeBilinearTest()
{
    Graph graph;
    RefWorkloadFactory factory;
    auto workload = CreateResizeBilinearWorkloadTest<ResizeBilinearWorkloadType>(factory, graph);

    // check that outputs and inputs are as we expect them (see definition of CreateResizeBilinearWorkloadTest)
    CheckInputOutput(
        std::move(workload),
        TensorInfo({ 2, 3, 4, 4 }, ResizeBilinearWorkloadType::ms_DataType),
        TensorInfo({ 2, 3, 2, 2 }, ResizeBilinearWorkloadType::ms_DataType));
}

BOOST_AUTO_TEST_CASE(CreateResizeBilinearFloat32)
{
    RefCreateResizeBilinearTest<RefResizeBilinearFloat32Workload>();
}

BOOST_AUTO_TEST_CASE(CreateResizeBilinearUint8)
{
    RefCreateResizeBilinearTest<RefResizeBilinearUint8Workload>();
}

BOOST_AUTO_TEST_CASE(CreateL2NormalizationFloat32)
{
    Graph graph;
    RefWorkloadFactory factory;
    auto workload = CreateL2NormalizationWorkloadTest<RefL2NormalizationFloat32Workload>(factory, graph);

    // check that outputs and inputs are as we expect them (see definition of CreateL2NormalizationWorkloadTest)
    CheckInputOutput(
        std::move(workload),
        TensorInfo({ 5, 20, 50, 67 }, RefL2NormalizationFloat32Workload::ms_DataType),
        TensorInfo({ 5, 20, 50, 67 }, RefL2NormalizationFloat32Workload::ms_DataType));
}

template <typename ReshapeWorkloadType>
static void RefCreateReshapeWorkloadTest()
{
    Graph graph;
    RefWorkloadFactory factory;
    auto workload = CreateReshapeWorkloadTest<ReshapeWorkloadType>(factory, graph);

    // check that outputs and inputs are as we expect them (see definition of CreateReshapeWorkloadTest)
    CheckInputOutput(
        std::move(workload),
        TensorInfo({ 4, 1 }, ReshapeWorkloadType::ms_DataType),
        TensorInfo({ 1, 4 }, ReshapeWorkloadType::ms_DataType));
}

BOOST_AUTO_TEST_CASE(CreateReshapeFloat32Workload)
{
    RefCreateReshapeWorkloadTest<RefReshapeFloat32Workload>();
}

BOOST_AUTO_TEST_CASE(CreateReshapeUint8Workload)
{
    RefCreateReshapeWorkloadTest<RefReshapeUint8Workload>();
}

BOOST_AUTO_TEST_SUITE_END()
