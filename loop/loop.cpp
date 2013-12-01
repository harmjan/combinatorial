#include <list>
#include <vector>
#include "structures.h"


//
// Class declarations
//


class S : public std::vector<s_vector_entry> {};
class W : public std::vector<walsh_vector_entry> {};
class B : public std::list<s_vector_entry*> {};


//
// Function declarations
//


std::vector<bool> run(std::vector<bool> bitstring, std::vector<s_vector_entry>& S, std::vector<walsh_vector_entry>& Wprime,	std::list<s_vector_entry*>& B);
std::vector<bool> run(std::vector<bool> bitstring, S& S, W& W, B& B);
s_vector_entry* getNextBest(B& B);
void addToB(B& B, s_vector_entry* item);
void removeFromB(B& B, s_vector_entry* item);
int improvement(const s_vector_entry& move);



//
// Implementations
//



// Compatibility overload
std::vector<bool> run(
		std::vector<bool> bitstring,
		std::vector<s_vector_entry>& S,
		std::vector<walsh_vector_entry>& W,
		std::list<s_vector_entry*>& B)
{
	::S tS;
	::B tB;
	::W tW;
	tS.insert(tS.begin(), S.begin(), S.end());
	tB.insert(tB.begin(), B.begin(), B.end());
	tW.insert(tW.begin(), W.begin(), W.end());

	return run(bitstring, tS, tW, tB);
}


// Run algorithm
std::vector<bool> run(std::vector<bool> bitstring, S& S, W& W, B& B)
{
	// Do steepest descent while there are impoving moves
	while ( !B.empty() )
	{
		// Get next impoving move
		s_vector_entry* flipTarget = getNextBest(B);

		std::list<s_vector_entry*> dirty; // We store updated partial sums here

		std::list<walsh_vector_entry*>& coefficients = flipTarget->walsh_coefficients;

		// For every walsh coefficient i that contains the bit being flipped
		for ( std::list<walsh_vector_entry*>::iterator i = coefficients.begin() ; i != coefficients.end ; i++ )
		{
			// Flip walsh coefficient
			(*i)->value *= -1;

			// For every partial sum that this coefficient contributes to
			for ( std::list<s_vector_entry*>::iterator r = (*i)->influenced_sums.begin() ; r != (*i)->influenced_sums.end() ; r++ )
			{
				// Update partial sum
				(*r)->value += 2*(*i)->value;
				// Mark partial sum as dirty
				dirty.push_back(*r);
			}
		}

		// Update buffer B
		for ( std::list<s_vector_entry*>::iterator r = dirty.begin() ; r != dirty.end ; r++ )
		{
			if ( improvement(**r) > 0 )
				addToB(B, *r);
			else
				removeFromB(B, *r);
		}
	}

	return bitstring;
}


// Removes and returns the next best move from buffer B
s_vector_entry* getNextBest(B& B)
{
	// Scan the first a items
	int itemsScanned = 0;
	B::iterator bestMove = B.begin();
	for ( B::iterator i=B.begin() ; itemsScanned < a && i!=B.end ; i++ )
	{
		if ( improvement(**i) > improvement(**bestMove) )
			bestMove = i;
	}

	// Remove best move from the buffer
	s_vector_entry* rv = *bestMove;
	B.erase(bestMove);
	//TODO: sort?
	return rv;
}


// Adds a new move in B
void addToB(B& B, s_vector_entry* item)
{
	if ( item->buffer_entry != B.end() )
		return; // Already in B
	else
	{
		B.push_front(item);
		item->buffer_entry = B.begin();
		//TODO: sort?
	}

}


// Removes move from B
void removeFromB(B& B, s_vector_entry* item)
{
	if ( item->buffer_entry == B.end() )
		return; // Not in B
	else
	{
		B.erase(item->buffer_entry);
		item->buffer_entry = B.end();
		//TODO: sort?
	}
}


// Normalize for descent or ascent
// Returns positive value for imporoving moves
// Greater value means greater improvement
int improvement(const s_vector_entry& move)
{
	return move.value; // Descent
	//return -move.value; // Ascent
}