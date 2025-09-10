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

#ifndef QUACKLE_FIXEDSTRING_H
#define QUACKLE_FIXEDSTRING_H

#include <cstdio>
#include <cstdlib>

// Global invariant: maximum legal rack letters for EN is small (<= 15 is safe ceiling).
// Use a conservative compile-time max; adapt to your type if capacity is explicit.
#ifndef QUACKLE_FIXEDSTRING_CAPACITY
#define QUACKLE_FIXEDSTRING_CAPACITY 64
#endif

static inline void quackle_fls_abort(const char* where, int len, int cap) {
    fprintf(stderr, "[FLS][FATAL] %s: length=%d > capacity=%d\n", where, len, cap);
    abort();
}

#include <cassert>
#include <string>
#include <string.h>

#define FIXED_STRING_MAXIMUM_LENGTH 40

namespace Quackle
{

class FixedLengthString 
{
  public:
    typedef char* iterator;
    typedef const char* const_iterator;
    typedef unsigned int size_type;
    typedef char& reference;
    typedef const char& const_reference;

    FixedLengthString();
    FixedLengthString(const char* s, size_type n);
    FixedLengthString(size_type n, char c);
    FixedLengthString(const char* s);
    FixedLengthString(const FixedLengthString& s);
    FixedLengthString(FixedLengthString&& s);

    const_iterator begin() const;
    const_iterator end() const;
    iterator begin();
    iterator end();
    void erase(const iterator i);
    size_type length() const;
    FixedLengthString substr(size_type pos, size_type n) const;
    bool empty() const;
    size_type size() const { return length(); }
    int capacity() const { return (int)maxSize; }
    void clear() { m_end = m_data; }
    void push_back(char c);
    void pop_back();
    const char* constData() const { return m_data; }

    int compare(const FixedLengthString& s) const;

    FixedLengthString& operator+=(char c);
    FixedLengthString& operator+=(const FixedLengthString& s);

    const_reference operator[](size_type n) const { return m_data[n]; }
    FixedLengthString& operator=(const FixedLengthString &s);

    static const unsigned int maxSize = FIXED_STRING_MAXIMUM_LENGTH;

