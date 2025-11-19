#include "CppUnitTest.h"

#include "ComputerPlayer.h"
#include "Board.h"
#include "Cell.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SOSCoreNativeTests {

    TEST_CLASS(ComputerPlayerTests) {
public:

    // Test 1: On an empty board, the computer should always pick a valid move.
    TEST_METHOD(ComputerChoosesValidMove_OnEmptyBoard) {
        Board b(3);
        ComputerPlayer ai;
        auto mv = ai.chooseMove(b);
        Assert::IsTrue(mv.has_value(), L"Computer should pick a move.");
        auto [r, c, L] = *mv;
        Assert::IsTrue(r >= 0 && r < b.size(), L"Row must be within bounds.");
        Assert::IsTrue(c >= 0 && c < b.size(), L"Col must be within bounds.");
        Assert::IsTrue(L == Cell::S || L == Cell::O, L"Letter must be S or O.");
    }

    // Test 2: The computer should block when an SOS is about to happen.
    TEST_METHOD(ComputerBlocksMiddle_OAgainstS_S) {
        // S _ S across top row → best block is O at (0,1)
        Board b(3);
        b.place(0, 0, Cell::S);
        b.place(0, 2, Cell::S);

        ComputerPlayer ai;
        auto mv = ai.chooseMove(b);
        Assert::IsTrue(mv.has_value(), L"Computer should find a blocking move.");
        auto [r, c, L] = *mv;

        Assert::AreEqual(0, r, L"Row should be top row.");
        Assert::AreEqual(1, c, L"Col should be middle column.");
        Assert::IsTrue(L == Cell::O, L"Should place O to block SOS.");
    }

    // Test 3: The computer should make an immediate scoring move when possible.
    TEST_METHOD(ComputerPrefersImmediateScore_WhenAvailable) {
        // Set up row 1: S _ S → placing O in the middle scores immediately
        Board b(3);
        b.place(1, 0, Cell::S);
        b.place(1, 2, Cell::S);

        ComputerPlayer ai;
        auto mv = ai.chooseMove(b);
        Assert::IsTrue(mv.has_value(), L"Computer should find a scoring move.");
        auto [r, c, L] = *mv;

        Assert::AreEqual(1, r, L"Row should be the middle row.");
        Assert::AreEqual(1, c, L"Column should be the middle column.");
        Assert::IsTrue(L == Cell::O, L"Should place O to make SOS.");
    }
    };

}
