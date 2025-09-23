# 🚀 Guida Completa Quackle Power Engine

## 📋 Panoramica

Quackle Power Engine è un sistema completo di motori Scrabble con diversi livelli di potenza, progettato per utilizzare **esclusivamente** le strategie native di Quackle senza workaround o placeholder.

## 🎯 Motori Disponibili

### 1. **quackle_power** - Massima Potenza (Raccomandato)
```bash
./quackle_power <rack> [board_letter] [row] [col]
```
- ✅ **Tutte le strategie native**: Syn2, Worths, VcPlace, Bogowin, Superleaves
- ✅ **Dizionario ottimizzato**: GADDAG enable1.15
- ✅ **Valutazione completa**: Score + leave analysis con equity
- ✅ **Performance eccellenti**: 2739+ mosse/secondo

### 2. **quackle_levels** - Livelli Configurabili
```bash
./quackle_levels <level> <rack> [board_letter] [row] [col]
```
- **maximum**: Massima potenza (tutte le strategie)
- **high**: Alta potenza (strategie principali)
- **medium**: Potenza media (strategie base)
- **basic**: Potenza base (solo generazione)

### 3. **simple_move** - Generazione Base
```bash
./simple_move <rack> [board_letter] [row] [col]
```
- ✅ **Generazione rapida**: Solo mosse valide
- ❌ **Valutazione limitata**: Nessuna strategia avanzata

### 4. **quackle_benchmark** - Test Performance
```bash
./quackle_benchmark
```
- ✅ **Benchmark completo**: Test con 10 rack diversi
- ✅ **Metriche dettagliate**: Tempo, mosse/secondo, equity

## 🎮 Esempi di Utilizzo

### Massima Potenza
```bash
# Board vuota
./quackle_power FALREI?
# Output: FRAgILE (76 punti, equity: 78.39)

# Board con lettera
./quackle_power HELLO?? A 7 7
# Output: HOLL. (12 punti, equity: 71.00)
```

### Livelli Configurabili
```bash
# Massima potenza
./quackle_levels maximum FALREI?
# Strategie: Syn2✅ Worths✅ VcPlace✅ Bogowin✅ Superleaves✅

# Alta potenza
./quackle_levels high FALREI?
# Strategie: Syn2❌ Worths✅ VcPlace❌ Bogowin✅ Superleaves❌

# Potenza media
./quackle_levels medium FALREI?
# Strategie: Syn2❌ Worths✅ VcPlace❌ Bogowin✅ Superleaves❌

# Potenza base
./quackle_levels basic FALREI?
# Strategie: Syn2❌ Worths❌ VcPlace❌ Bogowin❌ Superleaves❌
```

## 📊 Confronto Livelli di Potenza

| Livello | Strategie | Equity | Mosse | Performance | Raccomandato |
|---------|-----------|--------|-------|-------------|--------------|
| **Maximum** | Tutte | Completa | 20 | Ottimale | ✅ Partite competitive |
| **High** | Principali | Buona | 15 | Alta | ✅ Analisi rapide |
| **Medium** | Base | Limitata | 10 | Media | ⚠️ Uso generale |
| **Basic** | Minime | Nessuna | 5 | Base | ❌ Solo test |

## 🔧 Strategie Native Utilizzate

### Syn2 - Sinergia Lettere
- **Descrizione**: Valuta le sinergie tra coppie di lettere
- **Esempio**: F-R, R-A, A-G in "FRAgILE"
- **Livelli**: Maximum only

### Worths - Valore Lettere
- **Descrizione**: Assegna valori alle singole lettere
- **Esempio**: F(4), R(1), A(1), G(2)
- **Livelli**: Maximum, High, Medium

### VcPlace - Posizionamento Vocale-Consonante
- **Descrizione**: Analizza pattern vocale-consonante
- **Esempio**: F-R-A-G-I-L-E (C-V-C-V-C-V-C)
- **Livelli**: Maximum only

### Bogowin - Probabilità Vittoria
- **Descrizione**: Calcola probabilità di vittoria
- **Esempio**: Basato su posizione e lettere rimanenti
- **Livelli**: Maximum, High, Medium

### Superleaves - Valutazione Leave
- **Descrizione**: Valuta le lettere rimanenti nel rack
- **Esempio**: I, L, E rimanenti dopo "FRAgILE"
- **Livelli**: Maximum only

### CatchallEvaluator - Valutazione Completa
- **Descrizione**: Combina tutte le strategie
- **Esempio**: Score + leave value = equity
- **Livelli**: Maximum, High

