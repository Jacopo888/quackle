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
    int capacity() const { return (int)MAX_SIZE; }
    size_type maxSize() const { return MAX_SIZE; }
    void clear() { m_len = 0; }
    void push_back(char c);
    void pop_back();
    const char* constData() const { return m_data; }

    int compare(const FixedLengthString& s) const;

    FixedLengthString& operator+=(char c);
    FixedLengthString& operator+=(const FixedLengthString& s);

    const_reference operator[](size_type n) const { return m_data[n]; }
    FixedLengthString& operator=(const FixedLengthString &s);

    // Hardening methods
    bool sane() const { return m_len <= MAX_SIZE && m_data && (m_data + m_len) <= (m_data + MAX_SIZE); }
    bool isTruncated() const { return m_truncated; }

    static const unsigned int MAX_SIZE = FIXED_STRING_MAXIMUM_LENGTH;

  private:
    static const std::string dummyString; // just to get to traits
    char m_data[MAX_SIZE];
    size_type m_len; // length of string (no null terminator)
    bool m_truncated; // flag for silent truncation in release mode
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
    : m_len(0), m_truncated(false)
{
    // CRITICAL FIX: Initialize buffer to prevent garbage data
    memset(m_data, 0, MAX_SIZE);
}

inline
FixedLengthString::FixedLengthString(const char* s, size_type n)
    : m_truncated(false)
{
    // Hardening: clamp to MAX_SIZE
    if (n >= MAX_SIZE) {
        n = MAX_SIZE - 1;
        m_truncated = true;
    }
    
    // CRITICAL FIX: Initialize buffer first
    memset(m_data, 0, MAX_SIZE);
    if (n > 0) {
        memcpy(m_data, s, n);
    }
    m_len = n;
    
    // CRITICAL FIX: Add null terminator
    m_data[m_len] = '\0';
    
    assert(sane());
}

inline
FixedLengthString::FixedLengthString(size_type n, char c)
    : m_truncated(false)
{
    // Hardening: clamp to MAX_SIZE
    if (n >= MAX_SIZE) {
        n = MAX_SIZE - 1;
        m_truncated = true;
    }
    
    // CRITICAL FIX: Initialize buffer first
    memset(m_data, 0, MAX_SIZE);
    for (unsigned int i = 0; i < n; ++i) {
        m_data[i] = c;
    }
    m_len = n;
    
    // CRITICAL FIX: Add null terminator
    m_data[m_len] = '\0';
    
    assert(sane());
}

inline
FixedLengthString::FixedLengthString(const char* s)
    : m_truncated(false)
{
    size_t sz = strlen(s);
    
    // Hardening: clamp to MAX_SIZE
    if (sz >= MAX_SIZE) {
        sz = MAX_SIZE - 1;
        m_truncated = true;
    }
    
    // CRITICAL FIX: Initialize buffer first
    memset(m_data, 0, MAX_SIZE);
    if (sz > 0) {
        memcpy(m_data, s, sz);
    }
    m_len = sz;
    
    // CRITICAL FIX: Add null terminator
    m_data[m_len] = '\0';
    
    assert(sane());
}

inline
FixedLengthString::FixedLengthString(const FixedLengthString& s)
    : m_truncated(false)
{
    size_type len = s.size();
    
    // Hardening: clamp to MAX_SIZE
    if (len >= MAX_SIZE) {
        len = MAX_SIZE - 1;
        m_truncated = true;
    }
    
    // CRITICAL FIX: Initialize buffer first
    memset(m_data, 0, MAX_SIZE);
    if (len > 0) {
        memcpy(m_data, s.m_data, len);
    }
    m_len = len;
    
    // CRITICAL FIX: Add null terminator
    m_data[m_len] = '\0';
    
    assert(sane());
}

inline
FixedLengthString::FixedLengthString(FixedLengthString&& s)
    : m_truncated(false)
{
    size_type sz = s.size();
    
    // Hardening: clamp to MAX_SIZE
    if (sz >= MAX_SIZE) {
        sz = MAX_SIZE - 1;
        m_truncated = true;
    }
    
    // CRITICAL FIX: Initialize buffer first
    memset(m_data, 0, MAX_SIZE);
    if (sz > 0) {
        memcpy(m_data, s.m_data, sz);
    }
    m_len = sz;
    
    // CRITICAL FIX: Add null terminator
    m_data[m_len] = '\0';
    
    assert(sane());
}

inline FixedLengthString & 
FixedLengthString::operator=(const FixedLengthString &s)
{
    if (this != &s) {
        size_type len = s.size();
        
        // Hardening: clamp to MAX_SIZE
        if (len >= MAX_SIZE) {
            len = MAX_SIZE - 1;
            m_truncated = true;
        } else {
            m_truncated = false;
        }
        
        // CRITICAL FIX: Initialize buffer first
        memset(m_data, 0, MAX_SIZE);
        if (len > 0) {
            memcpy(m_data, s.m_data, len);
        }
        m_len = len;
        
        // CRITICAL FIX: Add null terminator
        m_data[m_len] = '\0';
        
        assert(sane());
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
    // CRITICAL FIX: Use m_len instead of m_end to avoid corruption
    return m_data + m_len;
}

inline FixedLengthString::iterator
FixedLengthString::begin()
{
    return m_data;
}

inline FixedLengthString::iterator
FixedLengthString::end()
{
    // CRITICAL FIX: Use m_len instead of m_end to avoid corruption
    return m_data + m_len;
}

inline void
FixedLengthString::erase(const iterator i)
{
    if (i >= begin() && i < end()) {
        memmove(i, i+1, end() - i);
        --m_len;
        m_data[m_len] = '\0';
        assert(sane());
    }
}

inline FixedLengthString::size_type
FixedLengthString::length() const
{
    // CRITICAL FIX: Use m_len directly, no calculation needed
    return m_len;
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
    if (m_len < MAX_SIZE - 1) {
        m_data[m_len] = c;
        ++m_len;
        m_data[m_len] = '\0';
    } else {
        // Silent truncation in release mode
        m_truncated = true;
    }
    assert(sane());
    return *this;
}

inline FixedLengthString & 
FixedLengthString::operator+=(const FixedLengthString& s)
{
    size_type sz = s.size();
    size_type available = MAX_SIZE - 1 - m_len;
    
    if (sz > available) {
        sz = available;
        m_truncated = true;
    }
    
    if (sz > 0) {
        memcpy(m_data + m_len, s.m_data, sz);
        m_len += sz;
        m_data[m_len] = '\0';
    }
    
    assert(sane());
    return *this;
}

inline void
FixedLengthString::push_back(char c)
{
    if (m_len < MAX_SIZE - 1) {
        m_data[m_len] = c;
        ++m_len;
        m_data[m_len] = '\0';
    } else {
        // Silent truncation in release mode
        m_truncated = true;
    }
    assert(sane());
}

inline void
FixedLengthString::pop_back()
{
    if (m_len > 0) {
        --m_len;
        m_data[m_len] = '\0';
    }
    assert(sane());
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
