#!/usr/bin/env bash
set -euo pipefail

# Simple helper to generate a .gaddag from a word list using the
# makegaddag tool in this repo. Requires Qt and a built makegaddag binary.

if [[ $# -lt 2 ]]; then
  echo "Usage: $0 <wordlist.txt> <output.gaddag> [alphabet]" >&2
  echo "Example: $0 data/lexicons/enable1.txt enable1.gaddag english" >&2
  exit 1
fi

WORDLIST="$1"
OUTFILE="$2"
ALPHABET="${3:-english}"

if [[ ! -f "$WORDLIST" ]]; then
  echo "Word list not found: $WORDLIST" >&2
  exit 1
fi

# Locate makegaddag binary (built via qmake or your IDE)
MAKEGADDAG_BIN="./makegaddag/makegaddag"
if [[ ! -x "$MAKEGADDAG_BIN" ]]; then
  MAKEGADDAG_BIN="./makegaddag"
fi

if [[ ! -x "$MAKEGADDAG_BIN" ]]; then
  echo "makegaddag binary not found. Build it with qmake or your IDE." >&2
  echo "Example:" >&2
  echo "  qmake makegaddag.pro && make -j" >&2
  exit 1
fi

TMPDIR=$(mktemp -d)
trap 'rm -rf "$TMPDIR"' EXIT

cp "$WORDLIST" "$TMPDIR/gaddaginput.raw"
"$MAKEGADDAG_BIN" -f "$TMPDIR/gaddaginput.raw" -o "$OUTFILE" -a "$ALPHABET"
echo "Wrote $OUTFILE"

