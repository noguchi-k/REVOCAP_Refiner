/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : ElementContainerNArray                                  #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#include "MeshDB/kmbElementContainerMixedArray.h"
#include "MeshDB/kmbElement.h"
#include <algorithm>

#ifdef _MSC_VER
#pragma warning(disable:4100)
#pragma warning(disable:4996)
#endif

const char* kmb::ElementContainerMixedArray::CONTAINER_TYPE = "mixed_array";

kmb::ElementContainerMixedArray::ElementContainerMixedArray(size_t typeCounter[kmb::ELEMENT_TYPE_NUM])
: ElementContainer()
, size(0)
, nodeTable(NULL)
, etypeTable(NULL)
, indexTable(NULL)
, nodeTableDeletable(true)
, etypeTableDeletable(true)
, indexTableDeletable(true)
, nodeOffset(0)
, index(0)
, nindex(0)
, count(0)
{
	initialize(typeCounter);
}

kmb::ElementContainerMixedArray::ElementContainerMixedArray(size_t eSize)
: ElementContainer()
, size(0)
, nodeTable(NULL)
, etypeTable(NULL)
, indexTable(NULL)
, nodeTableDeletable(true)
, etypeTableDeletable(true)
, indexTableDeletable(true)
, nodeOffset(0)
, index(0)
, nindex(0)
, count(0)
{
	initialize(eSize);
}

kmb::ElementContainerMixedArray::ElementContainerMixedArray(size_t eSize,char* etype, kmb::nodeIdType *nodeTable, bool writable, kmb::nodeIdType offset )
: ElementContainer()
, size(0)
, nodeTable(NULL)
, etypeTable(NULL)
, indexTable(NULL)
, nodeTableDeletable(true)
, etypeTableDeletable(true)
, indexTableDeletable(true)
, nodeOffset(0)
, index(0)
, nindex(0)
, count(0)
{
	this->size = eSize;
	this->etypeTable = etype;
	this->nodeTable = nodeTable;
	this->nodeTableDeletable = false;
	this->etypeTableDeletable = false;
	this->nodeOffset = offset;
	this->indexTable = new size_t[size];
	if( !writable ){
		for(size_t i=0;i<size;++i){
			this->indexTable[i] = nindex;
			kmb::elementType etype = static_cast<kmb::elementType>(this->etypeTable[i]);
			++(this->typeCounter[ etype ]);
			nindex += kmb::Element::getNodeCount( etype );
		}
		this->index = size;
		this->count = size;
	}else{
		std::fill_n( indexTable, size, 0 );
		std::fill_n( etypeTable, size, kmb::UNKNOWNTYPE );
	}
}

kmb::ElementContainerMixedArray::~ElementContainerMixedArray(void)
{
	clear();
}

void
kmb::ElementContainerMixedArray::clear(void)
{
	kmb::ElementContainer::clear();
	if( nodeTableDeletable && nodeTable ){
		delete[] nodeTable;
		nodeTable = NULL;
	}
	if( etypeTableDeletable && etypeTable ){
		delete[] etypeTable;
		etypeTable = NULL;
	}
	if( indexTableDeletable && indexTable ){
		delete[] indexTable;
		indexTable = NULL;
	}
	count = 0;
}

void
kmb::ElementContainerMixedArray::initialize(size_t eSize)
{
	clear();
	index = 0;
	nindex = 0;
	count = 0;
	size_t nCount = eSize * kmb::Element::MAX_NODE_COUNT;
	size_t eCount = eSize;
	nodeTable = new kmb::nodeIdType[nCount];
	etypeTable = new char[eCount];
	indexTable = new size_t[eCount];
	std::fill_n( nodeTable, nCount, kmb::nullNodeId );
	std::fill_n( etypeTable, eCount, kmb::UNKNOWNTYPE );
	std::fill_n( indexTable, eCount, 0 );
	size = eCount;
}