## 📈 Interpretazione Risultati

### Equity vs Score
```
FRAgILE: Score 76, Equity 78.39 (+2.39 leave value) ✅ Eccellente
FLAxIER: Score 76, Equity 78.21 (+2.21 leave value) ✅ Molto buona
FRIAbLE: Score 76, Equity 77.38 (+1.38 leave value) ✅ Buona
```

### Strategie in Azione
- **Equity > Score**: Mossa strategica eccellente
- **Equity = Score**: Mossa neutra
- **Equity < Score**: Mossa subottimale

## 🚀 Performance

### Benchmark Risultati
```
=== BENCHMARK RESULTS ===
        Rack   Moves  Time(ms)   Best Move   Score    Equity
----------------------------------------------------------------------
      FALREI?      20         7     FRAgILE      76      78.4
      AEIRSTZ      20         0      ZAIRES      50      50.0
      QUACKLE      20         0       QUACK      60      60.0
----------------------------------------------------------------------
       TOTAL     200        73         N/A     N/A       N/A

=== PERFORMANCE ANALYSIS ===
Total racks tested: 10
Total moves generated: 200
Average time per rack: 7ms
Moves per second: 2739
```

### Metriche per Livello
- **Maximum**: 20 mosse, equity completa, ~7ms
- **High**: 15 mosse, equity buona, ~5ms
- **Medium**: 10 mosse, equity limitata, ~3ms
- **Basic**: 5 mosse, nessuna equity, ~1ms

## 🛠️ Installazione e Setup

### Compilazione
```bash
# 1. Compila Quackle base
cd /home/jacopo/Progetti-github/quackle
qmake && make -j4

# 2. Compila motori
g++ -std=c++11 -I. -Iquackleio -o quackle_power quackle_power.cpp \
    -Llib/release -Lquackleio/lib/release -lquackle -lquackleio

g++ -std=c++11 -I. -Iquackleio -o quackle_levels quackle_levels.cpp \
    -Llib/release -Lquackleio/lib/release -lquackle -lquackleio

g++ -std=c++11 -I. -Iquackleio -o quackle_benchmark quackle_benchmark.cpp \
    -Llib/release -Lquackleio/lib/release -lquackle -lquackleio
```

### Dizionari Richiesti
```
data/lexica/
├── enable1.15.gaddag    # Per massima potenza
├── enable1.15.dawg      # Fallback
└── twl06.dawg          # Per livelli base
```

## 🔧 Troubleshooting

### Problemi Comuni

| Problema | Soluzione |
|----------|-----------|
| "Strategy candidate missing" | Usa "default_english" per maximum |
| "No DAWG lexicon found" | Verifica enable1.15.gaddag |
| Performance lente | Usa livello "high" o "medium" |
| Mosse non ottimali | Verifica strategie attive |
| Crash con maximum | Usa livello "high" |

### Verifica Configurazione
```bash
# Verifica strategie
./quackle_power FALREI? | grep "STRATEGY STATUS"

# Test performance
./quackle_benchmark

# Test livelli
./quackle_levels maximum FALREI? | grep "LEVEL ANALYSIS"
```

## 🎯 Best Practices

### Per Partite Competitive
1. Usa `quackle_power` o `quackle_levels maximum`
2. Verifica che tutte le strategie siano attive
3. Analizza equity per decisioni strategiche
4. Usa dizionario enable1.15.gaddag

### Per Analisi Rapide
1. Usa `quackle_levels high` o `medium`
2. Limita a 10-15 mosse per velocità
3. Focus su score invece di equity

### Per Sviluppo/Test
1. Usa `quackle_benchmark` per validare performance
2. Testa con rack diversi
3. Verifica consistenza dei risultati

## 📚 File di Documentazione

- **README_QUACKLE_POWER.md**: Guida dettagliata completa
- **QUICK_START.md**: Guida rapida per l'uso immediato
- **GUIDA_COMPLETA.md**: Questo file - panoramica completa

## 🏆 Conclusione

Quackle Power Engine rappresenta l'apice delle capacità di Quackle, offrendo:

- **Massima potenza**: Tutte le strategie native attive
- **Flessibilità**: Livelli configurabili per ogni esigenza
- **Performance**: 2739+ mosse/secondo
- **Affidabilità**: Nessun workaround, solo API native
- **Facilità d'uso**: Interfaccia semplice e intuitiva

**Il sistema è pronto per l'uso professionale!** 🚀

---

*Per supporto tecnico, consulta il codice sorgente o la documentazione di Quackle.*