  private:
    static const std::string dummyString; // just to get to traits
    char m_data[maxSize];
    char* m_end; // points at the terminating NULL
};


inline FixedLengthString
operator+(const FixedLengthString &lhs, const FixedLengthString& rhs)
{
    FixedLengthString str(lhs);
    str += rhs;
    return str;
}

inline FixedLengthString
operator+(char lhs, const FixedLengthString& rhs)
{
    FixedLengthString str(1, lhs);
    str += rhs;
    return str;
}

inline FixedLengthString
operator+(const FixedLengthString &lhs, char rhs)
{
    FixedLengthString str(lhs);
    str += rhs;
    return str;
}

inline
FixedLengthString::FixedLengthString()
    : m_end(m_data)
{
    // CRITICAL FIX: Initialize buffer to prevent garbage data
    memset(m_data, 0, maxSize);
}

inline
FixedLengthString::FixedLengthString(const char* s, size_type n)
{
    assert(n < maxSize);
    // CRITICAL FIX: Initialize buffer first
    memset(m_data, 0, maxSize);
    memcpy(m_data, s, n);
    m_end = m_data + n;
    // CRITICAL FIX: Add null terminator
    *m_end = '\0';
}

inline
FixedLengthString::FixedLengthString(size_type n, char c)
    : m_end(m_data)
{
    assert(n < maxSize);
    // CRITICAL FIX: Initialize buffer first
    memset(m_data, 0, maxSize);
    for (unsigned int i = 0; i < n; ++i) {
	*m_end++ = c;
    }
    // CRITICAL FIX: Add null terminator
    *m_end = '\0';
}

inline
FixedLengthString::FixedLengthString(const char* s)
{
    size_t sz = strlen(s);
    assert(sz < maxSize);
    // CRITICAL FIX: Initialize buffer first
    memset(m_data, 0, maxSize);
    memcpy(m_data, s, sz);
    m_end = m_data + sz;
    // CRITICAL FIX: Add null terminator
    *m_end = '\0';
}

inline
FixedLengthString::FixedLengthString(const FixedLengthString& s)
{
    int cap = (int)maxSize;
    int len = s.size();
    
    // Only log when corruption is detected
    if (len < 0 || len > cap || len > QUACKLE_FIXEDSTRING_CAPACITY) {
        fprintf(stderr, "[FLS.copy-ctor][CORRUPT] src len=%d cap=%d (maxCap=%d)\n",
                len, cap, QUACKLE_FIXEDSTRING_CAPACITY);
        quackle_fls_abort("copy-ctor", len, cap);
    }
    
    // CRITICAL FIX: Initialize buffer first
    memset(m_data, 0, maxSize);
    // Use bounded copy even if we already checked
    if (len > 0) memcpy(m_data, s.m_data, (size_t)len);
    m_end = m_data + len;
    // CRITICAL FIX: Add null terminator
    *m_end = '\0';
}

inline
FixedLengthString::FixedLengthString(FixedLengthString&& s)
{
    int sz = s.size();
    // CRITICAL FIX: Initialize buffer first
    memset(m_data, 0, maxSize);
    memcpy(m_data, s.m_data, sz);
    m_end = m_data + sz;
    // CRITICAL FIX: Add null terminator
    *m_end = '\0';
}

inline FixedLengthString & 
FixedLengthString::operator=(const FixedLengthString &s)
{
    if (this != &s) {
        int cap = (int)maxSize;
        int len = s.size();
        
        // Only log when corruption is detected
        if (len < 0 || len > cap || len > QUACKLE_FIXEDSTRING_CAPACITY) {
            fprintf(stderr, "[FLS.assign-copy][CORRUPT] src len=%d cap=%d (maxCap=%d)\n",
                    len, cap, QUACKLE_FIXEDSTRING_CAPACITY);
            quackle_fls_abort("assign-copy", len, cap);
        }
        
                    // CRITICAL FIX: Initialize buffer first
                    fprintf(stderr, "[FLS.assign-copy] memset m_data=%p size=%d\n", m_data, (int)maxSize);
                    memset(m_data, 0, maxSize);
                    if (len > 0) {
                        fprintf(stderr, "[FLS.assign-copy] memcpy src=%p dst=%p size=%d\n", s.m_data, m_data, (int)len);
                        memcpy(m_data, s.m_data, (size_t)len);
                    }
        m_end = m_data + len;
        // CRITICAL FIX: Add null terminator
        *m_end = '\0';
        fprintf(stderr, "[FLS.assign-copy] m_end set to %p (m_data=%p + %d)\n", m_end, m_data, (int)len);
        
        // CRITICAL FIX: Verify m_end is valid
        if (m_end == nullptr || m_end < m_data || m_end > m_data + maxSize) {
            fprintf(stderr, "[FLS.assign-copy][CORRUPT] m_end=%p m_data=%p maxSize=%d\n", 
                    m_end, m_data, (int)maxSize);
            quackle_fls_abort("assign-copy-end-corruption", (int)(m_end - m_data), (int)maxSize);
        }
    }
    return *this;
}

inline FixedLengthString::const_iterator
FixedLengthString::begin() const
{
    return m_data;
}

inline FixedLengthString::const_iterator
FixedLengthString::end() const
{
    // CRITICAL FIX: Check for corruption
    if (m_end == nullptr || m_end < m_data || m_end > m_data + maxSize) {
        fprintf(stderr, "[FLS.end][CORRUPT] m_end=%p m_data=%p maxSize=%d\n", 
                m_end, m_data, (int)maxSize);
        quackle_fls_abort("end-corruption", (int)(m_end - m_data), (int)maxSize);
    }
    return m_end;
}

inline FixedLengthString::iterator
FixedLengthString::begin()
{
    return m_data;
}

inline FixedLengthString::iterator
FixedLengthString::end()
{
    return m_end;
}

inline void
FixedLengthString::erase(const iterator i)
{
    memmove(i, i+1, m_end - i);
    --m_end;
}

inline FixedLengthString::size_type
FixedLengthString::length() const
{
    size_type len = FixedLengthString::size_type(m_end - m_data);
    // CRITICAL FIX: Check for corruption
    if (len < 0 || len > maxSize || m_end < m_data || m_end > m_data + maxSize) {
        fprintf(stderr, "[FLS.length][CORRUPT] m_end=%p m_data=%p len=%d maxSize=%d\n", 
                m_end, m_data, (int)len, (int)maxSize);
        quackle_fls_abort("length-corruption", (int)len, (int)maxSize);
    }
    return len;
}

inline FixedLengthString
FixedLengthString::substr(size_type pos, size_type n) const
{
    assert(pos + n <= size());
    return FixedLengthString(&m_data[pos], n);
}

inline bool
FixedLengthString::empty() const
{
    return length() == 0;
}

inline FixedLengthString & 
FixedLengthString::operator+=(char c)
{
    assert(size() < maxSize - 1);
    *m_end++ = c;
    // CRITICAL FIX: Add null terminator
    *m_end = '\0';
    
    // CRITICAL FIX: Verify m_end is valid
    if (m_end == nullptr || m_end < m_data || m_end > m_data + maxSize) {
        fprintf(stderr, "[FLS.operator+=][CORRUPT] m_end=%p m_data=%p maxSize=%d\n", 
                m_end, m_data, (int)maxSize);
        quackle_fls_abort("operator+=end-corruption", (int)(m_end - m_data), (int)maxSize);
    }
    return *this;
}

inline FixedLengthString & 
FixedLengthString::operator+=(const FixedLengthString& s)
{
    int sz = s.size();
    assert(size() + sz < maxSize);
    memcpy(m_end, s.m_data, sz);
    m_end += sz;
    // CRITICAL FIX: Add null terminator
    *m_end = '\0';
    return *this;
}

inline void
FixedLengthString::push_back(char c)
{
    int cap = (int)maxSize;
    if (m_end - m_data < 0 || m_end - m_data >= cap) {
        fprintf(stderr, "[FLS][push_back] overflow: length=%d cap=%d\n", (int)(m_end - m_data), cap);
        quackle_fls_abort("push_back", (int)(m_end - m_data), cap);
    }
    
    // CRITICAL FIX: Verify m_end is valid before using it
    if (m_end == nullptr || m_end < m_data || m_end > m_data + maxSize) {
        fprintf(stderr, "[FLS.push_back][CORRUPT] m_end=%p m_data=%p maxSize=%d\n", 
                m_end, m_data, (int)maxSize);
        quackle_fls_abort("push_back-end-corruption", (int)(m_end - m_data), (int)maxSize);
    }
    
    *this += c;
}

inline void
FixedLengthString::pop_back()
{
    assert(size() > 0);
    
    // CRITICAL FIX: Verify m_end is valid before using it
    if (m_end == nullptr || m_end < m_data || m_end > m_data + maxSize) {
        fprintf(stderr, "[FLS.pop_back][CORRUPT] m_end=%p m_data=%p maxSize=%d\n", 
                m_end, m_data, (int)maxSize);
        quackle_fls_abort("pop_back-end-corruption", (int)(m_end - m_data), (int)maxSize);
    }
    
    m_end--;
}

inline int
FixedLengthString::compare(const FixedLengthString& s) const
{
    int size1 = size();
    int size2 = s.size();
    int sz = (size1 < size2) ? size1 : size2;
    for (int i = 0; i < sz; ++i) {
	if (m_data[i] < s.m_data[i]) {
	    return -1;
	} else if (m_data[i] > s.m_data[i]) {
	    return 1;
	}
    }
    if (size1 > size2) {
	return 1;
    } else if (size2 > size1) {
	return -1;
    }
    return 0;
}

inline bool
operator<(const Quackle::FixedLengthString &lhs, const Quackle::FixedLengthString& rhs)
{
    return (lhs.compare(rhs) < 0);
}


} // end namespace

inline bool
operator==(const Quackle::FixedLengthString &lhs, const Quackle::FixedLengthString& rhs)
{
    return (lhs.compare(rhs) == 0);
}

inline bool
operator!=(const Quackle::FixedLengthString &lhs, const Quackle::FixedLengthString& rhs)
{
    return (lhs.compare(rhs) != 0);
}

#endif