void
kmb::ElementContainerMixedArray::initialize(size_t typeCounter[kmb::ELEMENT_TYPE_NUM])
{
	clear();
	index = 0;
	nindex = 0;
	count = 0;
	size_t nCount = getNodeTableSize(typeCounter);
	size_t eCount = getElementSize(typeCounter);
	nodeTable = new kmb::nodeIdType[nCount];
	etypeTable = new char[eCount];
	indexTable = new size_t[eCount];
	std::fill_n( nodeTable, nCount, kmb::nullNodeId );
	std::fill_n( etypeTable, eCount, kmb::UNKNOWNTYPE );
	std::fill_n( indexTable, eCount, 0 );
	size = eCount;
}

const char*
kmb::ElementContainerMixedArray::getContainerType(void) const
{
	return kmb::ElementContainerMixedArray::CONTAINER_TYPE;
}

size_t
kmb::ElementContainerMixedArray::getNodeTableSize(size_t typeCounter[kmb::ELEMENT_TYPE_NUM])
{
	size_t nSize = 0;
	for(int i=0;i<kmb::ELEMENT_TYPE_NUM;++i){
		int len = kmb::Element::getNodeCount( static_cast<kmb::elementType>(i) );
		if( len > 0 ){
			nSize += len * typeCounter[i];
		}
	}
	return nSize;
}

size_t
kmb::ElementContainerMixedArray::getElementSize(size_t typeCounter[kmb::ELEMENT_TYPE_NUM])
{
	size_t eSize = 0;
	for(int i=0;i<kmb::ELEMENT_TYPE_NUM;++i){
		int len = kmb::Element::getNodeCount( static_cast<kmb::elementType>(i) );
		if( len > 0 ){
			eSize += typeCounter[i];
		}
	}
	return eSize;
}

kmb::elementIdType
kmb::ElementContainerMixedArray::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes)
{
	if( nodeTable == NULL || etypeTable == NULL || indexTable == NULL || index >= size ){
		return kmb::Element::nullElementId;
	}

	while( index < size && etypeTable[index] != kmb::UNKNOWNTYPE ){
		++index;
	}
	if( index >= size ){
		return kmb::Element::nullElementId;
	}

	etypeTable[index] = static_cast<char>(etype);
	indexTable[index] = nindex;
	int len = kmb::Element::getNodeCount( etype );
	for(int i=0;i<len;++i){
		nodeTable[nindex+i] = nodes[i] + this->nodeOffset;
	}
	++(this->typeCounter[ etype ]);
	nindex += len;
	++index;
	++count;
	return static_cast< kmb::elementIdType >( offsetId + index - 1 );
}

kmb::elementIdType
kmb::ElementContainerMixedArray::addElement(kmb::elementType etype, const kmb::nodeIdType *nodes, const kmb::elementIdType id)
{
	int i = id - offsetId;
	if( nodeTable == NULL || etypeTable == NULL || indexTable == NULL ||
		i < 0 || static_cast<size_t>(i) >= size || etypeTable[i] != kmb::UNKNOWNTYPE ){
		return kmb::Element::nullElementId;
	}
	etypeTable[i] = static_cast<char>(etype);
	indexTable[i] = nindex;
	int len = kmb::Element::getNodeCount( etype );
	for(int j=0;j<len;++j){
		nodeTable[nindex+j] = nodes[j] + this->nodeOffset;
	}
	++(this->typeCounter[ etype ]);
	nindex += len;
	++count;
	return id;
}

bool
kmb::ElementContainerMixedArray::getElement(kmb::elementIdType id,kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	int i = id - offsetId;
	if( nodeTable == NULL || etypeTable == NULL || indexTable == NULL ||
		i < 0 || static_cast<size_t>(i) >= size || etypeTable[i] == kmb::UNKNOWNTYPE ){
		return false;
	}
	etype = static_cast<kmb::elementType>(etypeTable[i]);
	size_t ni = indexTable[i];
	int len = kmb::Element::getNodeCount( etype );
	for(int j=0;j<len;++j){
		nodes[j] = nodeTable[ni+j] - this->nodeOffset;
	}
	return true;
}

