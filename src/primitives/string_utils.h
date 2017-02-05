#pragma once
#include <stdint.h>
#include "../primitives/hash.h"

class Stream;

// Non-blocking version of Stream.readBytesUntil('\n', ...). Returns line if found, or NULL if no line.
char *read_line(Stream &stream);

// Parses provided string to null-terminated array of trimmed strings.
// NOTE: Provided string is changed - null characters are added after words.
char **parse_words(char *str);

// Returns pointer to a null-terminated next word in *str (or NULL); updates *str to point to remaining string.
// NOTE: Provided string is changed - null characters are added after words.
char* next_word(char **str);

// Parses given string into a uint32 and returns true if the parsing is successful.
bool parse_uint32(const char *str, uint32_t *res);
bool parse_float(const char *str, float *res);


// Simple structure to hold both original word and its hash to help with comparisons.
// Very useful when paired with static hashed literals like "abc"_hash .
// If the parsed word is in the form "<word><num>", then we use hash "<word>#" and set idx = <num>.
struct HashedWord {
    char *word;
    uint32_t hash;
    uint32_t idx;
    operator unsigned long() const { return hash; } // By default, can be casted to uint32_t as a hash.
    inline bool as_uint32(uint32_t *res) { return parse_uint32(word, res); }
    inline bool as_float(float *res) { return parse_float(word, res); }
};

// Return a static, zero-terminated array of hashes of provided words.
HashedWord *hash_words(char **words);
