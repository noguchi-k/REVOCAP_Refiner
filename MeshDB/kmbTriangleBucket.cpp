/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.5                          #
# Class Name : TriangleBucket                                          #
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
#include "MeshDB/kmbTriangleBucket.h"
#include "MeshDB/kmbElementContainer.h"
#include "Geometry/kmb_Point3DContainer.h"
#include "Geometry/kmb_Geometry4D.h"

kmb::TriangleBucket::TriangleBucket(void)
: kmb::Bucket<kmb::elementIdType>()
, points(NULL)
, elements(NULL)
, coordMatrix(NULL)
{
}

kmb::TriangleBucket::TriangleBucket(const kmb::BoxRegion &box,int xnum,int ynum,int znum)
: kmb::Bucket<kmb::elementIdType>(box,xnum,ynum,znum)
, points(NULL)
, elements(NULL)
, coordMatrix(NULL)
{
}

kmb::TriangleBucket::~TriangleBucket(void)
{
}

void
kmb::TriangleBucket::setContainer(const kmb::Point3DContainer* points,const kmb::ElementContainer* elements,const kmb::Matrix4x4* coordMatrix)
{
	this->points = points;
	this->elements = elements;
	this->coordMatrix = coordMatrix;
}

int
kmb::TriangleBucket::append(kmb::elementIdType elementId)
{
	if( elements == NULL || points == NULL ){
		return 0;
	}
	int count = 0;
	kmb::ElementContainer::const_iterator eIter = elements->find(elementId);
	if( !eIter.isFinished() && eIter.getType() == kmb::TRIANGLE ){
		kmb::BoundingBox box;
		kmb::Point3D a,b,c;
		if( points->getPoint( eIter.getCellId(0), a ) &&
			points->getPoint( eIter.getCellId(1), b ) &&
			points->getPoint( eIter.getCellId(2), c ) )
		{
			if( coordMatrix ){
				coordMatrix->convert( a );
				coordMatrix->convert( b );
				coordMatrix->convert( c );
			}
			box.update(a);
			box.update(b);
			box.update(c);
			box.expand(1.1);
			int i0=0,j0=0,k0=0,i1=0,j1=0,k1=0;
			getSubIndices( box.getMin().x(), box.getMin().y(), box.getMin().z(), i0, j0, k0 );
			getSubIndices( box.getMax().x(), box.getMax().y(), box.getMax().z(), i1, j1, k1 );
			count += appendSubBucket(i0,i1,j0,j1,k0,k1,a,b,c,eIter.getId());
		}
	}
	return count;
}

int
kmb::TriangleBucket::appendAll(void)
{
	if( elements == NULL || points == NULL ){
		return 0;
	}
	int count = 0;
	kmb::BoundingBox bbox;
	kmb::BoxRegion box;
	kmb::Point3D a,b,c;
	kmb::ElementContainer::const_iterator eIter = elements->begin();
	while( !eIter.isFinished() ){
		if( eIter.getType() == kmb::TRIANGLE &&
			points->getPoint( eIter.getCellId(0), a ) &&
			points->getPoint( eIter.getCellId(1), b ) &&
			points->getPoint( eIter.getCellId(2), c ) )
		{
			if( coordMatrix ){
				coordMatrix->convert( a );
				coordMatrix->convert( b );
				coordMatrix->convert( c );
			}
			bbox.initialize();
			bbox.update(a);
			bbox.update(b);
			bbox.update(c);
			bbox.expand(1.1);
			int i0=0,j0=0,k0=0,i1=0,j1=0,k1=0;
			getSubIndices( bbox.getMin().x(), bbox.getMin().y(), bbox.getMin().z(), i0, j0, k0 );
			getSubIndices( bbox.getMax().x(), bbox.getMax().y(), bbox.getMax().z(), i1, j1, k1 );
			int subCount = appendSubBucket(i0,i1,j0,j1,k0,k1,a,b,c,eIter.getId());
			if( subCount == 0 ){
			}
			count += subCount;
		}
		++eIter;
	}
	return count;
}

