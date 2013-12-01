#include "structures.h"


void s_vector_entry::flip() {
	for( auto it=walsh_coefficients.begin(); it!=walsh_coefficients.end(); ++it ) {
		(*it)->flip();
	}
}
