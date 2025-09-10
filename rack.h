/*
 *  Quackle -- Crossword game artificial intelligence and analysis tool
 *  Copyright (C) 2005-2019 Jason Katz-Brown, John O'Laughlin, and John Fultz.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QUACKLE_RACK_H
#define QUACKLE_RACK_H

#include "alphabetparameters.h"


namespace Quackle
{

class Move;

class Rack
{
public:
	// constructs a new empty rack
	Rack();

	// constructs a new rack containing letters in t
	Rack(const LetterString &tiles);

	// copy constructor
	Rack(const Rack &other);

	// move constructor
	Rack(Rack &&other) noexcept;

	// copy assignment operator
	Rack& operator=(const Rack &other);

	// move assignment operator
	Rack& operator=(Rack &&other) noexcept;

	// tiles like AEILNN?
	void setTiles(const LetterString &tiles);
	const LetterString &tiles() const;
	LetterString alphaTiles() const;

	// returns true if no tiles are on rack
	bool empty() const;

	// returns true if this rack has exactly the same letters as
	// the specified rack
	bool equals(const Rack &rack) const;

	// number of tiles on rack
	unsigned int size() const;

	// equivalent to operator-=(move.usedTiles())
	// and returns true if all tiles in used were found
	// in this rack and unloaded
	bool unload(const LetterString &used);

	void load(const LetterString &tiles);

	// same as above but nonmutating
	bool contains(const LetterString &used) const;

	void shuffle();

	// sum of scores of letters on rack
	int score() const;

	UVString xml() const;
	UVString toString() const;

private:
	LetterString m_tiles;
};

inline Rack::Rack()
{
	// CRITICAL FIX: Explicitly initialize m_tiles to prevent corruption
	m_tiles = Quackle::LetterString();
}

inline Rack::Rack(const LetterString &tiles)
{
    setTiles(tiles);
}

inline Rack::Rack(const Rack &other)
{
	// CRITICAL FIX: Initialize m_tiles first to prevent corruption
	m_tiles = Quackle::LetterString();
	// Then copy the content safely
	m_tiles = other.m_tiles;
}

inline Rack::Rack(Rack &&other) noexcept
{
	m_tiles = std::move(other.m_tiles);
}

inline Rack& Rack::operator=(const Rack &other)
{
	if (this != &other) {
		// CRITICAL FIX: Initialize m_tiles first to prevent corruption
		m_tiles = Quackle::LetterString();
		// Then copy the content safely
		m_tiles = other.m_tiles;
	}
	return *this;
}

inline Rack& Rack::operator=(Rack &&other) noexcept
{
	if (this != &other) {
		m_tiles = std::move(other.m_tiles);
	}
	return *this;
}

inline void Rack::setTiles(const LetterString &tiles)
{
	m_tiles = tiles;
}

inline const LetterString &Rack::tiles() const
{
	return m_tiles;
}

inline bool Rack::empty() const
{
	return m_tiles.empty();
}

}

const Quackle::Rack operator-(const Quackle::Rack &rack, const Quackle::Move &move);
const Quackle::Rack operator-(const Quackle::Rack &rack1, const Quackle::Rack &rack2);

UVOStream &operator<<(UVOStream &o, const Quackle::Rack &rack);

#endif
