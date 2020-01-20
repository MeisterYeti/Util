/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019-2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_RESOURCES_RESOURCE_FORMAT_H_
#define UTIL_RESOURCES_RESOURCE_FORMAT_H_

#include "AttributeFormat.h"
#include "../StringIdentifier.h"
#include "../TypeConstant.h"
#include "../Utils.h"

#include <deque>

namespace Util {
	
//! @defgroup resources Resources

/** ResourceFormat
 * @ingroup resources
 */
class ResourceFormat {
public:
	using Attribute = AttributeFormat;	
	using AttributeContainer_t = std::deque<Attribute>;
		
	ResourceFormat(size_t _attributeAlignment=0) : attributeAlignment(_attributeAlignment) {}
	virtual ~ResourceFormat() = default;
	
	/*! Create and add a new attribute to the ResourceFormat.
		@param nameId The name of the attribute.
		@param type The base type of the attribute.
		@param vecSize The number of channels the attribute has (e.g., 3 for RGB or Vec3).
		@param colSize The number of columns for matrix types (e.g., 3 for Matrix3x3).
		@param arraySize The number of elements in an array of the attribute type.
		@param normalized Specifies that the underlying type is automatically converted to/from float value in the range [-1.0,1.0] or [0.0,1.0]
		@param internalType User defined internal type id (e.g., for compressed data). 
		@return the new attribute
		@note the owner of the attribute is the ResourceFormat
		@note Before using this function, check a default method can be used instead (e.g. @p appendFloat)
		@note When @p internalType is set, the @p type, @p vecSize, @p colSize and @p arraySize are still used for size calculation, 
		e.g., if a R10G10B10A2 attribute is packed into a single 32 bit integer, the @p vecSize should be 1.
	*/
	const Attribute& appendAttribute(const StringIdentifier& nameId, TypeConstant type, uint16_t vecSize=1, uint16_t colSize=1, uint32_t arraySize=1, bool normalized=false, uint32_t internalType=0);

	//! @see appendAttribute(const StringIdentifier& nameId, TypeConstant type, uint16_t vecSize=1, uint16_t colSize=1, uint32_t arraySize=1, bool normalized=false, uint32_t internalType=0)
	const Attribute& appendAttribute(const StringIdentifier& nameId, TypeConstant type, uint16_t vecSize=1, bool normalized=false, uint32_t internalType=0) {
		return appendAttribute(nameId, type, vecSize, 1, 1, normalized, internalType);
	}

	//! directly appends the attribute without recalculating offsets
	const Attribute& _appendAttribute(const StringIdentifier& nameId, TypeConstant type, uint16_t vecSize, uint16_t colSize, uint32_t arraySize, bool normalized, uint32_t internalType, size_t offset);
	
	//! Add an attribute with the given name and the given number of float values.
	const Attribute & appendFloat(const Util::StringIdentifier& nameId, uint32_t arraySize=1, bool normalized=false) {
		return appendAttribute(nameId, TypeConstant::FLOAT, 1, 1, arraySize, normalized);
	}

	//! Add a float vector attribute.
	const Attribute & appendFloatVec(const Util::StringIdentifier& nameId, uint16_t components, uint32_t arraySize=1, bool normalized=false) {
		return appendAttribute(nameId, TypeConstant::FLOAT, components, 1, arraySize, normalized);
	}

	//! Add a 2d float vector attribute.
	const Attribute & appendVec2(const Util::StringIdentifier& nameId, uint32_t arraySize=1, bool normalized=false) {
		return appendAttribute(nameId, TypeConstant::FLOAT, 2, 1, arraySize, normalized);
	}

	//! Add a 3d float vector attribute.
	const Attribute & appendVec3(const Util::StringIdentifier& nameId, uint32_t arraySize=1, bool normalized=false) {
		return appendAttribute(nameId, TypeConstant::FLOAT, 3, 1, arraySize, normalized);
	}

	//! Add a 4d float vector attribute.
	const Attribute & appendVec4(const Util::StringIdentifier& nameId, uint32_t arraySize=1, bool normalized=false) {
		return appendAttribute(nameId, TypeConstant::FLOAT, 4, 1, arraySize, normalized);
	}

	//! Add a float matrix attribute.
	const Attribute & appendFloatMat(const Util::StringIdentifier& nameId, uint16_t rows, uint16_t columns, uint32_t arraySize=1, bool normalized=false) {
		return appendAttribute(nameId, TypeConstant::FLOAT, rows, columns, arraySize, normalized);
	}
	
