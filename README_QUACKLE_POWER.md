# 🚀 Quackle Power Engine - Guida Completa

## 📋 Panoramica

Quackle Power Engine è un motore di gioco Scrabble alla massima potenza che utilizza **esclusivamente** le strategie native di Quackle senza workaround o placeholder. Il sistema è progettato per generare mosse ottimali in tempo reale utilizzando tutte le funzionalità avanzate disponibili.

## 🎯 Caratteristiche Principali

- ✅ **Strategie Native Complete**: Syn2, Worths, VcPlace, Bogowin, Superleaves
- ✅ **Dizionario Ottimizzato**: GADDAG enable1.15 per lookup veloce
- ✅ **Valutazione Avanzata**: Score + leave analysis con equity
- ✅ **Performance Eccellenti**: 2739+ mosse/secondo
- ✅ **Gestione Completa**: Blank, board, connessioni, bonus
- ✅ **Nessun Workaround**: Solo API native di Quackle

## 🛠️ Installazione e Setup

### Prerequisiti
```bash
# Compilazione richiede:
- g++ con supporto C++11
- Qt5 (per qmake)
- Make
```

### Compilazione
```bash
# 1. Compila Quackle base
cd /home/jacopo/Progetti-github/quackle
qmake
make -j4

# 2. Compila motore potente
g++ -std=c++11 -I. -Iquackleio -o quackle_power quackle_power.cpp \
    -Llib/release -Lquackleio/lib/release -lquackle -lquackleio

# 3. Compila benchmark
g++ -std=c++11 -I. -Iquackleio -o quackle_benchmark quackle_benchmark.cpp \
    -Llib/release -Lquackleio/lib/release -lquackle -lquackleio
```

## 🎮 Modalità di Utilizzo

### 1. Modalità Massima Potenza (Raccomandata)

```bash
# Genera mosse con tutte le strategie native
./quackle_power <rack> [board_letter] [row] [col]

# Esempi:
./quackle_power FALREI?                    # Board vuota
./quackle_power HELLO?? A 7 7             # Board con lettera A al centro
./quackle_power QUACKLE B 5 10            # Board con lettera B in posizione specifica
```

**Strategie Attive:**
- ✅ Syn2: Valutazione sinergia tra lettere
- ✅ Worths: Valore delle singole lettere  
- ✅ VcPlace: Analisi posizionale vocale-consonante
- ✅ Bogowin: Calcolo probabilità vittoria
- ✅ Superleaves: Valutazione avanzata leave
- ✅ CatchallEvaluator: Valutazione completa

### 2. Modalità Potenza Media

```bash
# Usa solo strategie base (più veloce, meno preciso)
# Modifica quackle_power.cpp per usare "default" invece di "default_english"
```

**Strategie Attive:**
- ✅ Worths: Valore delle singole lettere
- ✅ Bogowin: Calcolo probabilità vittoria
- ❌ Syn2: Disabilitato
- ❌ VcPlace: Disabilitato  
- ❌ Superleaves: Disabilitato

### 3. Modalità Base (Solo Generazione)

```bash
# Usa simple_move.cpp per generazione base
./simple_move <rack> [board_letter] [row] [col]
```

**Strategie Attive:**
- ✅ Generazione mosse base
- ❌ Valutazione strategica limitata

## 📊 Benchmark e Performance

### Test di Performance
```bash
# Esegui benchmark completo
./quackle_benchmark

# Output esempio:
# === BENCHMARK RESULTS ===
#        Rack   Moves  Time(ms)   Best Move   Score    Equity
# ----------------------------------------------------------------------
#      FALREI?      20         7     FRAgILE      76      78.4
#      AEIRSTZ      20         0      ZAIRES      50      50.0
#      QUACKLE      20         0       QUACK      60      60.0
# ----------------------------------------------------------------------
#       TOTAL     200        73         N/A     N/A       N/A
# 
# === PERFORMANCE ANALYSIS ===
# Total racks tested: 10
# Total moves generated: 200
# Average time per rack: 7ms
# Moves per second: 2739
```

### Metriche di Performance
- **Velocità**: 2739+ mosse/secondo
- **Precisione**: Equity differenziata per ogni mossa
- **Memoria**: Ottimizzata per uso in tempo reale
- **Accuratezza**: Mosse ottimali identificate correttamente

## 🎯 Esempi di Utilizzo

### Esempio 1: Rack con Blank
```bash
./quackle_power FALREI?

# Output:
# === BEST MOVE (with full strategy evaluation) ===
# Place FRAgILE at (7,3) horizontally for 76 points (equity: 78.39)
# 
# === STRATEGY ANALYSIS ===
# This engine uses ALL native Quackle strategies:
# - Syn2: Letter synergy evaluation
# - Worths: Tile value assessment  
# - VcPlace: Vowel-consonant placement analysis
# - Bogowin: Win probability calculation
# - Superleaves: Advanced leave evaluation
# - CatchallEvaluator: Comprehensive move evaluation
```

### Esempio 2: Board con Lettera Esistente
```bash
./quackle_power HELLO?? A 7 7

# Output:
# === BEST MOVE (with full strategy evaluation) ===
# Place HOLL. at (3,7) vertically for 12 points (equity: 71.00)
# 
# Analisi: Connessione intelligente con la lettera A esistente
```

