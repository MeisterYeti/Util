/*
	This file is part of the Util library.
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "ResourceAccessor.h"

#include <limits>
#include <sstream>

namespace Util {

void ResourceAccessor::assertRangeLocation(uint32_t index, uint16_t location) const {
	if(location >= format.getNumAttributes()) {
		std::ostringstream s;
		s << "Trying to access attribute at location " << location << " of overall " << format.getNumAttributes() << " attributes.";
		throw std::range_error(s.str());
	}
	if(index >= elementCount) {
		std::ostringstream s;
		s << "Trying to access element at index " << index << " of overall " << elementCount << " elements.";
		throw std::range_error(s.str());
	}
}

//-------------------

ResourceAccessor::ResourceAccessor(uint8_t* ptr, size_t size, ResourceFormat format) : format(format), dataPtr(ptr), dataSize(size), elementCount(size / format.getSize()) {
	const auto& attributes = format.getAttributes();
	for(uint32_t i=0; i<attributes.size(); ++i) {
		locations[attributes[i].getNameId()] = i;
		accessors[i] = AttributeAccessor::create(dataPtr, size, attributes[i], format.getSize());
	}
}

//-------------------

ResourceAccessor::~ResourceAccessor() { }

//-------------------

void ResourceAccessor::readRaw(size_t index, uint8_t* targetPtr, size_t count) {
	assertRangeLocation(index+count-1, 0);
	const uint8_t* ptr = dataPtr + index*format.getSize();
	std::copy(ptr, ptr + count*format.getSize(), targetPtr);
}

//-------------------

void ResourceAccessor::writeRaw(size_t index, const uint8_t* sourcePtr, size_t count) {
	assertRangeLocation(index+count-1, 0);
	uint8_t* ptr = dataPtr + index*format.getSize();
	std::copy(sourcePtr, sourcePtr + count*format.getSize(), ptr);
}

//-------------------

} /* Util */
