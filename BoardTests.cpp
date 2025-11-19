#include "CppUnitTest.h"
#include "Board.h"
#include "Cell.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SOSCoreNativeTests {

    TEST_CLASS(BoardTests) {
public:

    TEST_METHOD(StartsEmpty3x3ByDefault) {
        Board b(3);
        Assert::AreEqual(3, b.size());
        for (int r = 0; r < b.size(); ++r) {
            for (int c = 0; c < b.size(); ++c) {
                Assert::IsTrue(b.at(r, c) == Cell::Empty);
            }
        }
    }

    TEST_METHOD(PlaceSAndO) {
        Board b(3);
        Assert::IsTrue(b.place(1, 1, Cell::S));
        Assert::IsTrue(b.at(1, 1) == Cell::S);

        Assert::IsTrue(b.place(0, 2, Cell::O));
        Assert::IsTrue(b.at(0, 2) == Cell::O);
    }

    TEST_METHOD(RejectsInvalidOrOccupied) {
        Board b(3);
        // out-of-bounds
        Assert::IsFalse(b.place(-1, 0, Cell::S));
        Assert::IsFalse(b.place(0, -1, Cell::O));
        Assert::IsFalse(b.place(3, 0, Cell::S));
        Assert::IsFalse(b.place(0, 3, Cell::O));

        // occupied
        Assert::IsTrue(b.place(1, 1, Cell::S));
        Assert::IsFalse(b.place(1, 1, Cell::O));
        Assert::IsTrue(b.at(1, 1) == Cell::S);
    }

    TEST_METHOD(ResetClampsAndClears) {
        Board b(5);
        Assert::IsTrue(b.place(2, 2, Cell::O));
        b.reset(3); // canonical size for reset in our app flow
        Assert::AreEqual(3, b.size());
        for (int r = 0; r < b.size(); ++r)
            for (int c = 0; c < b.size(); ++c)
                Assert::IsTrue(b.at(r, c) == Cell::Empty);
    }

    TEST_METHOD(OOBReadReturnsEmptySafely) {
        Board b(3);
        Assert::IsTrue(b.at(-1, 0) == Cell::Empty);
        Assert::IsTrue(b.at(0, -1) == Cell::Empty);
        Assert::IsTrue(b.at(99, 99) == Cell::Empty);
    }
    };

} // namespace