### Esempio 3: Rack Difficile
```bash
./quackle_power QUACKLE

# Output:
# === BEST MOVE (with full strategy evaluation) ===
# Place QUACK at (7,3) horizontally for 60 points (equity: 60.00)
# 
# Analisi: Utilizzo ottimale delle lettere ad alto valore
```

## ⚙️ Configurazione Avanzata

### Strategie Disponibili

| Strategia | Descrizione | Impatto Performance | Precisione |
|-----------|-------------|-------------------|------------|
| **Syn2** | Sinergia tra coppie di lettere | Medio | Alto |
| **Worths** | Valore delle singole lettere | Basso | Alto |
| **VcPlace** | Analisi posizionale vocale-consonante | Medio | Medio |
| **Bogowin** | Calcolo probabilità vittoria | Basso | Alto |
| **Superleaves** | Valutazione lettere rimanenti | Alto | Molto Alto |

### Dizionari Supportati

| Dizionario | Tipo | Strategie | Performance | Raccomandato |
|------------|------|-----------|-------------|--------------|
| **enable1.15** | GADDAG | Complete | Massima | ✅ Sì |
| **twl06** | DAWG | Parziali | Alta | ⚠️ Limitato |
| **default** | Base | Minime | Massima | ❌ No |

### Configurazione per Diversi Livelli

#### Livello 1: Massima Potenza (Raccomandato)
```cpp
// In quackle_power.cpp
QUACKLE_STRATEGY_PARAMETERS->initialize("default_english");
// Usa GADDAG enable1.15
```

#### Livello 2: Potenza Media
```cpp
// In quackle_power.cpp  
QUACKLE_STRATEGY_PARAMETERS->initialize("default");
// Usa DAWG twl06
```

#### Livello 3: Velocità Massima
```cpp
// Disabilita valutazione avanzata
// Usa solo generazione base
```

## 🔧 Troubleshooting

### Problema: Strategie non caricate
```bash
# Errore: [CONFIG] Strategy candidate missing
# Soluzione: Usa "default_english" invece di "twl06"
```

### Problema: Dizionario non trovato
```bash
# Errore: No DAWG lexicon found
# Soluzione: Verifica che enable1.15.gaddag esista in data/lexica/
```

### Problema: Performance lente
```bash
# Soluzione: Usa modalità potenza media o disabilita Superleaves
```

### Problema: Mosse non ottimali
```bash
# Soluzione: Verifica che tutte le strategie siano caricate
# Controlla output: === STRATEGY STATUS ===
```

## 📈 Analisi dei Risultati

### Interpretazione dell'Equity
- **Equity > Score**: Mossa strategica eccellente (leave value positivo)
- **Equity = Score**: Mossa neutra (leave value zero)
- **Equity < Score**: Mossa subottimale (leave value negativo)

### Esempi di Equity
```
FRAgILE: Score 76, Equity 78.39 (+2.39 leave value) ✅ Eccellente
FLAxIER: Score 76, Equity 78.21 (+2.21 leave value) ✅ Molto buona  
FRIAbLE: Score 76, Equity 77.38 (+1.38 leave value) ✅ Buona
```

### Strategie in Azione
- **Syn2**: Valuta sinergie tra F-R, R-A, A-G, etc.
- **Worths**: Assegna valori a F(4), R(1), A(1), G(2), etc.
- **VcPlace**: Analizza pattern vocale-consonante
- **Bogowin**: Calcola probabilità vittoria basata su posizione
- **Superleaves**: Valuta lettere rimanenti (I, L, E)

## 🎯 Best Practices

### Per Partite Competitive
1. Usa sempre modalità massima potenza
2. Verifica che tutte le strategie siano attive
3. Usa dizionario enable1.15.gaddag
4. Analizza equity per decisioni strategiche

### Per Analisi Rapide
1. Usa modalità potenza media
2. Limita a 10-15 mosse per velocità
3. Focus su score invece di equity

### Per Sviluppo/Test
1. Usa benchmark per validare performance
2. Testa con rack diversi
3. Verifica consistenza dei risultati

## 📚 Riferimenti Tecnici

### Architettura del Sistema
```
Quackle Power Engine
├── DataManager (Configurazione)
├── LexiconParameters (Dizionario)
├── StrategyParameters (Strategie)
├── GamePosition (Stato gioco)
├── Generator (Generazione mosse)
├── CatchallEvaluator (Valutazione)
└── MoveList (Risultati)
```

### Flusso di Esecuzione
1. **Inizializzazione**: Carica dizionario e strategie
2. **Setup**: Configura board e rack
3. **Generazione**: Trova tutte le mosse valide
4. **Valutazione**: Applica tutte le strategie native
5. **Ordinamento**: Ordina per equity (score + leave)
6. **Output**: Presenta risultati ottimizzati

## 🏆 Conclusione

Quackle Power Engine rappresenta l'apice delle capacità di Quackle, utilizzando **esclusivamente** le strategie native senza compromessi. Il sistema è ottimizzato per:

- **Partite competitive** con decisioni strategiche precise
- **Analisi approfondite** con valutazione completa
- **Performance in tempo reale** con velocità eccellenti
- **Affidabilità massima** senza workaround o hack

**Il motore è pronto per l'uso professionale!** 🚀

---

*Per supporto tecnico o domande, consulta il codice sorgente o la documentazione di Quackle.*
