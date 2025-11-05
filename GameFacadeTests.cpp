#include "CppUnitTest.h"
#include "GameFacade.h"
#include "GameConfig.h"
#include "Board.h"
#include "Cell.h"
#include "Player.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace SOSCoreNativeTests {

	static GameConfig Simple3() { GameConfig c; c.size = 3; c.mode = GameMode::Simple;  return c; }
	static GameConfig General3() { GameConfig c; c.size = 3; c.mode = GameMode::General; return c; }

	TEST_CLASS(GameFacadeTests) {
public:

	TEST_METHOD(PlaceLetterWritesBoardAndFlipTurn_SimpleNoScore) {
		GameFacade g; g.newGame(Simple3());
		auto m1 = g.placeLetter(0, 0, Cell::S);
		Assert::IsTrue(m1.placed);
		Assert::AreEqual(0, m1.newSequences);
		Assert::IsTrue(g.board().at(0, 0) == Cell::S);
		Assert::IsTrue(g.currentPlayer() == Player::Red);
	}

	TEST_METHOD(Simple_FirstSOSWins) {
		GameFacade g; g.newGame(Simple3());
		Assert::IsTrue(g.placeLetter(1, 0, Cell::S).placed); // Blue
		Assert::IsTrue(g.placeLetter(0, 0, Cell::O).placed); // Red
		Assert::IsTrue(g.placeLetter(1, 1, Cell::O).placed); // Blue
		Assert::IsTrue(g.placeLetter(0, 1, Cell::S).placed); // Red
		auto win = g.placeLetter(1, 2, Cell::S);             // Blue
		Assert::IsTrue(win.placed);
		Assert::IsTrue(win.newSequences >= 1);
		Assert::IsTrue(g.result() == GameResult::BlueWins);
		Assert::AreEqual(1, g.scoreBlue());
	}

	TEST_METHOD(Simple_DrawWhenBoardFullNoSOS) {
		GameFacade g; g.newGame(Simple3());
		for (int r = 0; r < 3; ++r)
			for (int c = 0; c < 3; ++c)
				Assert::IsTrue(g.placeLetter(r, c, Cell::O).placed);
		Assert::IsTrue(g.result() == GameResult::Draw);
	}

	TEST_METHOD(General_ScoringAndExtraTurn) {
		GameFacade g; g.newGame(General3());
		// Set vertical S_S and score 1 with center O
		Assert::IsTrue(g.placeLetter(0, 1, Cell::S).placed); // B
		Assert::IsTrue(g.placeLetter(0, 0, Cell::S).placed); // R filler
		Assert::IsTrue(g.placeLetter(2, 1, Cell::S).placed); // B
		Assert::IsTrue(g.placeLetter(2, 2, Cell::S).placed); // R filler

		auto out = g.placeLetter(1, 1, Cell::O);             // Blue scores 1
		Assert::IsTrue(out.placed);
		Assert::IsTrue(out.newSequences >= 1);
		Assert::IsTrue(out.extraTurn);
		Assert::AreEqual(out.newSequences, g.scoreBlue());   // +1 per sequence
		Assert::IsTrue(g.currentPlayer() == Player::Blue);
		Assert::IsTrue(g.result() == GameResult::Ongoing);
	}

	// Deterministic: exactly 2 sequences (horizontal + vertical). Avoid diagonals by using O in corners.
	TEST_METHOD(General_MultipleSOSInOneMoveScoresAll) {
		GameFacade g; g.newGame(General3());
		// Blue builds S at top/bottom/left/right of center.
		Assert::IsTrue(g.placeLetter(0, 1, Cell::S).placed); // B: top
		Assert::IsTrue(g.placeLetter(0, 0, Cell::O).placed); // R: corner O (blocks TL↘BR diag)
		Assert::IsTrue(g.placeLetter(2, 1, Cell::S).placed); // B: bottom
		Assert::IsTrue(g.placeLetter(2, 2, Cell::O).placed); // R: corner O (blocks TR↙BL diag)
		Assert::IsTrue(g.placeLetter(1, 0, Cell::S).placed); // B: left
		Assert::IsTrue(g.placeLetter(0, 2, Cell::O).placed); // R: corner O
		Assert::IsTrue(g.placeLetter(1, 2, Cell::S).placed); // B: right
		Assert::IsTrue(g.placeLetter(2, 0, Cell::O).placed); // R: corner O

		// Blue places center 'O' — creates horizontal + vertical SOS (>=2 sequences).
		auto out = g.placeLetter(1, 1, Cell::O);
		Assert::IsTrue(out.placed);
		Assert::IsTrue(out.newSequences >= 2);

		// +1 per SOS: score must match sequences from this move
		Assert::AreEqual(out.newSequences, g.scoreBlue());

		// If the board is now full, result should be BlueWins; otherwise Blue gets extra turn.
		const auto& b = g.board();
		bool boardFull = true;
		for (int r = 0; r < b.size(); ++r)
			for (int c = 0; c < b.size(); ++c)
				if (b.at(r, c) == Cell::Empty) { boardFull = false; break; }

		if (boardFull) {
			Assert::IsTrue(g.result() == GameResult::BlueWins);
		}
		else {
			Assert::IsTrue(out.extraTurn);
			Assert::IsTrue(g.currentPlayer() == Player::Blue);
			Assert::IsTrue(g.result() == GameResult::Ongoing);
		}
	}


	};

} // namespace
