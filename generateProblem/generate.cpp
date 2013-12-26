#include <iostream>

#include <vector>

std::vector<std::pair<std::vector<int>,std::vector<double>>> subfunctions;

int main(int argc,char *argv[]) {
	int n, k;
	std::cin >> n >> k;

	if( n < k)
		return -1;

	srand(time(NULL));

	// Generate the start bitstring
	std::vector<bool> bitstring(n);
	for( int i=0; i<n; ++i ) {
		bitstring[i] = rand()%2==0;
	}

	// Generate the subfunctions
	subfunctions.resize(n);
	for( int i=0; i<n; ++i ) {
		// While generating the dependencies make sure you don't generate
		// the same dependency twice per subfunction. Also make sure that
		// the first dependency is the bitnumber so every bit exists at
		// least in 1 subfunction.
		subfunctions[i].first.resize(k);
		if( k > 0 )
			subfunctions[i].first[0] = i;
		for( int j=1; j<k; ++j ) {
			int new_dependency;
			bool unique;
			do {
				unique = true;
				new_dependency = rand() % n;
				for( int k=0; k<j; ++k ) {
					if( subfunctions[i].first[k] == new_dependency ) {
						unique = false;
						break;
					}
				}
			} while(!unique);
			subfunctions[i].first[j] = new_dependency;
		}

		// Generate the fitness values
		subfunctions[i].second.resize(1<<k);
		for( int j=0; j<(1<<k); ++j ) {
			// Make the value 0 with 50% change
			if( rand() % 100 < 50 )
				subfunctions[i].second[j] = 0;
			else
				subfunctions[i].second[j] = static_cast<double>(rand()) / static_cast<double>(RAND_MAX/2) - 1;
		}
	}

	// Output the generated instance
	std::cout << n << " " << k << std::endl;
	for( bool bit : bitstring )
		std::cout << bit << " ";
	std::cout << std::endl;
	for( auto subfunc : subfunctions ) {
		for( int depend : subfunc.first )
			std::cout << depend << " ";
		std::cout << std::endl;
		for( double fitness : subfunc.second )
			std::cout << fitness << " ";
		std::cout << std::endl;
	}
	std::cout << std::flush;
}
