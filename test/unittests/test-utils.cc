// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "test/unittests/test-utils.h"

#include "src/base/platform/time.h"
#include "src/flags.h"
#include "src/isolate-inl.h"

namespace v8 {

// static
Isolate* TestWithIsolate::isolate_ = NULL;


TestWithIsolate::TestWithIsolate()
    : isolate_scope_(isolate()), handle_scope_(isolate()) {}


TestWithIsolate::~TestWithIsolate() {}


// static
void TestWithIsolate::SetUpTestCase() {
  Test::SetUpTestCase();
  EXPECT_EQ(NULL, isolate_);
  isolate_ = v8::Isolate::New();
  EXPECT_TRUE(isolate_ != NULL);
}


// static
void TestWithIsolate::TearDownTestCase() {
  ASSERT_TRUE(isolate_ != NULL);
  isolate_->Dispose();
  isolate_ = NULL;
  Test::TearDownTestCase();
}


TestWithContext::TestWithContext()
    : context_(Context::New(isolate())), context_scope_(context_) {}


TestWithContext::~TestWithContext() {}


namespace base {
namespace {

inline int64_t GetRandomSeedFromFlag(int random_seed) {
  return random_seed ? random_seed : TimeTicks::Now().ToInternalValue();
}

}  // namespace

TestWithRandomNumberGenerator::TestWithRandomNumberGenerator()
    : rng_(GetRandomSeedFromFlag(internal::FLAG_random_seed)) {}


TestWithRandomNumberGenerator::~TestWithRandomNumberGenerator() {}

}  // namespace base


namespace internal {

TestWithIsolate::~TestWithIsolate() {}


Factory* TestWithIsolate::factory() const { return isolate()->factory(); }


TestWithZone::~TestWithZone() {}

}  // namespace internal
}  // namespace v8
