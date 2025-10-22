#pragma once
#include <wx/wx.h>
#include "GameLogic.h" // Include our logic

class MyFrame : public wxFrame {
public:
    MyFrame();

private:
    // Event Handlers
    void OnNewGame(wxCommandEvent& event);
    void OnBoardClick(wxCommandEvent& event);
    void OnClose(wxCloseEvent& event);

    // Helper to create the board
    void CreateBoard();

    // Game Logic
    GameLogic game;

    // --- GUI Controls ---
    wxRadioBox* gameModeRadio;
    wxTextCtrl* boardSizeText;

    wxRadioBox* bluePlayerRadio;
    wxRadioBox* redPlayerRadio;

    wxPanel* boardPanel; // Panel to hold the button grid
    wxGridSizer* boardSizer; // Sizer for the buttons

    wxStaticText* statusText;
    wxButton* newGameButton;

    // Store a 2D array for buttons just to create them
    // We won't store them because they get deleted
};

class MyApp : public wxApp {
public:
    virtual bool OnInit();
};