bool
kmb::ElementContainerMixedArray::deleteElement(const kmb::elementIdType id)
{
	int i = id - offsetId;
	if( nodeTable == NULL || etypeTable == NULL || indexTable == NULL ||
		i < 0 || static_cast<size_t>(i) >= size || etypeTable[i] == kmb::UNKNOWNTYPE ){
		return false;
	}
	kmb::elementType etype = static_cast<kmb::elementType>(etypeTable[i]);
	size_t ni = indexTable[i];
	int len = kmb::Element::getNodeCount( etype );
	for(int j=0;j<len;++j){
		nodeTable[ni+j] = kmb::nullNodeId;
	}
	--(this->typeCounter[ etype ]);
	etypeTable[i] = kmb::UNKNOWNTYPE;
	indexTable[i] = 0;
	return true;
}

bool
kmb::ElementContainerMixedArray::includeElement(const kmb::elementIdType id) const
{
	int i = id - offsetId;
	if( nodeTable == NULL || etypeTable == NULL || indexTable == NULL ||
		i < 0 || static_cast<size_t>(i) >= size || etypeTable[i] == kmb::UNKNOWNTYPE ){
		return false;
	}
	return true;
}

kmb::elementIdType
kmb::ElementContainerMixedArray::getMaxId(void) const
{
	return static_cast< kmb::elementIdType >( this->offsetId + this->size - 1 );
}

kmb::elementIdType
kmb::ElementContainerMixedArray::getMinId(void) const
{
	return this->offsetId;
}

size_t
kmb::ElementContainerMixedArray::getCount(void) const
{
	return this->count;
}



kmb::ElementContainerMixedArray::_iteratorMA::_iteratorMA(void)
: index(0)
, elementContainer(NULL)
{
}

