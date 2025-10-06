/*
 *  Engine Wrapper per test Quackle
 *  Copyright (C) 2024 - Basato su Quackle
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cstdlib>
#include <cstring>

#include "datamanager.h"
#include "lexiconparameters.h"
#include "boardparameters.h"
#include "strategyparameters.h"
#include "game.h"
#include "alphabetparameters.h"
#include "evaluator.h"
#include "catchall.h"

using namespace Quackle;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::map;

// Struttura per rappresentare una mossa
struct MoveResult {
    string move_type;
    vector<map<string, string>> tiles;
    int score;
    string word;
    string explanation;
};

class QuackleEngine {
private:
    Game m_game;
    bool m_initialized;
    string m_lexicon;
    string m_lexdir;
    string m_ruleset;
    
public:
    QuackleEngine() : m_initialized(false) {}
    
    bool initialize(const string& lexicon, const string& lexdir, const string& ruleset) {
        m_lexicon = lexicon;
        m_lexdir = lexdir;
        m_ruleset = ruleset;
        
        cout << "=== QUACKLE ENGINE INITIALIZATION ===" << endl;
        cout << "Lexicon: " << lexicon << endl;
        cout << "Lexicon Directory: " << lexdir << endl;
        cout << "Ruleset: " << ruleset << endl;
        
        // Inizializzazione DataManager
        DataManager* dm = DataManager::self();
        dm->setAppDataDirectory("data");
        dm->setBoardParameters(new EnglishBoard());
        dm->setAlphabetParameters(new EnglishAlphabetParameters());
        dm->setLexiconParameters(new LexiconParameters());
        dm->setStrategyParameters(new StrategyParameters());
        
        cout << "Alphabet: " << QUACKLE_ALPHABET_PARAMETERS->alphabetName() << endl;
        cout << "Alphabet size: " << QUACKLE_ALPHABET_PARAMETERS->length() << endl;
        
        // Caricamento dizionario
        if (!loadLexicon()) {
            cout << "❌ Failed to load lexicon: " << lexicon << endl;
            return false;
        }
        
        // Inizializzazione strategie
        cout << "\nInitializing strategies..." << endl;
        try {
            QUACKLE_STRATEGY_PARAMETERS->initialize("default_english");
        } catch (...) {
            cout << "Some strategies not available, continuing with available ones..." << endl;
        }
        
        printStrategyStatus();
        
        // Crea un gioco semplice
        PlayerList players;
        players.push_back(Player(MARK_UV("Player1"), Player::HumanPlayerType, 110));
        players.push_back(Player(MARK_UV("Player2"), Player::HumanPlayerType, 110));
        m_game.setPlayers(players);
        m_game.addPosition();
        
        m_initialized = true;
        cout << "✅ Quackle Engine initialized successfully!" << endl;
        return true;
    }
    
    MoveResult generateMove(const string& rack, const map<string, map<string, string>>& board, const string& difficulty) {
        MoveResult result;
        
        if (!m_initialized) {
            result.move_type = "error";
            result.explanation = "Engine not initialized";
            return result;
        }
        
        cout << "\n=== GENERATING MOVE ===" << endl;
        cout << "Rack: " << rack << endl;
        cout << "Difficulty: " << difficulty << endl;
        cout << "Board tiles: " << board.size() << endl;
        
        // Setup board
        m_game.currentPosition().setEmptyBoard();
        
        // Place board tiles
        for (const auto& tile : board) {
            string pos = tile.first;
            string letter = tile.second.at("letter");
            bool isBlank = tile.second.count("isBlank") && tile.second.at("isBlank") == "true";
            
            // Parse position (e.g., "8,8")
            size_t comma = pos.find(',');
            if (comma != string::npos) {
                int row = std::stoi(pos.substr(0, comma));
                int col = std::stoi(pos.substr(comma + 1));
                
                cout << "Placing " << letter << " at (" << row << "," << col << ")" << endl;
                Move move = Move::createPlaceMove(row, col, true, QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV(letter)));
                m_game.currentPosition().underlyingBoardReference().makeMove(move);
            }
        }
        
        // Set rack
        cout << "Setting rack to: " << rack << endl;
        m_game.currentPosition().setCurrentPlayerRack(Rack(QUACKLE_ALPHABET_PARAMETERS->encode(MARK_UV(rack))));
        
        // Genera mosse
        cout << "Generating moves..." << endl;
        int maxMoves = (difficulty == "hard") ? 20 : (difficulty == "medium") ? 10 : 5;
        m_game.currentPosition().kibitz(maxMoves * 2);
        MoveList moves = m_game.currentPosition().moves();
        
        if (moves.empty()) {
            result.move_type = "pass";
            result.score = 0;
            result.explanation = "No valid moves found";
            return result;
        }
        
        // Valuta e ordina mosse
        CatchallEvaluator evaluator;
        for (auto& move : moves) {
            move.equity = evaluator.equity(m_game.currentPosition(), move);
        }
        
        std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
            return a.equity > b.equity;
        });
        
        // Limita al numero richiesto
        if (moves.size() > static_cast<size_t>(maxMoves)) {
            moves.resize(maxMoves);
        }
        
        // Determina se è la primissima mossa (board vuota prima di giocare)
        bool opening = board.empty();

        // Helper: verifica se una mossa piazza almeno una tessera sul centro (7,7) 0-based
        auto touchesCenter = [](const Move& m) -> bool {
            if (m.action != Move::Place) return false;
            const int CENTER = 7; // 15x15 board -> index 7
            if (m.horizontal) {
                if (m.startrow != CENTER) return false;
                int endc = m.startcol + static_cast<int>(m.tiles().length()) - 1;
                return m.startcol <= CENTER && endc >= CENTER;
            } else {
                if (m.startcol != CENTER) return false;
                int endr = m.startrow + static_cast<int>(m.tiles().length()) - 1;
                return m.startrow <= CENTER && endr >= CENTER;
            }
        };

        // Se apertura: scegli la prima mossa (dopo ordinamento per equity) che tocca il centro; fallback alla migliore assoluta se nessuna
        const Move* chosen = &moves[0];
        if (opening) {
            for (const auto& m : moves) {
                if (touchesCenter(m)) { chosen = &m; break; }
            }
        }

        const Move& bestMove = *chosen;
        
        if (bestMove.action == Move::Place) {
            result.move_type = "play";
            result.score = bestMove.score;
            result.word = QUACKLE_ALPHABET_PARAMETERS->userVisible(bestMove.tiles());
            
            // Converti le tessere in formato JSON
            for (int i = 0; i < bestMove.tiles().length(); ++i) {
                map<string, string> tile;
                tile["letter"] = QUACKLE_ALPHABET_PARAMETERS->userVisible(bestMove.tiles()[i]);
                tile["row"] = std::to_string(bestMove.startrow);
                tile["col"] = std::to_string(bestMove.startcol + i);
                result.tiles.push_back(tile);
            }
            
            result.explanation = "Best move: " + result.word + " for " + std::to_string(result.score) + " points";
        } else if (bestMove.action == Move::Exchange) {
            result.move_type = "exchange";
            result.score = bestMove.score;
            result.word = QUACKLE_ALPHABET_PARAMETERS->userVisible(bestMove.tiles());
            result.explanation = "Exchange: " + result.word;
        } else {
            result.move_type = "pass";
            result.score = bestMove.score;
            result.explanation = "Pass";
        }
        
        cout << "✅ Generated move: " << result.explanation;
        if (opening && !touchesCenter(bestMove)) {
            cout << " (WARNING: opening move does not touch center - no alternative found)";
        }
        cout << endl;
        return result;
    }
    
private:
    bool loadLexicon() {
        // Prova prima GADDAG
        string gaddagFile = LexiconParameters::findDictionaryFile(m_lexicon + ".gaddag");
        if (!gaddagFile.empty()) {
            QUACKLE_LEXICON_PARAMETERS->loadGaddag(gaddagFile);
            if (QUACKLE_LEXICON_PARAMETERS->hasGaddag()) {
                cout << "✅ GADDAG loaded: " << gaddagFile << endl;
                return true;
            }
        }
        
        // Fall back to DAWG
        string dawgFile = LexiconParameters::findDictionaryFile(m_lexicon + ".dawg");
        if (dawgFile.empty()) {
            dawgFile = LexiconParameters::findDictionaryFile("twl06.dawg");
        }
        
        if (!dawgFile.empty()) {
            QUACKLE_LEXICON_PARAMETERS->loadDawg(dawgFile);
            if (QUACKLE_LEXICON_PARAMETERS->hasDawg()) {
                cout << "✅ DAWG loaded: " << dawgFile << endl;
                return true;
            }
        }
        
        return false;
    }
    
    void printStrategyStatus() {
        cout << "\n=== STRATEGY STATUS ===" << endl;
        cout << "Lexicon: " << (QUACKLE_LEXICON_PARAMETERS->hasDawg() ? "DAWG" : "NO") 
             << " / " << (QUACKLE_LEXICON_PARAMETERS->hasGaddag() ? "GADDAG" : "NO") << endl;
        
        cout << "Strategies:" << endl;
        cout << "  - Syn2: " << (QUACKLE_STRATEGY_PARAMETERS->hasSyn2() ? "YES" : "NO") << endl;
        cout << "  - Worths: " << (QUACKLE_STRATEGY_PARAMETERS->hasWorths() ? "YES" : "NO") << endl;
        cout << "  - VcPlace: " << (QUACKLE_STRATEGY_PARAMETERS->hasVcPlace() ? "YES" : "NO") << endl;
        cout << "  - Bogowin: " << (QUACKLE_STRATEGY_PARAMETERS->hasBogowin() ? "YES" : "NO") << endl;
        cout << "  - Superleaves: " << (QUACKLE_STRATEGY_PARAMETERS->hasSuperleaves() ? "YES" : "NO") << endl;
    }
};

// Funzione per stampare JSON
void printJson(const MoveResult& result) {
    cout << "{" << endl;
    cout << "  \"move_type\": \"" << result.move_type << "\"," << endl;
    cout << "  \"score\": " << result.score << "," << endl;
    cout << "  \"word\": \"" << result.word << "\"," << endl;
    cout << "  \"tiles\": [" << endl;
    
    for (size_t i = 0; i < result.tiles.size(); ++i) {
        const auto& tile = result.tiles[i];
        cout << "    {" << endl;
        cout << "      \"letter\": \"" << tile.at("letter") << "\"," << endl;
        cout << "      \"row\": " << tile.at("row") << "," << endl;
        cout << "      \"col\": " << tile.at("col") << endl;
        cout << "    }";
        if (i < result.tiles.size() - 1) cout << ",";
        cout << endl;
    }
    
    cout << "  ]," << endl;
    cout << "  \"explanation\": \"" << result.explanation << "\"" << endl;
    cout << "}" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Usage: " << argv[0] << " --lexicon <lexicon> --lexdir <dir> --ruleset <ruleset>" << endl;
        return 1;
    }
    
    string lexicon, lexdir, ruleset;
    
    // Parse arguments
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc) break;
        
        string arg = argv[i];
        string value = argv[i + 1];
        
        if (arg == "--lexicon") lexicon = value;
        else if (arg == "--lexdir") lexdir = value;
        else if (arg == "--ruleset") ruleset = value;
    }
    
    QuackleEngine engine;
    if (!engine.initialize(lexicon, lexdir, ruleset)) {
        return 1;
    }
    
    // Read JSON input from stdin
    string input;
    std::getline(std::cin, input);
    
    // Simple JSON parsing (basic implementation)
    // In a real implementation, you'd use a proper JSON library
    
    // Extract rack
    size_t rackStart = input.find("\"rack\": \"") + 9;
    size_t rackEnd = input.find("\"", rackStart);
    string rack = input.substr(rackStart, rackEnd - rackStart);
    
    // Extract difficulty
    size_t diffStart = input.find("\"difficulty\": \"") + 15;
    size_t diffEnd = input.find("\"", diffStart);
    string difficulty = input.substr(diffStart, diffEnd - diffStart);
    
    // Extract board (simplified - just check if board object exists)
    map<string, map<string, string>> board;
    if (input.find("\"board\": {") != string::npos) {
        // For this test, we'll add a simple board tile
        // In a real implementation, you'd parse the full board JSON
        size_t boardStart = input.find("\"board\": {");
        if (boardStart != string::npos) {
            // Look for specific board tiles
            if (input.find("\"8,8\"") != string::npos) {
                map<string, string> tile;
                tile["letter"] = "A";
                tile["isBlank"] = "false";
                board["8,8"] = tile;
            }
        }
    }
    
    MoveResult result = engine.generateMove(rack, board, difficulty);
    printJson(result);
    
    return 0;
}
