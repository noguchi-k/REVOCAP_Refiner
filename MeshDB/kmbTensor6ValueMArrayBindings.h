/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : Tensor6ValueMArrayBindings                              #
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
#pragma once
#include "MeshDB/kmbDataBindings.h"
#include "Geometry/kmb_BLArray.h"

namespace kmb{

class Tensor6ValueMArrayBindings : public kmb::DataBindings
{
private:
	BLArray<double,6> vArray;
	BLArrayIndex aIndex;
	size_t count;
public:
	Tensor6ValueMArrayBindings(size_t count,kmb::DataBindings::bindingMode bmode=kmb::DataBindings::NODEVARIABLE);
	virtual ~Tensor6ValueMArrayBindings(void);
	virtual const char* getContainerType(void) const{
		return "Tensor6ValueMArrayBindings";
	};
	virtual void clear(void);
	virtual bool setPhysicalValue(kmb::idType id,kmb::PhysicalValue* val);
	virtual bool setPhysicalValue(kmb::idType id,double* val);
	virtual bool setValue(kmb::idType id, double value,int index=0);
	virtual kmb::PhysicalValue* getPhysicalValue(kmb::idType id) const;
	virtual bool getPhysicalValue(kmb::idType id,double *val) const;
	virtual bool hasId(kmb::idType id) const;
	virtual bool deleteId(kmb::idType id);
	virtual bool replaceId(kmb::idType old_id,kmb::idType new_id);
	virtual size_t getIdCount() const;
public:
	class _iteratorMArray : public DataBindings::_iterator
	{
		friend class Tensor6ValueMArrayBindings;
		_iteratorMArray(void);
		virtual ~_iteratorMArray(void);
		virtual kmb::idType getId(void) const;
		virtual bool getFace(kmb::Face &f) const;
		virtual kmb::PhysicalValue* getValue(void) const;
		virtual bool getValue(double *value) const;
		virtual DataBindings::_iterator* operator++(void);
		virtual DataBindings::_iterator* operator++(int n);
		virtual DataBindings::_iterator* clone(void);
	private:
		const kmb::Tensor6ValueMArrayBindings* values;
		kmb::BLArrayIndex aIndex;
	};
	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
};

}