kmb::ElementContainerMixedArray::_iteratorMA::~_iteratorMA(void)
{
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerMixedArray::_iteratorMA::clone(void)
{
	kmb::ElementContainerMixedArray::_iteratorMA* obj = NULL;
	obj = new kmb::ElementContainerMixedArray::_iteratorMA();
	obj->elementContainer = this->elementContainer;
	obj->index = this->index;
	return obj;
}

kmb::elementIdType
kmb::ElementContainerMixedArray::_iteratorMA::getId(void) const
{
	return static_cast< kmb::elementIdType >( index );
}

bool
kmb::ElementContainerMixedArray::_iteratorMA::getElement(kmb::elementType &etype,kmb::nodeIdType *nodes) const
{
	etype = static_cast<kmb::elementType>(elementContainer->etypeTable[index]);
	if( etype == kmb::UNKNOWNTYPE ){
		return false;
	}
	size_t ni = elementContainer->indexTable[index];
	int len = kmb::Element::getNodeCount( etype );
	for(int j=0;j<len;++j){
		nodes[j] = elementContainer->nodeTable[ni+j] - elementContainer->nodeOffset;
	}
	return true;
}

kmb::elementType
kmb::ElementContainerMixedArray::_iteratorMA::getType(void) const
{
	return static_cast<kmb::elementType>(elementContainer->etypeTable[index]);
}

kmb::nodeIdType
kmb::ElementContainerMixedArray::_iteratorMA::getCellId(int cellIndex) const
{
	kmb::elementType etype = static_cast<kmb::elementType>(elementContainer->etypeTable[index]);
	if( etype == kmb::UNKNOWNTYPE ){
		return kmb::nullNodeId;
	}
	size_t ni = elementContainer->indexTable[index];
	int len = kmb::Element::getNodeCount( etype );
	if( 0 <= cellIndex && cellIndex < len ){
		return elementContainer->nodeTable[ni+cellIndex] - elementContainer->nodeOffset;
	}
	return kmb::nullNodeId;
}

bool
kmb::ElementContainerMixedArray::_iteratorMA::setCellId(int cellIndex, kmb::nodeIdType nodeId)
{
	kmb::elementType etype = static_cast<kmb::elementType>(elementContainer->etypeTable[index]);
	if( etype == kmb::UNKNOWNTYPE ){
		return false;
	}
	size_t ni = elementContainer->indexTable[index];
	int len = kmb::Element::getNodeCount( etype );
	if( 0 <= cellIndex && cellIndex < len ){
		elementContainer->nodeTable[ni+cellIndex] = nodeId + elementContainer->nodeOffset;
		return true;
	}
	return false;
}

kmb::nodeIdType
kmb::ElementContainerMixedArray::_iteratorMA::operator[](const int cellIndex) const
{
	kmb::elementType etype = static_cast<kmb::elementType>(elementContainer->etypeTable[index]);
	if( etype == kmb::UNKNOWNTYPE ){
		return kmb::nullNodeId;
	}
	size_t ni = elementContainer->indexTable[index];
	int len = kmb::Element::getNodeCount( etype );
	if( 0 <= cellIndex && cellIndex < len ){
		return elementContainer->nodeTable[ni+cellIndex] - elementContainer->nodeOffset;
	}
	return kmb::nullNodeId;
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerMixedArray::_iteratorMA::operator++(void)
{
	while( ++index < elementContainer->size ){
		if( elementContainer->etypeTable[index] != kmb::UNKNOWNTYPE ){
			return this;
		}
	}
	return NULL;
}

kmb::ElementContainer::_iterator*
kmb::ElementContainerMixedArray::_iteratorMA::operator++(int n)
{
	index++;
	while( index < elementContainer->size ){
		if( elementContainer->etypeTable[index] != kmb::UNKNOWNTYPE ){
			return this;
		}
		index++;
	}
	return NULL;
}

kmb::ElementContainer::iterator
kmb::ElementContainerMixedArray::begin(void)
{
	if( nodeTable == NULL || etypeTable == NULL || indexTable == NULL || getCount() == 0 ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::ElementContainerMixedArray::_iteratorMA* _it = NULL;
	_it = new kmb::ElementContainerMixedArray::_iteratorMA();
	if( _it ){
		_it->elementContainer = this;
		_it->index = 0;
	}
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerMixedArray::begin(void) const
{
	if( nodeTable == NULL || etypeTable == NULL || indexTable == NULL || getCount() == 0 ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::ElementContainerMixedArray::_iteratorMA* _it = NULL;
	_it = new kmb::ElementContainerMixedArray::_iteratorMA();
	if( _it ){
		_it->elementContainer = this;
		_it->index = 0;
	}
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}

kmb::ElementContainer::iterator
kmb::ElementContainerMixedArray::find(kmb::elementIdType id)
{
	int i = id - offsetId;
	if( nodeTable == NULL || etypeTable == NULL || indexTable == NULL ||
		i < 0 || static_cast<size_t>(i) >= size || etypeTable[i] == kmb::UNKNOWNTYPE ){
		return kmb::ElementContainer::iterator(NULL);
	}
	kmb::ElementContainerMixedArray::_iteratorMA* _it = NULL;
	_it = new kmb::ElementContainerMixedArray::_iteratorMA();
	if( _it ){
		_it->elementContainer = this;
		_it->index = i;
	}
	return kmb::ElementContainer::iterator(_it,offsetId);
}

kmb::ElementContainer::const_iterator
kmb::ElementContainerMixedArray::find(kmb::elementIdType id) const
{
	int i = id - offsetId;
	if( nodeTable == NULL || etypeTable == NULL || indexTable == NULL ||
		i < 0 || static_cast<size_t>(i) >= size || etypeTable[i] == kmb::UNKNOWNTYPE ){
		return kmb::ElementContainer::const_iterator(NULL);
	}
	kmb::ElementContainerMixedArray::_iteratorMA* _it = NULL;
	_it = new kmb::ElementContainerMixedArray::_iteratorMA();
	if( _it ){
		_it->elementContainer = this;
		_it->index = i;
	}
	return kmb::ElementContainer::const_iterator(_it,offsetId);
}
