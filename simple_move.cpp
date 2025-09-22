/* Simple move generator from specific rack and board */
#include <iostream>
#include <string>

#include "datamanager.h"
#include "lexiconparameters.h"
#include "boardparameters.h"
#include "game.h"

using namespace Quackle;
using std::cout;
using std::endl;
using std::string;

static bool ensureLexicon()
{
    // Try to load enable1.15.gaddag first
    std::string gaddag = LexiconParameters::findDictionaryFile("enable1.15.gaddag");
    cout << "Looking for enable1.15.gaddag: " << gaddag << endl;
    if (!gaddag.empty()) {
        QUACKLE_LEXICON_PARAMETERS->loadGaddag(gaddag);
        cout << "GADDAG loaded: " << (QUACKLE_LEXICON_PARAMETERS->hasGaddag() ? "YES" : "NO") << endl;
        if (QUACKLE_LEXICON_PARAMETERS->hasGaddag()) return true;
    }

    // Fall back to enable1.15.dawg
    std::string dawg = LexiconParameters::findDictionaryFile("enable1.15.dawg");
    cout << "Looking for enable1.15.dawg: " << dawg << endl;
    if (dawg.empty()) {
        dawg = LexiconParameters::findDictionaryFile("twl06.dawg");
        cout << "Fallback to twl06.dawg: " << dawg << endl;
    }
    if (dawg.empty()) return false;
    
    QUACKLE_LEXICON_PARAMETERS->loadDawg(dawg);
    cout << "DAWG loaded: " << (QUACKLE_LEXICON_PARAMETERS->hasDawg() ? "YES" : "NO") << endl;
    return QUACKLE_LEXICON_PARAMETERS->hasDawg();
}

void printBoard(const Board &board) {
    cout << "Board state:" << endl;
    for (int r = 0; r < board.height(); ++r) {
        for (int c = 0; c < board.width(); ++c) {
            Letter letter = board.letter(r, c);
            if (letter == QUACKLE_NULL_MARK) {
                cout << ".";
            } else {
                cout << QUACKLE_ALPHABET_PARAMETERS->userVisible(letter);
            }
        }
        cout << endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <rack> [board_letter] [row] [col]" << endl;
        cout << "  rack: letters in rack (e.g., AEIRSTZ)" << endl;
        cout << "  board_letter: optional letter on board (e.g., A)" << endl;
        cout << "  row: optional row for board letter (e.g., 7)" << endl;
        cout << "  col: optional column for board letter (e.g., 7)" << endl;
        cout << "  Example: " << argv[0] << " AEIRSTZ" << endl;
        cout << "  Example: " << argv[0] << " HELLO?? A 7 7" << endl;
        return 1;
    }

    DataManager dm;
    dm.setAppDataDirectory("data");
    dm.setBoardParameters(new EnglishBoard());

    cout << "Alphabet: " << QUACKLE_ALPHABET_PARAMETERS->alphabetName() << endl;
    cout << "Alphabet size: " << QUACKLE_ALPHABET_PARAMETERS->length() << endl;

    if (!ensureLexicon()) {
        std::cerr << "No DAWG lexicon found under data/lexica.\n";
        return 2;
    }

    // Create a simple 2-player game
    Game game;
    PlayerList players;
    players.push_back(Player(MARK_UV("P1"), Player::HumanPlayerType, 110));
    players.push_back(Player(MARK_UV("P2"), Player::HumanPlayerType, 110));
    game.setPlayers(players);
    game.addPosition();

    // Set up board
    game.currentPosition().setEmptyBoard();
    
    // Place a letter on board if provided
    if (argc >= 5) {
        string boardLetter = argv[2];
        int row = std::stoi(argv[3]);
        int col = std::stoi(argv[4]);
        
        cout << "Placing " << boardLetter << " at (" << row << "," << col << ")" << endl;
        Move move = Move::createPlaceMove(row, col, true, QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV(boardLetter)));
        game.currentPosition().underlyingBoardReference().makeMove(move);
    }

    // Set rack
    string rackStr = argv[1];
    cout << "Setting rack to: " << rackStr << endl;
    game.currentPosition().setCurrentPlayerRack(Rack(QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV(rackStr))));
    
    printBoard(game.currentPosition().board());

    // Generate moves
    cout << "\nGenerating moves..." << endl;
    game.currentPosition().kibitz(10);
    const MoveList &moves = game.currentPosition().moves();
    
    cout << "\nFound " << moves.size() << " moves:" << endl;
    for (size_t i = 0; i < moves.size() && i < 10; ++i) {
        const Move &move = moves[i];
        cout << "Move " << (i+1) << ": ";
        if (move.action == Move::Place) {
            cout << "PLACE " << QUACKLE_ALPHABET_PARAMETERS->userVisible(move.tiles()) << " at (" << move.startrow << "," << move.startcol << ") ";
            cout << (move.horizontal ? "horizontal" : "vertical");
            cout << " - Score: " << move.score;
        } else if (move.action == Move::Exchange) {
            cout << "EXCHANGE " << QUACKLE_ALPHABET_PARAMETERS->userVisible(move.tiles()) << " - Score: " << move.score;
        } else if (move.action == Move::Pass) {
            cout << "PASS - Score: " << move.score;
        }
        cout << endl;
    }
    
    if (moves.empty()) {
        cout << "No valid moves found!" << endl;
        return 3;
    }
    
    // Show best move
    const Move &bestMove = moves[0];
    cout << "\n=== BEST MOVE ===" << endl;
    if (bestMove.action == Move::Place) {
        cout << "Place " << QUACKLE_ALPHABET_PARAMETERS->userVisible(bestMove.tiles()) << " at (" << bestMove.startrow << "," << bestMove.startcol << ") ";
        cout << (bestMove.horizontal ? "horizontally" : "vertically");
        cout << " for " << bestMove.score << " points" << endl;
    } else if (bestMove.action == Move::Exchange) {
        cout << "Exchange " << QUACKLE_ALPHABET_PARAMETERS->userVisible(bestMove.tiles()) << " for " << bestMove.score << " points" << endl;
    } else if (bestMove.action == Move::Pass) {
        cout << "Pass for " << bestMove.score << " points" << endl;
    }

    return 0;
}
