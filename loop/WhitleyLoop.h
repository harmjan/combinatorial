#pragma once

#include <vector>
#include <list>
#include <set>

#include "structures.h"


class S : public std::vector<s_vector_entry> {};
class W : public std::vector<walsh_vector_entry> {};
class B	: public std::list<s_vector_entry*> {};


class WhitleyLoop
{
	static int a;
public:
	WhitleyLoop(void);
	~WhitleyLoop(void);

	// Compatibility overload
	static std::vector<bool> run(
		std::vector<bool> bitstring,
		std::vector<s_vector_entry>& S,
		std::vector<walsh_vector_entry>& Wprime,
		std::list<s_vector_entry*>& B
	);

	// Run algorithm
	static std::vector<bool> run(
		std::vector<bool> bitstring,
		S& S,
		W& W,
		B& B
	);

	// Abstractions over operations on B
	static s_vector_entry* getNextBest(B& B);
	static void addToB(B& B, s_vector_entry* item);
	static void removeFromB(B& B, s_vector_entry* item);

	// Normalize for descent or ascent
	// Returns positive value for imporoving moves
	// Greater value means greater improvement
	static int improvement(const s_vector_entry& move)
	{
		return move.value; // Descent
		//return -move.value; // Ascent
	}
};

