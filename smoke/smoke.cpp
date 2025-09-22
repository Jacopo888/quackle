/* Quick smoke test for libquackle */
#include <iostream>
#include <string>

#include "datamanager.h"
#include "lexiconparameters.h"
#include "boardparameters.h"
#include "game.h"

using namespace Quackle;
using std::cout;
using std::endl;

static bool ensureLexicon()
{
    // Prefer enable1.gaddag (to avoid DAWG/GADDAG hash mismatch); fall back to well-known DAWGs
    std::string gaddag = LexiconParameters::findDictionaryFile("enable1.gaddag");
    cout << "Looking for enable1.gaddag: " << gaddag << endl;
    if (!gaddag.empty()) {
        QUACKLE_LEXICON_PARAMETERS->loadGaddag(gaddag);
        cout << "GADDAG loaded: " << (QUACKLE_LEXICON_PARAMETERS->hasGaddag() ? "YES" : "NO") << endl;
        if (QUACKLE_LEXICON_PARAMETERS->hasGaddag()) return true;
    }

    // Fall back to DAWGs
    std::string dawg = LexiconParameters::findDictionaryFile("enable1.dawg");
    cout << "Looking for enable1.dawg: " << dawg << endl;
    if (dawg.empty()) dawg = LexiconParameters::findDictionaryFile("twl06.dawg");
    if (dawg.empty()) dawg = LexiconParameters::findDictionaryFile("nwl18.dawg");
    if (dawg.empty()) dawg = LexiconParameters::findDictionaryFile("csw19.dawg");
    if (dawg.empty()) dawg = LexiconParameters::findDictionaryFile("csw15.dawg");
    if (dawg.empty()) return false;
    QUACKLE_LEXICON_PARAMETERS->loadDawg(dawg);
    cout << "DAWG loaded: " << (QUACKLE_LEXICON_PARAMETERS->hasDawg() ? "YES" : "NO") << endl;
    return QUACKLE_LEXICON_PARAMETERS->hasDawg();
}

int main()
{
    DataManager dm;
    dm.setAppDataDirectory("data");
    dm.setBoardParameters(new EnglishBoard());

    cout << "Alphabet: " << QUACKLE_ALPHABET_PARAMETERS->alphabetName() << endl;
    cout << "Alphabet size: " << QUACKLE_ALPHABET_PARAMETERS->length() << endl;

    if (!ensureLexicon()) {
        std::cerr << "No DAWG lexicon found under data/lexica.\n";
        return 2;
    }

    // Create a simple 2-player game and add initial position
    Game game;
    PlayerList players;
    players.push_back(Player(MARK_UV("P1"), Player::HumanPlayerType, 110));
    players.push_back(Player(MARK_UV("P2"), Player::HumanPlayerType, 110));
    game.setPlayers(players);
    game.addPosition();

    // Test 1: Empty board + AEIRSTZ
    game.currentPosition().setEmptyBoard();
    game.currentPosition().setCurrentPlayerRack(Rack(QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV("AEIRSTZ"))));
    cout << "Test1 rack: " << game.currentPosition().currentPlayer().rack() << endl;
    cout << "Test1 board empty: " << (game.currentPosition().board().isEmpty() ? "YES" : "NO") << endl;
    
    // Test DAWG reading
    cout << "Testing DAWG reading:" << endl;
    unsigned int p;
    Letter letter;
    bool t, lastchild, british;
    int playability;
    QUACKLE_LEXICON_PARAMETERS->dawgAt(1, p, letter, t, lastchild, british, playability);
    cout << "  DAWG root: p=" << p << " letter=" << (int)letter << " t=" << t << " lastchild=" << lastchild << endl;
    
    game.currentPosition().kibitz(10);
    const MoveList &moves1 = game.currentPosition().moves();
    bool hasPlace1 = false;
    for (const auto &m : moves1) {
        if (m.action == Move::Place) { hasPlace1 = true; break; }
    }
    cout << "Test1 moves: " << moves1 << endl;
    cout << "Test1 result: " << (hasPlace1 ? "OK" : "FAIL") << endl;

    // Test 2: Center A + HELLO??
    game.currentPosition().setEmptyBoard();
    // Place an 'A' at the center
    int r = QUACKLE_BOARD_PARAMETERS->startRow();
    int c = QUACKLE_BOARD_PARAMETERS->startColumn();
    Move a = Move::createPlaceMove(r, c, /*horizontal*/ true, QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV("A")));
    game.currentPosition().underlyingBoardReference().makeMove(a);

    game.currentPosition().setCurrentPlayerRack(Rack(QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV("HELLO??"))));
    game.currentPosition().kibitz(10);
    const MoveList &moves2 = game.currentPosition().moves();
    bool hasPlace2 = false;
    for (const auto &m : moves2) {
        if (m.action == Move::Place) { hasPlace2 = true; break; }
    }
    cout << "Test2 moves: " << moves2 << endl;
    cout << "Test2 result: " << (hasPlace2 ? "OK" : "FAIL") << endl;

    return (hasPlace1 && hasPlace2) ? 0 : 1;
}
