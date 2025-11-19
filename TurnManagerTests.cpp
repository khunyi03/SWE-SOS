#include "CppUnitTest.h"
#include "TurnManager.h"
#include "Player.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SOSCoreNativeTests {

    TEST_CLASS(TurnManagerTests) {
public:

    TEST_METHOD(StartsBlueThenAlternates) {
        TurnManager tm;
        tm.reset();
        Assert::IsTrue(tm.current() == Player::Blue);
        tm.pass();
        Assert::IsTrue(tm.current() == Player::Red);
        tm.pass();
        Assert::IsTrue(tm.current() == Player::Blue);
    }

    TEST_METHOD(ResetReturnsToBlue) {
        TurnManager tm;
        tm.reset();
        tm.pass();
        Assert::IsTrue(tm.current() == Player::Red);
        tm.reset();
        Assert::IsTrue(tm.current() == Player::Blue);
    }
    };

} // namespace