	//! Add a 3x3 float matrix attribute.
	const Attribute & appendMat3(const Util::StringIdentifier& nameId, uint32_t arraySize=1, bool normalized=false) {
		return appendAttribute(nameId, TypeConstant::FLOAT, 3, 3, arraySize, normalized);
	}
	
	//! Add a 4x4 float matrix attribute.
	const Attribute & appendMat4(const Util::StringIdentifier& nameId, uint32_t arraySize=1, bool normalized=false) {
		return appendAttribute(nameId, TypeConstant::FLOAT, 4, 4, arraySize, normalized);
	}

	//! Add an attribute with the given name and the given number of unsigned int values.
	const Attribute & appendUInt(const Util::StringIdentifier& nameId, uint32_t arraySize=1) {
		return appendAttribute(nameId, TypeConstant::UINT32, 1, 1, arraySize, false);
	}

	//! Add a uint32 vector attribute.
	const Attribute & appendUIntVec(const Util::StringIdentifier& nameId, uint16_t components, uint32_t arraySize=1, bool normalized=false) {
		return appendAttribute(nameId, TypeConstant::UINT32, components, 1, arraySize, normalized);
	}

	//! Add an attribute with the given name and the given number of int values.
	const Attribute & appendInt(const Util::StringIdentifier& nameId, uint32_t arraySize=1) {
		return appendAttribute(nameId, TypeConstant::INT32, 1, 1, arraySize, false);
	}

	//! Add a int32 vector attribute.
	const Attribute & appendIntVec(const Util::StringIdentifier& nameId, uint16_t components, uint32_t arraySize=1, bool normalized=false) {
		return appendAttribute(nameId, TypeConstant::INT32, components, 1, arraySize, normalized);
	}
		
	/*! Get a reference to the attribute with the corresponding name.
		\return Always returns an attribute.
				If the attribute is not present in the vertex description, it is empty.
		\note The owner of the attribute is the ResourceFormat, so be careful if the
				ResourceFormat is deleted or reassigned.*/
	const Attribute& getAttribute(const StringIdentifier& nameId) const;
	const Attribute& getAttribute(const std::string& name) const {
		return getAttribute(StringIdentifier(name));
	}
	const Attribute& getAttribute(uint32_t location) const {
		return attributes.at(location);
	}

	bool hasAttribute(const StringIdentifier& nameId) const;
	bool hasAttribute(const std::string& name) const {
		return hasAttribute(StringIdentifier(name));
	}
	
	//! Returns the location index of the attribute within the the resource format.
	const uint32_t getAttributeLocation(const StringIdentifier& nameId) const;
	const uint32_t getAttributeLocation(const std::string& name) const {
		return getAttributeLocation(StringIdentifier(name));
	}
	
	/**
	 * Update an existing attribute of or append a new attribute to the ResourceFormat.
	 *
	 * @param attr Attribute that contains the new data.
	 * @param recalculateOffsets If @p true, the offsets of all attributes will be recalculated (The size will be recalculated either way).
	 * @warning When manually setting the offsets, make sure that they fit within the sizes and offsets of the other attributes.
	 * Otherwise, unpredictable side effects can occur.
	 */
	void updateAttribute(const Attribute& attr);

	//! Merges this resource format with another.
	void merge(const ResourceFormat& other);

	//! Returns the number of attributes
	uint32_t getNumAttributes() const { return static_cast<uint32_t>(attributes.size()); }
	const AttributeContainer_t& getAttributes() const { return attributes; }

	/**
	 * Forcefully set the size of the resource format.
	 * This can be useful when requiring specific alignments.
	 * @note When adding/updating an attribute, the size gets recalculated.
	 * 
	 * @param value The size.
	 */ 
	void setSize(size_t value) { size = value; }
	size_t getSize() const { return size; }

	size_t getAlignment() const { return attributeAlignment; }

	std::string toString(bool formatted=false) const;
	bool operator==(const ResourceFormat& other) const;
	bool operator!=(const ResourceFormat& other) const;
	bool operator<(const ResourceFormat& other) const;
private:
	AttributeContainer_t attributes;
	size_t size = 0;
	size_t attributeAlignment;
};

} /* Util */


template <> struct std::hash<Util::ResourceFormat> {
	std::size_t operator()(const Util::ResourceFormat& format) const {
		std::size_t result = 0;
		Util::hash_combine(result, format.getSize());
		Util::hash_combine(result, format.getAlignment());
		for(const auto& attr : format.getAttributes())
			Util::hash_combine(result, attr);
		return result;
	}
};


#endif /* end of include guard: UTIL_RESOURCES_RESOURCE_FORMAT_H_ */
