/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : Vector2ValueMArrayBindings                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2011/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include <cfloat>
#include "MeshDB/kmbVector2ValueMArrayBindings.h"

kmb::Vector2ValueMArrayBindings::Vector2ValueMArrayBindings(size_t size,kmb::DataBindings::bindingMode bmode)
: kmb::DataBindings()
, vArray(DBL_MAX)
, aIndex()
, count(0)
{
	this->type = kmb::PhysicalValue::VECTOR2;
	this->bMode = bmode;
	vArray.initialize(size);
	aIndex = vArray.getBLArrayIndex(0);
}

kmb::Vector2ValueMArrayBindings::~Vector2ValueMArrayBindings(void)
{
	clear();
}

void
kmb::Vector2ValueMArrayBindings::clear(void)
{
	vArray.clear();
	aIndex.clear();
	count = 0;
}

bool
kmb::Vector2ValueMArrayBindings::setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val)
{
	if( val && 0 <= id && val->getType() == kmb::PhysicalValue::VECTOR2 ){
		kmb::BLArrayIndex ind = vArray.getBLArrayIndex( static_cast<size_t>(id) );
		double q[2];
		reinterpret_cast< kmb::Vector2Value* >(val)->getValue(q);
		if( vArray.set( ind, q ) ){
			if( aIndex <= ind ){
				aIndex = ind;
				++aIndex;
			}
			++count;
			return true;
		}
	}
	return false;
}

bool
kmb::Vector2ValueMArrayBindings::setPhysicalValue(kmb::idType id,double* val)
{
	if( val && 0 <= id ){
		kmb::BLArrayIndex ind = vArray.getBLArrayIndex( static_cast<size_t>(id) );
		if( vArray.set( ind, val ) ){
			if( aIndex <= ind ){
				aIndex = ind;
				++aIndex;
			}
			++count;
			return true;
		}
	}
	return false;
}

bool
kmb::Vector2ValueMArrayBindings::setValue(kmb::idType id, double value,int index)
{
	if( 0 <= id && 0 <= index && index < 6 ){
		kmb::BLArrayIndex ind = vArray.getBLArrayIndex( static_cast<size_t>(id) );
		double q[2] = {0.0, 0.0};
		vArray.get( static_cast<size_t>(id), q );
		q[index] = value;
		if( vArray.set( ind, q ) ){
			if( aIndex <= ind ){
				aIndex = ind;
				++aIndex;
			}
			++count;
			return true;
		}
	}
	return false;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::PhysicalValue*
kmb::Vector2ValueMArrayBindings::getPhysicalValue(kmb::idType id) const
{
	return NULL;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

bool
kmb::Vector2ValueMArrayBindings::getPhysicalValue(kmb::idType id, double *val) const
{
	if( val && 0 <= id && id < static_cast<kmb::idType>(aIndex.getIndex()) ){
		if( vArray.get( static_cast<size_t>(id), val ) ){
			return true;
		}
	}
	return false;
}

bool
kmb::Vector2ValueMArrayBindings::hasId(kmb::idType id) const
{
	if( 0 <= id && id < static_cast<kmb::idType>(aIndex.getIndex()) ){
		if( vArray.has( static_cast<size_t>(id) ) ){
			return true;
		}
	}
	return false;
}

bool
kmb::Vector2ValueMArrayBindings::deleteId(kmb::idType id)
{
	if( 0 <= id && id < static_cast<kmb::idType>(aIndex.getIndex()) ){
		if( vArray.erase( static_cast<size_t>(id)) ){
			--count;
			return true;
		}
	}
	return false;
}

bool
kmb::Vector2ValueMArrayBindings::replaceId(kmb::idType old_id,kmb::idType new_id)
{
	kmb::idType size = static_cast<kmb::idType>(aIndex.getIndex());
	if( 0<= old_id && old_id < size && 0<= new_id && new_id < size ){
		double tmp_old[2], tmp_new[2];
		if( vArray.get( static_cast<size_t>(old_id), tmp_old ) &&
			vArray.get( static_cast<size_t>(new_id), tmp_new ) &&
			vArray.set( static_cast<size_t>(new_id), tmp_old ) &&
			vArray.set( static_cast<size_t>(old_id), tmp_new ) )
		{
			return true;
		}
	}
	return false;
}

size_t
kmb::Vector2ValueMArrayBindings::getIdCount() const
{
	return count;
}

kmb::DataBindings::iterator
kmb::Vector2ValueMArrayBindings::begin(void)
{
	if( this->getIdCount() == 0 ){
		return kmb::DataBindings::iterator(NULL);
	}
	kmb::Vector2ValueMArrayBindings::_iteratorMArray* _it = NULL;
	_it = new kmb::Vector2ValueMArrayBindings::_iteratorMArray();
	if( _it ){
		_it->values = this;
		_it->aIndex = this->vArray.getBLArrayIndex(0);
	}
	return kmb::DataBindings::iterator(_it);
}

kmb::DataBindings::const_iterator
kmb::Vector2ValueMArrayBindings::begin(void) const
{
	if( this->getIdCount() == 0 ){
		return kmb::DataBindings::const_iterator(NULL);
	}
	kmb::Vector2ValueMArrayBindings::_iteratorMArray* _it = NULL;
	_it = new kmb::Vector2ValueMArrayBindings::_iteratorMArray();
	if( _it ){
		_it->values = this;
		_it->aIndex = this->vArray.getBLArrayIndex(0);
	}
	return kmb::DataBindings::const_iterator(_it);
}

kmb::Vector2ValueMArrayBindings::_iteratorMArray::_iteratorMArray(void)
: kmb::DataBindings::_iterator()
, values( NULL )
, aIndex()
{
}

kmb::Vector2ValueMArrayBindings::_iteratorMArray::~_iteratorMArray(void)
{
}

kmb::idType
kmb::Vector2ValueMArrayBindings::_iteratorMArray::getId(void) const
{
	return static_cast<kmb::idType>(aIndex.getIndex());
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

bool
kmb::Vector2ValueMArrayBindings::_iteratorMArray::getFace(kmb::Face &f) const
{
	return false;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

kmb::PhysicalValue*
kmb::Vector2ValueMArrayBindings::_iteratorMArray::getValue(void) const
{
	return NULL;
}

bool
kmb::Vector2ValueMArrayBindings::_iteratorMArray::getValue(double *value) const
{
	if( values->vArray.get( aIndex, value ) ){
		return true;
	}
	return false;
}

kmb::DataBindings::_iterator*
kmb::Vector2ValueMArrayBindings::_iteratorMArray::operator++(void)
{
	do{
		++aIndex;
		if( values->vArray.has(aIndex) ){
			return this;
		}
	}while( values->vArray.valid(aIndex) );
	return NULL;
}

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869)
#endif

kmb::DataBindings::_iterator*
kmb::Vector2ValueMArrayBindings::_iteratorMArray::operator++(int n)
{
	do{
		aIndex++;
		if( values->vArray.has(aIndex) ){
			return this;
		}
	}while( values->vArray.valid(aIndex) );
	return NULL;
}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif

kmb::DataBindings::_iterator*
kmb::Vector2ValueMArrayBindings::_iteratorMArray::clone(void)
{
	kmb::Vector2ValueMArrayBindings::_iteratorMArray* _itClone = NULL;
	_itClone = new kmb::Vector2ValueMArrayBindings::_iteratorMArray();
	if( _itClone ){
		_itClone->values = this->values;
		_itClone->aIndex = this->aIndex;
	}
	return _itClone;
}

