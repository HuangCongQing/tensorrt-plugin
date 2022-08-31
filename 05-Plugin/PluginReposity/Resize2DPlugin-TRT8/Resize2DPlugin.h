/*
 * Copyright (c) 2021-2022, NVIDIA CORPORATION. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <NvInfer.h>
#include <cuda_fp16.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#ifdef DEBUG
    #define WHERE_AM_I()                          \
        do                                        \
        {                                         \
            printf("%14p[%s]\n", this, __func__); \
        } while (0);
#else
    #define WHERE_AM_I()
#endif // ifdef DEBUG

#define CEIL_DIVIDE(X, Y) (((X) + (Y)-1) / (Y))
#define ALIGN_TO(X, Y)    (CEIL_DIVIDE(X, Y) * (Y))

namespace
{
static const char *PLUGIN_NAME {"Resize2D"};
static const char *PLUGIN_VERSION_V1 {"1"};
static const char *PLUGIN_VERSION_V2 {"2"};
} // namespace

namespace nvinfer1
{
std::string getFormatString(TensorFormat format)
{
    std::string ret;
    switch (format)
    {
    case TensorFormat::kLINEAR: ret = std::string("LINE "); break;
    case TensorFormat::kCHW2: ret = std::string("CHW2 "); break;
    case TensorFormat::kHWC8: ret = std::string("HWC8 "); break;
    case TensorFormat::kCHW4: ret = std::string("CHW4 "); break;
    case TensorFormat::kCHW16: ret = std::string("CHW16"); break;
    case TensorFormat::kCHW32: ret = std::string("CHW32"); break;
    case TensorFormat::kHWC: ret = std::string("HWC  "); break;
    case TensorFormat::kDLA_LINEAR: ret = std::string("DLINE"); break;
    case TensorFormat::kDLA_HWC4: ret = std::string("DHWC4"); break;
    case TensorFormat::kHWC16: ret = std::string("HWC16"); break;
    default: ret = std::string("None ");
    }
    return ret;
}

std::string getDataTypeString(DataType type)
{
    std::string ret;
    switch (type)
    {
    case DataType::kFLOAT: ret = std::string("FP32 "); break;
    case DataType::kHALF: ret = std::string("FP16 "); break;
    case DataType::kINT8: ret = std::string("INT8 "); break;
    case DataType::kINT32: ret = std::string("INT32"); break;
    case DataType::kBOOL: ret = std::string("BOOL "); break;
    default: ret = std::string("None ");
    }
    return ret;
}

class Resize2DPluginV1 : public IPluginV2DynamicExt
{
private:
    const std::string name_;
    std::string       namespace_;
    struct
    {
        int nMode;
        int nScale;
        int nH0;
        int nW0;
        int nH1;
        int nW1;
    } m_;

public:
    Resize2DPluginV1() = delete;
    Resize2DPluginV1(const std::string &name, int nMode, int nScale = 0.0f, int nH1 = 0, int nW1 = 0);
    Resize2DPluginV1(const std::string &name, const void *buffer, size_t length);
    ~Resize2DPluginV1();

    // Method inherited from IPluginV2
    const char *getPluginType() const noexcept override;
    const char *getPluginVersion() const noexcept override;
    int32_t     getNbOutputs() const noexcept override;
    int32_t     initialize() noexcept override;
    void        terminate() noexcept override;
    size_t      getSerializationSize() const noexcept override;
    void        serialize(void *buffer) const noexcept override;
    void        destroy() noexcept override;
    void        setPluginNamespace(const char *pluginNamespace) noexcept override;
    const char *getPluginNamespace() const noexcept override;

    // Method inherited from IPluginV2Ext
    DataType getOutputDataType(int32_t index, DataType const *inputTypes, int32_t nbInputs) const noexcept override;
    void     attachToContext(cudnnContext *contextCudnn, cublasContext *contextCublas, IGpuAllocator *gpuAllocator) noexcept override;
    void     detachFromContext() noexcept override;

    //Method inherited from IPluginV2DynamicExt
    IPluginV2DynamicExt *clone() const noexcept override;
    DimsExprs            getOutputDimensions(int32_t outputIndex, const DimsExprs *inputs, int32_t nbInputs, IExprBuilder &exprBuilder) noexcept override;
    bool                 supportsFormatCombination(int32_t pos, const PluginTensorDesc *inOut, int32_t nbInputs, int32_t nbOutputs) noexcept override;
    void                 configurePlugin(const DynamicPluginTensorDesc *in, int32_t nbInputs, const DynamicPluginTensorDesc *out, int32_t nbOutputs) noexcept override;
    size_t               getWorkspaceSize(const PluginTensorDesc *inputs, int32_t nbInputs, const PluginTensorDesc *outputs, int32_t nbOutputs) const noexcept override;
    int32_t              enqueue(const PluginTensorDesc *inputDesc, const PluginTensorDesc *outputDesc, const void *const *inputs, void *const *outputs, void *workspace, cudaStream_t stream) noexcept override;
};

class Resize2DPluginV1Creator : public IPluginCreator
{
private:
    static PluginFieldCollection    fc_;
    static std::vector<PluginField> attr_;
    std::string                     namespace_;

public:
    Resize2DPluginV1Creator();
    ~Resize2DPluginV1Creator();
    const char *                 getPluginName() const noexcept override;
    const char *                 getPluginVersion() const noexcept override;
    const PluginFieldCollection *getFieldNames() noexcept override;
    IPluginV2 *                  createPlugin(const char *name, const PluginFieldCollection *fc) noexcept override;
    IPluginV2 *                  deserializePlugin(const char *name, const void *serialData, size_t serialLength) noexcept override;
    void                         setPluginNamespace(const char *pluginNamespace) noexcept override;
    const char *                 getPluginNamespace() const noexcept override;
};

class Resize2DPluginV2 : public IPluginV2DynamicExt
{
private:
    const std::string name_;
    std::string       namespace_;
    struct
    {
        int nMode;
        int nScale;
        int nH0;
        int nW0;
        int nH1;
        int nW1;
    } m_;

public:
    Resize2DPluginV2() = delete;
    Resize2DPluginV2(const std::string &name, int nMode, int nScale = 0.0f, int nH1 = 0, int nW1 = 0);
    Resize2DPluginV2(const std::string &name, const void *buffer, size_t length);
    ~Resize2DPluginV2();

    // Method inherited from IPluginV2
    const char *getPluginType() const noexcept override;
    const char *getPluginVersion() const noexcept override;
    int32_t     getNbOutputs() const noexcept override;
    int32_t     initialize() noexcept override;
    void        terminate() noexcept override;
    size_t      getSerializationSize() const noexcept override;
    void        serialize(void *buffer) const noexcept override;
    void        destroy() noexcept override;
    void        setPluginNamespace(const char *pluginNamespace) noexcept override;
    const char *getPluginNamespace() const noexcept override;

    // Method inherited from IPluginV2Ext
    DataType getOutputDataType(int32_t index, DataType const *inputTypes, int32_t nbInputs) const noexcept override;
    void     attachToContext(cudnnContext *contextCudnn, cublasContext *contextCublas, IGpuAllocator *gpuAllocator) noexcept override;
    void     detachFromContext() noexcept override;

    //Method inherited from IPluginV2DynamicExt
    IPluginV2DynamicExt *clone() const noexcept override;
    DimsExprs            getOutputDimensions(int32_t outputIndex, const DimsExprs *inputs, int32_t nbInputs, IExprBuilder &exprBuilder) noexcept override;
    bool                 supportsFormatCombination(int32_t pos, const PluginTensorDesc *inOut, int32_t nbInputs, int32_t nbOutputs) noexcept override;
    void                 configurePlugin(const DynamicPluginTensorDesc *in, int32_t nbInputs, const DynamicPluginTensorDesc *out, int32_t nbOutputs) noexcept override;
    size_t               getWorkspaceSize(const PluginTensorDesc *inputs, int32_t nbInputs, const PluginTensorDesc *outputs, int32_t nbOutputs) const noexcept override;
    int32_t              enqueue(const PluginTensorDesc *inputDesc, const PluginTensorDesc *outputDesc, const void *const *inputs, void *const *outputs, void *workspace, cudaStream_t stream) noexcept override;
};

class Resize2DPluginV2Creator : public IPluginCreator
{
private:
    static PluginFieldCollection    fc_;
    static std::vector<PluginField> attr_;
    std::string                     namespace_;

public:
    Resize2DPluginV2Creator();
    ~Resize2DPluginV2Creator();
    const char *                 getPluginName() const noexcept override;
    const char *                 getPluginVersion() const noexcept override;
    const PluginFieldCollection *getFieldNames() noexcept override;
    IPluginV2 *                  createPlugin(const char *name, const PluginFieldCollection *fc) noexcept override;
    IPluginV2 *                  deserializePlugin(const char *name, const void *serialData, size_t serialLength) noexcept override;
    void                         setPluginNamespace(const char *pluginNamespace) noexcept override;
    const char *                 getPluginNamespace() const noexcept override;
};

} // namespace nvinfer1
