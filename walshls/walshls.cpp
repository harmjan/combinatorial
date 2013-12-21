#include <iostream>

#include <vector>
#include <list>
#include <unordered_map>

#include <algorithm>

#include <chrono>

#include <cassert>

const bool TIMING     = true;
const bool SHOW_STEPS = true && !TIMING;
const bool DEBUG      = false && !TIMING;

const unsigned int alpha = 10;

// Global variables for shared data
unsigned int n, k;
// The format of the file in memory
std::vector<std::pair<std::vector<int>,std::vector<double>>> subfunctions(n);

// Prototype so the S vector can refer to it
struct walsh_vector_entry;

struct s_vector_entry {
	double value;
	std::list<walsh_vector_entry*> walsh_coefficients;
	std::list<s_vector_entry*>::iterator buffer_entry;
	int bitstring_index;
};
std::vector<s_vector_entry> S;

struct walsh_vector_entry {
	double value;
	std::list<s_vector_entry*> influenced_sums;
};
std::unordered_map<std::vector<bool>, walsh_vector_entry> Wprime;

std::list<s_vector_entry*> B;

inline unsigned long long pow2(unsigned int exp) {
	return 1UL << exp;
}

void walsh_recursive(std::vector<double>& from, std::vector<double>& to, unsigned int start, unsigned int end) {
	unsigned int size = end-start;
	if( size == 1 )
		return;

	for( unsigned int i=start; i<start+size/2; ++i ) {
		to[i]        = (from[i] + from[size/2+i])/2;
		to[size/2+i] = (from[i] - from[size/2+i])/2;
	}
	walsh_recursive(to, from, start, start+size/2);
	walsh_recursive(to, from, start+size/2, end);
}

/**
 * Calculate the walsh coefficient for a vector of 2^k entries.
 */
inline std::vector<double> walsh_transform(const std::vector<double>& fitness_evaluations) {
	std::vector<double> walsh1(fitness_evaluations);
	std::vector<double> walsh2(pow2(k));
	walsh_recursive(walsh1, walsh2, 0, pow2(k));

	// The recursive functions swaps the vector to read to and
	// write from every recursion, there are k recursions deep.
	// Return the correct vector.
	if( k%2==0 )
		return walsh1;
	else
		return walsh2;
}

