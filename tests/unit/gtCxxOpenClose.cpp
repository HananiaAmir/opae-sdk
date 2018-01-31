#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#include "gtest/gtest.h"
#include <opae/cxx/core/token.h>
#include <opae/cxx/core/handle.h>
#include <opae/cxx/core/properties.h>
#include <opae/cxx/accelerator.h>

using namespace opae::fpga::types;
using namespace opae::fpga::resource;

/**
 * @test handle_open_01
 * Given an environment with at least one accelerator<br>
 * When I call token::enumerate with a filter of only FPGA_ACCELERATOR<br>
 * And I call handle::open with a token<br>
 * Then I get a non-null handle<br>
 * And no exceptions are thrown when I release the handle and tokens<br>
 */
TEST(CxxOpen, handle_open_01) {
  auto tokens = token::enumerate({FPGA_ACCELERATOR});
  ASSERT_TRUE(tokens.size() > 0);
  handle::ptr_t h = handle::open(tokens[0], FPGA_OPEN_SHARED);
  ASSERT_NE(nullptr, h.get());
  h.reset();
  ASSERT_NO_THROW(tokens.clear());
}

/**
 * @test accelerator_open_01
 * Given an environment with at least one accelerator<br>
 * When I call accelerator::enumerate with a filter of only FPGA_ACCELERATOR<br>
 * And I call accelerator::open using an accelerator instance<br>
 * And I release the accelerator objects<br>
 * Then no exceptions are thrown<br>
 */
TEST(CxxOpen, accelerator_open_01) {
  auto accelerators = accelerator::enumerate({});
  ASSERT_TRUE(accelerators.size() > 0);
  ASSERT_NO_THROW(accelerators[0]->open(FPGA_OPEN_SHARED));
  ASSERT_NO_THROW(accelerators.clear());
}