int
kmb::TriangleBucket::appendSubBucket(int i0,int i1,int j0,int j1,int k0,int k1,kmb::Point3D &a,kmb::Point3D &b,kmb::Point3D &c,kmb::elementIdType elemId)
{
	if( i0 == i1 && j0 == j1 && k0 == k1 ){
		insert( i0,j0,k0,elemId );
		return 1;
	}

	int which = -1;
	if( i1-i0 >= j1-j0 ){
		if( i1-i0 >= k1-k0 ){

			which = 0;
		}else{

			which = 2;
		}
	}else{
		if( j1-j0 >= k1-k0 ){

			which = 1;
		}else{

			which = 2;
		}
	}
	kmb::Point3D p, q;
	switch( which ){
	case 0:
		{

			int i = (i0 + i1)/2;

			double x = getSubRegionMaxX(i,j0,k0);
			switch( kmb::PlaneYZ::getIntersectionTriangle(x,a,b,c,p,q) ){
			case -2:
				return appendSubBucket(i0,i,j0,j1,k0,k1,a,b,c,elemId);
			case -1:
				return appendSubBucket(i+1,i1,j0,j1,k0,k1,a,b,c,elemId);
			case 0:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,b,c,q,elemId) +
					appendSubBucket(i0,  i,j0,j1,k0,k1,b,q,p,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,a,p,q,elemId);
			case 1:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,c,a,q,elemId) +
					appendSubBucket(i0,  i,j0,j1,k0,k1,c,q,p,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,b,p,q,elemId);
			case 2:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,a,b,q,elemId) +
					appendSubBucket(i0,  i,j0,j1,k0,k1,a,q,p,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,c,p,q,elemId);
			case 3:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,a,p,q,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,b,c,q,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,b,q,p,elemId);
			case 4:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,b,p,q,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,c,a,q,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,c,q,p,elemId);
			case 5:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,c,p,q,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,a,b,q,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,a,q,p,elemId);
			case 6:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,a,p,c,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,a,b,p,elemId);
			case 7:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,b,p,a,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,b,c,p,elemId);
			case 8:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,c,p,b,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,c,a,p,elemId);
			case 9:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,a,b,p,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,a,p,c,elemId);
			case 10:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,b,c,p,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,b,p,a,elemId);
			case 11:
				return
					appendSubBucket(i0,  i,j0,j1,k0,k1,c,a,p,elemId) +
					appendSubBucket(i+1,i1,j0,j1,k0,k1,c,p,b,elemId);
			default:
				return 0;
			}
		}
	case 1:
		{

			int j = (j0 + j1)/2;

			double y = getSubRegionMaxY(i0,j,k0);
			switch( kmb::PlaneZX::getIntersectionTriangle(y,a,b,c,p,q) ){
			case -2:
				return appendSubBucket(i0,i1,j0,j,k0,k1,a,b,c,elemId);
			case -1:
				return appendSubBucket(i0,i1,j+1,j1,k0,k1,a,b,c,elemId);
			case 0:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,b,c,q,elemId) +
					appendSubBucket(i0,i1,j0, j, k0,k1,b,q,p,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,a,p,q,elemId);
			case 1:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,c,a,q,elemId) +
					appendSubBucket(i0,i1,j0, j, k0,k1,c,q,p,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,b,p,q,elemId);
			case 2:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,a,b,q,elemId) +
					appendSubBucket(i0,i1,j0, j, k0,k1,a,q,p,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,c,p,q,elemId);
			case 3:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,a,p,q,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,b,c,q,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,b,q,p,elemId);
			case 4:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,b,p,q,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,c,a,q,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,c,q,p,elemId);
			case 5:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,c,p,q,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,a,b,q,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,a,q,p,elemId);
			case 6:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,a,p,c,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,a,b,p,elemId);
			case 7:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,b,p,a,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,b,c,p,elemId);
			case 8:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,c,p,b,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,c,a,p,elemId);
			case 9:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,a,b,p,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,a,p,c,elemId);
			case 10:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,b,c,p,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,b,p,a,elemId);
			case 11:
				return
					appendSubBucket(i0,i1,j0, j, k0,k1,c,a,p,elemId) +
					appendSubBucket(i0,i1,j+1,j1,k0,k1,c,p,b,elemId);
			default:
				return 0;
			}
		}
	case 2:
		{

			int k = (k0 + k1)/2;

			double z = getSubRegionMaxZ(i0,j0,k);
			switch( kmb::PlaneXY::getIntersectionTriangle(z,a,b,c,p,q) ){
			case -2:
				return appendSubBucket(i0,i1,j0,j1,k0, k, a,b,c,elemId);
			case -1:
				return appendSubBucket(i0,i1,j0,j1,k+1,k1,a,b,c,elemId);
			case 0:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, b,c,q,elemId) +
					appendSubBucket(i0,i1,j0,j1,k0, k, b,q,p,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,a,p,q,elemId);
			case 1:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, c,a,q,elemId) +
					appendSubBucket(i0,i1,j0,j1,k0, k, c,q,p,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,b,p,q,elemId);
			case 2:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, a,b,q,elemId) +
					appendSubBucket(i0,i1,j0,j1,k0, k, a,q,p,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,c,p,q,elemId);
			case 3:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, a,p,q,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,b,c,q,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,b,q,p,elemId);
			case 4:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, b,p,q,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,c,a,q,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,c,q,p,elemId);
			case 5:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, c,p,q,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,a,b,q,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,a,q,p,elemId);
			case 6:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, a,p,c,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,a,b,p,elemId);
			case 7:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, b,p,a,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,b,c,p,elemId);
			case 8:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, c,p,b,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,c,a,p,elemId);
			case 9:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, a,b,p,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,a,p,c,elemId);
			case 10:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, b,c,p,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,b,p,a,elemId);
			case 11:
				return
					appendSubBucket(i0,i1,j0,j1,k0, k, c,a,p,elemId) +
					appendSubBucket(i0,i1,j0,j1,k+1,k1,c,p,b,elemId);
			default:
				return 0;
			}
		}
	default:
		return 0;
	}
}