int main()
{
	std::cin >> n >> k;

	// Read in the start bitstring
	std::vector<bool> bitstring(n);
	for( unsigned int i=0; i<n; ++i ) {
		int tmp;
		std::cin >> tmp;
		bitstring[i] = tmp==1;
	}

	// Read in the subfunctions
	subfunctions.resize(n);
	for( unsigned int i=0; i<n; ++i ) {
		subfunctions[i].first.resize(k);
		subfunctions[i].second.resize(pow2(k));

		// Read the mapping in
		for( unsigned int j=0; j<k; ++j ) {
			std::cin >> subfunctions[i].first[j];
		}

		// Read the evaluations in
		for( unsigned int j=0; j<pow2(k); ++j ) {
			std::cin >> subfunctions[i].second[j];
		}
	}

	std::chrono::time_point<std::chrono::high_resolution_clock> preprocess, start, end;
	if( TIMING ) {
		preprocess = std::chrono::high_resolution_clock::now();
	}

	// Transform the subfunctions into the walsh coefficients
	// and initialize the Wprime and S datastructures
	S.resize(n);
	for( unsigned int i=0; i<n; ++i ) {
		// Calculate the subfunction walsh coefficients
		std::vector<double> walsh_coefficients = walsh_transform(subfunctions[i].second);

		if( DEBUG ) {
			std::cout << "Walsh coefficients of subfunction " << i << ": ";
			for( double coef : walsh_coefficients )
				std::cout << coef << " ";
			std::cout << std::endl;
		}

		// Loop over all possible 2^K walsh coefficients and add them to
		// the Wprime vector. Set up the pointers from Wprime to S and
		// vice versa.
		for( unsigned long long walsh_index=0; walsh_index<pow2(k); ++walsh_index ) {
			// Skip this walsh coefficient if it is zero
			if( walsh_coefficients[walsh_index] == 0 ) { // TODO epsilon compare
				continue;
			}

			// Loop over all bits in walsh_index; if a bit is
			// set, set the correct bit in the wprime_index
			std::vector<bool> wprime_index(n, false);
			for( unsigned int j=0; j<k; ++j ) {
				if( walsh_index & pow2(j) ) {
					wprime_index[subfunctions[i].first[k-j-1]] = true;
				}
			}

			// Calculate the correct sign
			// TODO This implementation is O(n), it can probably be done smarter
			int bitcount = 0;
			for( unsigned int j=0; j<n; ++j ) {
				if( bitstring[j] && wprime_index[j] )
					bitcount += 1;
			}

			// Add the walsh coefficient with the right sign to Wprime
			if( bitcount%2 == 0 ) {
				Wprime[wprime_index].value += walsh_coefficients[walsh_index];
			} else {
				Wprime[wprime_index].value -= walsh_coefficients[walsh_index];
			}

			// Since we now know that Wprime[wprime_index] is non-zero, add
			// the connections from Wprime[wprime_index] to S and vice versa
			// if they aren't there already
			if( Wprime[wprime_index].influenced_sums.empty() ) {
				// Loop over all bits in wprime_index, if a bit is set
				// it means that the value is influenced by the S vector
				// entry with that bit number, add the mutual pointer.
				for( unsigned int j=0; j<n; ++j ) {
					if( wprime_index[j] ) {
						Wprime[wprime_index].influenced_sums.push_front(&S[j]);
						S[j].walsh_coefficients.push_front(&Wprime[wprime_index]);
					}
				}
			}
		}
	}

	if( DEBUG ) {
		std::cout << std::endl << "Non-zero entries of Wprime vector:" << std::endl;
		for( auto entry : Wprime ) {
			std::cout << "\tWprime[";
			for( bool bit : entry.first )
				std::cout << bit;
			std::cout << "] = " << entry.second.value << std::endl;
		}
	}

	// Initialize the S vector and buffer B. Only the pointer between the
	// structures were setup in the previous fase, now the walsh coefficients
	// must be summed into the S vector value and added to buffer B.
	for( unsigned int i=0; i<n; ++i ) {
		S[i].value = 0;
		for( struct walsh_vector_entry* coef_index : S[i].walsh_coefficients ) {
			S[i].value += coef_index->value;
		}
		// Initialize the buffer iterator to the past the end iterator
		if( S[i].value < 0 ) {
			B.push_front(&S[i]);
			S[i].buffer_entry = B.begin();
		} else {
			S[i].buffer_entry = B.end();
		}
		S[i].bitstring_index = i;
	}

	if( DEBUG ) {
		std::cout << std::endl << "The S vector:" << std::endl;
		for( unsigned int i=0; i<n; ++i ) {
			std::cout << "\tS[" << i << "] = " << S[i].value << " with " << S[i].walsh_coefficients.size() << " walsh coefficients" << std::endl;
		}
	}

	if( DEBUG ) {
		std::cout << std::endl << "Buffer B:" << std::endl;
		unsigned int entry_num = 0;
		for( auto entry : B ) {
			std::cout << "\tB[" << entry_num << "] = S[" << entry->bitstring_index << "] = " << entry->value << std::endl;
			++entry_num;
		}
		std::cout << std::endl;
	}

	if( TIMING ) {
		start = std::chrono::high_resolution_clock::now();
	} else {
		std::cout << "Starting with bitstring ";
		for( bool bit : bitstring )
			std::cout << bit;
		double fitness = 0;
		for( auto entry : Wprime )
			fitness += entry.second.value;
		std::cout << " with initial fitness " << fitness << std::endl;
	}

	// Start local searching, as long as you have moves in the buffer B pick
	// a best approximation, do that move and update the structures.
	unsigned int iteration = 0;
	while( !B.empty() ) {
		// Get the next best move out of buffer B, if buffer B is larger than
		// alpha items pick the best of the first alpha items.
		s_vector_entry* flipTarget;
		{
			// Scan the first alpha items and save the best improving move
			unsigned int itemsScanned = 0;
			std::list<s_vector_entry*>::iterator bestMove = B.begin();
			for ( std::list<s_vector_entry*>::iterator i=bestMove ; itemsScanned < alpha && i != B.end() ; i++ )
			{
				// We want the lowest S vector value
				if ( (**i).value < (**bestMove).value )
					bestMove = i;
				itemsScanned++;
			}

			// Remove best move from the buffer
			flipTarget = *bestMove;
			flipTarget->buffer_entry = B.end();
			B.erase(bestMove);
			//TODO: sort?
		}

		std::list<s_vector_entry*> dirty; // We store updated partial sums here
		std::list<walsh_vector_entry*>& coefficients = flipTarget->walsh_coefficients;

		// Flip bit in bitstring
		bitstring[flipTarget->bitstring_index] = !bitstring[flipTarget->bitstring_index];

		// Update the S vector sums
		// For every walsh coefficient i that contains the bit being flipped
		for ( std::list<walsh_vector_entry*>::iterator i = coefficients.begin() ; i != coefficients.end() ; i++ )
		{
			// Flip walsh coefficient
			(*i)->value *= -1;

			// For every partial sum that this coefficient contributes to (including the bit just flipped)
			for ( std::list<s_vector_entry*>::iterator r = (*i)->influenced_sums.begin() ; r != (*i)->influenced_sums.end() ; r++ )
			{
				// Update partial sum
				(*r)->value += 2*((*i)->value);
				// Mark partial sum as dirty
				dirty.push_back(*r);
			}
		}

		if( DEBUG ) {
			std::cout << "The S vector after iteration " << iteration << ":" << std::endl;
			for( unsigned int i=0; i<n; ++i ) {
				std::cout << "\tS[" << i << "] = " << S[i].value << std::endl;
			}
		}

		// Update buffer B by looping over all S vector sums that were changed
		// and adding/removing the sums if necessary.
		for ( std::list<s_vector_entry*>::iterator r=dirty.begin(); r!=dirty.end(); ++r )
		{
			s_vector_entry *item = *r;
			// See if we need to add this S vector entry
			if ( item->value < 0 ) {
				// Only add this entry if it is not in the buffer
				if( item->buffer_entry == B.end() ) {
					B.push_front(item);
					item->buffer_entry = B.begin();
					//TODO: sort?
				}
			} else { // Otherwise see if we need to remove this S vector entry
				// Only remove this entry if it is in the buffer
				if( item->buffer_entry != B.end() ) {
					B.erase(item->buffer_entry);
					item->buffer_entry = B.end();
					//TODO: sort?
				}
			}
		}

		if( DEBUG ) {
			std::cout << "Buffer B after iteration " << iteration << ":" << std::endl;
			unsigned int entry_number = 0;
			for( auto it : B ) {
				std::cout << "\tB[" << entry_number << "] = S[" << it->bitstring_index << "] = " << it->value << std::endl;
				++entry_number;
			}
		}

		++iteration;

		if( SHOW_STEPS ) {
			std::cout << "\tIteration " << iteration << ": ";
			for( bool bit : bitstring )
				std::cout << bit;
			std::cout << " with fitness ";
			double fitness = 0;
			for( auto entry : Wprime )
				fitness += entry.second.value;
			std::cout << fitness << std::endl;
		}
	}

	if (DEBUG)
	{
		// Verify that result is a local optimum

		bool localOptimum = true;
		for ( auto entry : S )
			if ( entry.value < 0 )
				localOptimum = false;
		assert( localOptimum );

		// TODO verify local optimum outside of walsh space
	}

	if( TIMING ) {
		end = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> elapsed_seconds_preprocess = start - preprocess;
		std::chrono::duration<double> elapsed_seconds_iterating  = end - start;

		double fitness;
		for(auto entry : Wprime)
			fitness+=entry.second.value;

		std::cout << elapsed_seconds_preprocess.count() << " " << elapsed_seconds_iterating.count() << " " << iteration << " " << fitness;
	} else {
		std::cout << "Found answer in " << iteration << " iterations" << std::endl;
		std::cout << "Local optimum: ";
		for( bool bit : bitstring )
			std::cout << bit;
		double fitness=0;
		for(auto entry : Wprime)
			fitness+=entry.second.value;
		std::cout << " with total fitness: " << fitness << std::endl;
	}

	// Force a flush
	std::cout << std::flush;
}
