#pragma once

#include <vector>
#include <list>

// Prototype so the S vector can refer to it
struct walsh_vector_entry;

struct s_vector_entry {
	double value;
	std::list<walsh_vector_entry*> walsh_coefficients;
	std::list<s_vector_entry*>::iterator buffer_entry;
	int bitstring_index;

	void flip();
};

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

