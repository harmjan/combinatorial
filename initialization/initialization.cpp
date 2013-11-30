#include <iostream>
#include <algorithm>
#include <vector>
#include <list>

#include <cassert>

const bool SHOW_STEPS = true;

// Global variables (I know, I am lazy :))
int n, k;
// The format of the file in memory
std::vector<std::pair<std::vector<int>,std::vector<double>>> subfunctions(n);

// Prototype so the S vector can refer to it
struct walsh_vector_entry;

struct s_vector_entry {
	double value;
	std::list<walsh_vector_entry*> walsh_coefficients;
	std::list<s_vector_entry*>::iterator buffer_entry;

	void flip();
};
std::vector<s_vector_entry> S;

struct walsh_vector_entry {
	double value;
	std::list<s_vector_entry*> influenced_sums;

	void flip() {
		for( auto it=influenced_sums.begin(); it!=influenced_sums.end(); ++it ) {
			(*it)->value -= 2*this->value;
		}
		this->value *= -1;
	}
};
std::vector<walsh_vector_entry> Wprime;

void s_vector_entry::flip() {
	for( auto it=walsh_coefficients.begin(); it!=walsh_coefficients.end(); ++it ) {
		(*it)->flip();
	}
}

std::list<s_vector_entry*> B;

inline unsigned long long pow2(unsigned int exp) {
	return 1 << static_cast<long long>(exp);
}

void walsh_recursive(std::vector<double>& from, std::vector<double>& to, unsigned int start, unsigned int end) {
	unsigned int size = end-start;
	if( size == 1 )
		return;

	for( unsigned int i=start; i<start+size/2; ++i ) {
		to[i]        = from[i] + from[size/2+i];
		to[size/2+i] = from[i] - from[size/2+i];
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
	//std::cout << "n is " << n <<std::endl;
	// Read in the start bitstring
	std::vector<bool> bitstring(n);
	for( int i=0; i<n; ++i ) {
		int tmp;
		std::cin >> tmp;
		bitstring[i] = tmp==1;
	}

	// Read in the subfunctions
	subfunctions.resize(n);
	for( int i=0; i<n; ++i ) {
		subfunctions[i].first.resize(k);
		subfunctions[i].second.resize(pow2(k));

		// Read the mapping in
		for( int j=0; j<k; ++j ) {
			std::cin >> subfunctions[i].first[j];
		}

		// Read the evaluations in
		for( unsigned int j=0; j<pow2(k); ++j ) {
			std::cin >> subfunctions[i].second[j];
		}
	}

	// Transform the subfunctions into the walsh coefficients
	// and add them in wprime
	Wprime.resize(pow2(n));
	for( int i=0; i<n; ++i ) {
		// Calculate the subfunction walsh coefficients
		std::vector<double> walsh_coefficients = walsh_transform(subfunctions[i].second);
		//copy the walsh_coefficients to subfunctions vector in order to be used at the initialization
		subfunctions[i].second = walsh_coefficients;

		if( SHOW_STEPS ) {
			std::cout << "Walsh coefficients of subfunction " << i << ": ";
			for( double coef : walsh_coefficients )
				std::cout << coef << " ";
			std::cout << std::endl;
		}
	}

	// Setup the S vector
	S.resize(n);
	for( int i=0; i<n; ++i ) {

		//2^k combinations for every subfunction
		for(unsigned int r=1;r<=subfunctions[i].first.size();++r){
   			std::vector<bool> comb_instance(subfunctions[i].first.size());
			unsigned long long wprime_index=0;  //keeps the index for wprime entry from the combinations of bits 
			long w_coef_index_of_value =0;   //keeps index of the value of the coefficient at subfunctions.second vector
			
   			std::fill(comb_instance.begin() + r, comb_instance.end(), true); //combinations instance
   			do {
				wprime_index =0;
				w_coef_index_of_value =0;
       				for (unsigned int l = 0; l < subfunctions[i].first.size(); ++l) {
           				if (!comb_instance[l]) {
						wprime_index += 1<< subfunctions[i].first[l];  //calc the index for Wprime vector
						w_coef_index_of_value += 1<< (subfunctions[i].first.size()-l-1); //calc the index of the fitness 
					}
       				}
				
				//coef may have been initialized before, only add the current value
				if(!Wprime[wprime_index].influenced_sums.empty())
				{	
					Wprime[wprime_index].value += subfunctions[i].second[w_coef_index_of_value]; //add value to existed wprime coef value
					continue;
				}
				//put w_coef to wprime for first time
				Wprime[wprime_index].value = subfunctions[i].second[w_coef_index_of_value];
				for (unsigned int l = 0; l < subfunctions[i].first.size(); ++l) {
           				if (!comb_instance[l]) {  // the same comb_instance with the one above
						S[subfunctions[i].first[l]].walsh_coefficients.push_front(&Wprime[wprime_index]); //Si -> wprime
						Wprime[wprime_index].influenced_sums.push_front(&S[subfunctions[i].first[l]]); //wprime ->Si
					}				
				}
   			} while (std::next_permutation(comb_instance.begin(), comb_instance.end()));
		}
	
	}

	//calculation of subfunction fitness values of every Si
	for( int i=0; i<n; ++i ) {
		S[i].value=0;
		for(struct walsh_vector_entry* coef_index : S[i].walsh_coefficients)
    			S[i].value+=coef_index->value;  //calculates the initial sum of Si values
	}


	if( SHOW_STEPS ) {
		for( int i=0; i<n; ++i ) 
			std::cout << "Fitness values of subfunction " << i << ": " << S[i].value << std::endl;
	}

	// Initialize buffer B
	for( int i=0; i<n; ++i ) {
		if( S[i].value > 0 )
			B.push_back(&S[i]);
	}
}
