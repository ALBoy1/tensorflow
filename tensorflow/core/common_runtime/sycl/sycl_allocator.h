/* Copyright 2016 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#if !TENSORFLOW_USE_SYCL
#error This file must only be included when building TensorFlow with SYCL support
#endif

#ifndef TENSORFLOW_COMMON_RUNTIME_SYCL_SYCL_ALLOCATOR_H_
#define TENSORFLOW_COMMON_RUNTIME_SYCL_SYCL_ALLOCATOR_H_

#include "tensorflow/core/framework/allocator.h"
#include "tensorflow/core/platform/mutex.h"
#include "tensorflow/core/platform/types.h"
#include "third_party/eigen3/unsupported/Eigen/CXX11/Tensor"

namespace tensorflow {

class SYCLAllocator : public Allocator {
 public:
  SYCLAllocator(Eigen::QueueInterface *queue);
  virtual ~SYCLAllocator() override;
  string Name() override;
  void *AllocateRaw(size_t alignment, size_t num_bytes) override;
  void DeallocateRaw(void *ptr) override;

  virtual bool ShouldAllocateEmptyTensors() override final { return true; }
  void Synchronize() { sycl_device_->synchronize(); }
  bool Ok() { return sycl_device_->ok(); }
  void GetStats(AllocatorStats* stats) override;
  // The SYCL buffers keep track of their size, so we already have tracking.
  bool TracksAllocationSizes() override { return true; }
  // Get the size of the corresponding SYCL buffer.
  // Implementing this also provides an implementation of
  // AllocatedSize(void* ptr) by default.
  size_t RequestedSize(void* ptr) override;
  Eigen::SyclDevice* getSyclDevice() { return sycl_device_; }
 private:
  Eigen::SyclDevice *sycl_device_;  // owned

  mutable mutex mu_;
  AllocatorStats stats_ GUARDED_BY(mu_);

  TF_DISALLOW_COPY_AND_ASSIGN(SYCLAllocator);
};

}  // namespace tensorflow

#endif  // TENSORFLOW_COMMON_RUNTIME_SYCL_SYCL_ALLOCATOR_H_
