include ../../Makefile.inc

LD_LZ       = -lz
SOURCE_CU   = $(shell find . -name '*.cu' 2>/dev/null) # ./LoadNpzPlugin.cu  2>指代错误输出
SOURCE_PY   = $(shell find . -name 'test*.py' 2>/dev/null) # 测试py文件路径
OBJ         = $(shell find . -name *.o 2>/dev/null)
DEP         = $(OBJ:.o=.d)
TARGET_SO   = $(SOURCE_CU:.cu=.so) # so文件

-include $(DEP)

all: $(TARGET_SO)

%.so: %.o cnpy.o
	$(NVCC) $(SOFLAG) $(LDFLAG) $(LD_LZ) -o $@ $+

%.o: %.cu
	$(NVCC) $(CUFLAG) $(INCLUDE) -M -MT $@ -o $(@:.o=.d) $<
	$(NVCC) $(CUFLAG) $(INCLUDE) -o $@ -c $<

cnpy.o: cnpy.cpp
	$(NVCC) $(CCFLAG) $(INCLUDE) -M -MT $@ -o $(@:.o=.d) $<
	$(NVCC) $(CCFLAG) $(INCLUDE) -o $@ -c $<

.PHONY: test
test:
	make clean
	make -j2
	python3 createData.py # 执行文件
	python3 $(SOURCE_PY)

.PHONY: clean
clean:
	rm -rf ./*.d ./*.o ./*.so ./*.exe ./*.plan ./*.npz

