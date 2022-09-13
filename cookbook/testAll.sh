#!/bin/bash
set -e
set -x

tf=`pip list |grep "tensorflow \|tensorflow-gpu"`
tfVersion=${tf#* }
tfMajorVersion=${tfVersion%%\.*}
pt=`pip list|grep "^torch\ "`
pd=`pip list|grep "^paddlepaddle-gpu\ "`

if false; then
# 00 ---------------------------------------------------------------------------
echo "[00-MNISTData] Start"

cd 00-MNISTData
python3 extractMnistData.py
cd ..

echo "[00-MNISTData] Finish"

# 01 ---------------------------------------------------------------------------
echo "[01-SimpleDemo] Start"

cd 01-SimpleDemo

#cd TensorRT6
#make test > result.log
#cd ..

#cd TensorRT7
#make test > result.log
#cd ..

#cd TensorRT8
#make test > result.log
#cd ..

cd TensorRT8.4
make test > result.log
cd ..

cd ..
echo "[01-SimpleDemo] Finish"

# 02 ---------------------------------------------------------------------------
echo "[02-API] Start"

cd 02-API

cd CudaEngine
python3 main.py > result.log
cd ..

cd Int8-QDQ
python3 main.py > result.log
cd ..

cd Layer
python3 testAllLayer.py
cd ..

cd PrintNetwork
python3 main.py > result.log
cd ..

cd ..
echo "[02-API] Finish"

# 03 ---------------------------------------------------------------------------
echo "[03-APIModel] Start"
cd 03-APIModel

if [[ $pt ]]; then
cd MNISTExample-pyTorch
python3 main.py > result.log
cd C++
make test 
cd ../..
fi

if [ $tfMajorVersion = "1" ]; then
cd MNISTExample-TensorFlow1
python3 main.py > result.log
cd ..

cd TensorFlow1
python3 Convolution.py > result-Convolution.log
python3 FullyConnected.py > result-FullyConnected.log
python3 RNN-LSTM.py > result-RNN-LSTM.log
cd ..
fi

if [ $tfMajorVersion = "2" ]; then
cd MNISTExample-TensorFlow2
python3 main.py > result.log
cd ..
fi

if [[ $pd ]]; then
cd MNISTExample-Paddlepaddle
python3 main.py > result.log
cd ..
fi

cd ..
echo "[03-APIModel] Finish"

# 04 ---------------------------------------------------------------------------
echo "[04-Parser] Start"
cd 04-Parser

if [[ $pt ]]; then
cd pyTorch-ONNX-TensorRT
python3 main.py > result.log
cd ..

cd pyTorch-ONNX-TensorRT-QAT
python main.py > result.log
cd ..
fi 

if [ $tfMajorVersion = "1" ]; then
cd TensorFlow1-ONNX-TensorRT
python3 main-NCHW.py > result-NCHW.log
python3 main-NHWC.py > result-NHWC.log
python3 main-NHWC-C2.py > result-NHWC-C2.log
cd ..

cd TensorFlow1-ONNX-TensorRT-QAT
python3 main.py > result.log
cd ..

cd TensorFlow1-UFF-TensorRT
python3 main.py > result.log
cd ..

#cd TensorFlow1-Caffe-TensorRT
#python buildModelInTensorFlow.py > result.log
#python runModelInTensorRT.py >> result.log
#cd ..
fi 

if [ $tfMajorVersion = "2" ]; then
cd TensorFlow2-ONNX-TensorRT
python main-NCHW.py > result-NCHW.log
python main-NHWC.py > result-NHWC.log
python main-NHWC-C2.py > result-NHWC-C2.log
cd ..

#cd TensorFlow2-ONNX-TensorRT-QAT
#python main.py > result.log
#cd ..
fi

if [[ $pd ]]; then
cd Paddlepaddle-ONNX-TensorRT
python3 main.py > result.log
cd ..
fi

cd ..
echo "[04-Parser] Finish"

# 05 ---------------------------------------------------------------------------
echo "[05-Plugin] Start"
cd 05-Plugin

cd loadNpz
make test > result.log
cd ..

cd multipleVersion
make test > result.log
cd ..

cd PluginProcess
make test > result.log
cd ..

cd useCuBLAS
make test > result.log
cd ..

cd useFP16
make test > result.log
cd ..

cd useINT8-PTQ
make test > result.log
cd ..

cd usePluginV2Ext
make test > result.log
cd ..

cd usePluginV2IOExt
make test > result.log
cd ..

cd usePluginV2DynamicExt
make test > result.log
cd ..

cd ..
echo "[05-Plugin] Finish"

# 06 ---------------------------------------------------------------------------
echo "[06-PluginAndParser] Start"
cd 06-PluginAndParser

if [[ $pt ]]; then
cd pyTorch-FailConvertNonZero
python3 main.py > result.log 2>&1

cd pyTorch-LayerNorm
make test > result.log
cd ..
fi

if [ $tfMajorVersion = "1" ]; then
cd TensorFlow1-AddScalar
python3 main.py > result.log
fi

if [ $tfMajorVersion = "2" ]; then
cd TensorFlow2-AddScalar
make test > result.log
cd ..
fi

cd ..
echo "[06-PluginAndParser] Finish"

# 07 ---------------------------------------------------------------------------
echo "[07-FrameworkTRT] Start"
cd 07-FrameworkTRT

echo "fuck"

cd ..
echo "[07-FrameworkTRT] Finish"

# 08 ---------------------------------------------------------------------------
echo "[08-Tool] Start"
cd 08-Tool

cd NsightSystems
chmod +x ./command.sh
./command.sh > result.log
cd ..

cd OnnxGraphSurgeon
chmod +x ./command.sh
./command.sh > result.log
cd ..

cd OnnxRuntime
python3 main.py > result.log
cd ..

cd Polygraphy

    cd convertExample
    chmod +x command.sh
    ./command.sh > result.log
    cd ..

    cd debugExample
    chmod +x command.sh
    ./command.sh > result.log 2>&1
    cd ..

    cd inspectExample
    chmod +x command.sh
    ./command.sh > result.log
    cd ..

    cd runExample
    chmod +x command.sh
    ./command.sh > result.log
    cd ..

    cd surgeonExample
    chmod +x command.sh
    ./command.sh > result.log
    cd ..

    cd templateExample
    chmod +x command.sh
    ./command.sh > result.log
    cd ..

    cd ..

# 跳过 trex 的范例

cd trtexec
chmod +x ./command.sh
./command.sh > result.log
cd ..

cd ..
echo "[08-Tool] Finish"
fi # 跳过不运行部分的 if
# 08 ---------------------------------------------------------------------------
echo "[09-Advance] Start"
cd 09-Advance
if false; then
cd AlgorithmSelector
python3 main.py > result.log
cd ..

cd CudaGraph
make test > result.log
cd ..

cd EngineInspector
python3 main.py > result.log
cd ..

cd Logger
python3 main.py > result.log
cd ..

# Multicontext 可能有点问题
cd MultiContext
python3 MultiContext.py > result.log
python3 MultiContext+CudaGraph.py > result.log
cd ..

# MultiOptimizationProfile 可能有点问题
cd MultiOptimizationProfile
python3 MultiOptimizationProfile.py > result.log
python3 MultiOptimizationProfile+CudaGraph.py > result.log
cd ..

cd MultiStream
python3 main.py > result.log
cd ..

cd nvtx
make test > result.log
cd ..


cd Profiling
python3 main.py > result.log
cd ..

cd ProfilingVerbosity
python3 main.py > result.log
cd ..
fi



cd ..
echo "[09-Advance] Finish"
