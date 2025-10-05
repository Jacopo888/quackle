/*
 * Test program per verificare il sistema di coordinate di Quackle
 * Compilare con: g++ -I. coordinate_test.cpp -Llib/release -lquackle -o coordinate_test
 */

#include <iostream>
#include <string>
#include "datamanager.h"
#include "move.h"
#include "boardparameters.h"

using namespace Quackle;
using std::cout;
using std::endl;

void testCoordinateSystem() {
    cout << "=== QUACKLE COORDINATE SYSTEM TEST ===" << endl;
    
    // Test 1: Centro board (H8 in notazione Scrabble)
    cout << "\n1. CENTRO BOARD (H8):" << endl;
    
    // Test A1/H8 notation
    Move move1 = Move::createPlaceMove(MARK_UV("8H"), QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV("A")));
    cout << "   A1/H8 notation '8H': row=" << move1.startrow << ", col=" << move1.startcol 
         << ", horizontal=" << (move1.horizontal ? "true" : "false") << endl;
    
    // Test H8 notation  
    Move move2 = Move::createPlaceMove(MARK_UV("H8"), QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV("A")));
    cout << "   H8 notation 'H8': row=" << move2.startrow << ", col=" << move2.startcol 
         << ", horizontal=" << (move2.horizontal ? "true" : "false") << endl;
    
    // Test direct 0-based coordinates
    Move move3 = Move::createPlaceMove(7, 7, true, QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV("A")));
    cout << "   Direct (7,7): row=" << move3.startrow << ", col=" << move3.startcol 
         << ", horizontal=" << (move3.horizontal ? "true" : "false") << endl;
    
    // Test 2: Board parameters
    cout << "\n2. BOARD PARAMETERS:" << endl;
    cout << "   Board size: " << QUACKLE_BOARD_PARAMETERS->width() << "x" << QUACKLE_BOARD_PARAMETERS->height() << endl;
    cout << "   Start position: row=" << QUACKLE_BOARD_PARAMETERS->startRow() 
         << ", col=" << QUACKLE_BOARD_PARAMETERS->startColumn() << endl;
    
    // Test 3: Various coordinate formats
    cout << "\n3. COORDINATE FORMATS:" << endl;
    
    // Test different A1/H8 formats
    const char* testCoords[] = {"A1", "H8", "8H", "O15", "15O", "1A", "A15", "O1"};
    for (int i = 0; i < 8; i++) {
        Move testMove = Move::createPlaceMove(MARK_UV(testCoords[i]), QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV("X")));
        cout << "   '" << testCoords[i] << "': row=" << testMove.startrow 
             << ", col=" << testMove.startcol 
             << ", horizontal=" << (testMove.horizontal ? "true" : "false") << endl;
    }
    
    // Test 4: Direction detection
    cout << "\n4. DIRECTION DETECTION:" << endl;
    cout << "   '8H' (number first): horizontal=" << (move1.horizontal ? "true" : "false") << endl;
    cout << "   'H8' (letter first): horizontal=" << (move2.horizontal ? "true" : "false") << endl;
    
    // Test 5: Position string conversion
    cout << "\n5. POSITION STRING CONVERSION:" << endl;
    cout << "   Move (7,7,true) -> positionString: '" << move3.positionString() << "'" << endl;
    cout << "   Move (7,7,false) -> positionString: '" << Move::createPlaceMove(7, 7, false, QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV("A"))).positionString() << "'" << endl;
}

int main() {
    // Initialize Quackle
    DataManager dm;
    dm.setBoardParameters(new EnglishBoard());
    
    testCoordinateSystem();
    
    return 0;
}

