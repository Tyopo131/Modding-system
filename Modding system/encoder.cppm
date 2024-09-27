#include "json.hpp"
export module mod_format;

import <string>;
import <memory>;
import <fstream>;



export namespace modFormat {
	class encoded_mod {
		std::shared_ptr<char> data;
		size_t size;
	public:
		size_t getSize() const { return size; }
		std::shared_ptr<char> getData() const { return std::shared_ptr<char>(data); }
		encoded_mod(const char* data, )
	};
}