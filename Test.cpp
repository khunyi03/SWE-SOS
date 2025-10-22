#include "pch.h"
#include "CppUnitTest.h"
#include "../wxSOS/GameLogic.h" // Path to your logic header

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SosGameTests
{
    TEST_CLASS(SosGameTests)
    {
    public:

        TEST_METHOD(TestNewGame)
        {
            GameLogic game;
            game.newGame(8, 0); // 8x8, Simple

            Assert::AreEqual(8, game.boardSize);
            Assert::AreEqual(0, game.gameMode);
            Assert::AreEqual(0, game.getTurn()); // Blue's turn
            Assert::AreEqual(0, game.getCell(5, 5)); // Cell is empty
        }

        TEST_METHOD(TestMakeMove)
        {
            GameLogic game;
            game.newGame(8, 0);

            // Blue makes a move
            bool success = game.makeMove(1, 1, 1); // (1,1), 'S'

            Assert::IsTrue(success);
            Assert::AreEqual(1, game.getCell(1, 1)); // Cell has 'S'
            Assert::AreEqual(1, game.getTurn()); // Now Red's turn
        }

        TEST_METHOD(TestInvalidMove)
        {
            GameLogic game;
            game.newGame(8, 0);
            game.makeMove(1, 1, 1); // Blue moves

            // Red tries to move on same spot
            bool success = game.makeMove(1, 1, 2);
            Assert::IsFalse(success);
            Assert::AreEqual(1, game.getCell(1, 1)); // Cell is still 'S'
            Assert::AreEqual(1, game.getTurn()); // Still Red's turn
        }

    };
}
