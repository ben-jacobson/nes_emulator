#include "catch.hpp"
#include "test_fixtures.h"
    
TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - CLI", "[cpu instruction]") {
    test_cpu.instr_CLI(); // clear IRQ disable bit
    REQUIRE(test_cpu.get_status_flags_struct().i == 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - SEI", "[cpu instruction]") {
    test_cpu.instr_SEI(); // set IRQ disable bit
    REQUIRE(test_cpu.get_status_flags_struct().i == 1);
}
