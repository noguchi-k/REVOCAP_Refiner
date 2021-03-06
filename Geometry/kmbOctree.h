/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Octree                                                  #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

/*
 * 節点または要素を位置情報に基づいて階層的に格納する
 * テンプレートクラス
 *
 * T = nodeIdType or elementIdType
 */

/*
 * T を idarray に maxCount まで格納する
 * 格納できなくなったら box を center で分けて、
 * 8個の子供に分配する
 * 現在格納されている個数を count に記録する
 */

#include "Geometry/kmbGeometry3D.h"
#include "Geometry/kmbBoundingBox.h"
#include "Common/kmbCalculator.h"
#include <cfloat>
#include <cmath>
#include <iostream>

#include <set>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100) // 使わない引数があっても警告を出さない for VC
#endif
#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869) // 使わない引数があっても警告を出さない for intel
#endif

namespace kmb{

template <typename T>
class Octree
{
protected:
	class OctreeNode{

	private:
		Octree<T>* octree;
		size_t count;
		size_t maxCount;
		T* idarray;
		typename kmb::Octree<T>::OctreeNode **children;
		kmb::BoundingBox box;
		kmb::Point3D center;
		int layer;

	public:
		OctreeNode(Octree<T>* oct,double minX=-1.0,double minY=-1.0,double minZ=-1.0,double maxX=1.0,double maxY=1.0,double maxZ=1.0,int l=0)
		: octree(oct)
		, count(0)
		, maxCount(0)
		, idarray(NULL)
		, children(NULL)
		, layer(l)
		{
			if( octree->maxExpand ){
				maxCount = oct->maxCount + oct->maxCount * layer / 4;
			}else{
				maxCount = oct->maxCount;
			}
			idarray = new T[maxCount];
			setRegion( minX, minY, minZ, maxX, maxY, maxZ );
		};

		virtual ~OctreeNode(void){
			clear();
		};

		void clear(void){
			if( children != NULL )
			{
				for( int i=0; i<8; ++i )
				{
					if( children[i] != NULL )
					{
						delete children[i];
						children[i] = NULL;
					}
				}
				delete[] children;
				children = NULL;
			}
			count = 0;
			if( idarray != NULL ){
				delete[] idarray;
				idarray = NULL;
			}
		};

		// 自分の階層に含まれている点の個数
		size_t getLocalCount(void) const{
			return count;
		}

		// 自分より下の階層に含まれている点の個数
		size_t getCount(void) const{
			size_t sum = 0;
			if( children != NULL ){
				for(int i=0;i<8;++i){
					sum += children[i]->getCount();
				}
			}else{
				sum = count;
			}
			return sum;
		}

		void setCenter(double x,double y,double z){
			center.setCoordinate(x,y,z);
		}

		// 格納されている T 型の要素について最短距離を返す
		double getNearest(const double x,const double y,const double z,T &t) const{
			double d = getNearestSq(x,y,z,t);
			if( d > 0.0 ){
				return sqrt(d);
			}else{
				return 0.0;
			}
		}

		void setRegion(const double minx,const double miny,const double minz,const double maxx,const double maxy,const double maxz){
			box.setMinMax(minx,miny,minz,maxx,maxy,maxz);
			box.getCenter( center );
		}

		// 座標を与えて追加
		int appendByPoint(const kmb::Point3D &pt,const T t){
			box.update(pt);
			if( children != NULL )
			{
				return children[ getChildIndex( pt.x(), pt.y(), pt.z() ) ]->appendByPoint(pt,t);
			}
			else if( idarray != NULL )
			{
				idarray[count] = t;
				++count;
				if( count == maxCount ){
					createChildren();
				}
				return 1;
			}
			return 0;
		}

		// 領域を与えて追加
		// 重複していてもよい
		int appendByRegion(const kmb::BoxRegion &b,const T t){
			box.update(b);
			if( children != NULL )
			{
				int res = 0;
				bool bxmax = ( this->center.x() < b.maxX() );
				bool bymax = ( this->center.y() < b.maxY() );
				bool bzmax = ( this->center.z() < b.maxZ() );
				bool bxmin = ( this->center.x() > b.minX() );
				bool bymin = ( this->center.y() > b.minY() );
				bool bzmin = ( this->center.z() > b.minZ() );
				if( bxmax && bymax && bzmax ) res += children[7]->appendByRegion(b,t);
				if( bxmax && bymax && bzmin ) res += children[6]->appendByRegion(b,t);
				if( bxmax && bymin && bzmax ) res += children[5]->appendByRegion(b,t);
				if( bxmax && bymin && bzmin ) res += children[4]->appendByRegion(b,t);
				if( bxmin && bymax && bzmax ) res += children[3]->appendByRegion(b,t);
				if( bxmin && bymax && bzmin ) res += children[2]->appendByRegion(b,t);
				if( bxmin && bymin && bzmax ) res += children[1]->appendByRegion(b,t);
				if( bxmin && bymin && bzmin ) res += children[0]->appendByRegion(b,t);
//				REVOCAP_Debug_X("%d %d\n",this->layer,res);
				return res;
			}
			else if( idarray != NULL )
			{
				idarray[count] = t;
				++count;
				if( count == maxCount ){
					createChildren();
				}
//				REVOCAP_Debug_X("%d [1] %d\n",this->layer,count);
				return 1;
			}
			return 0;
		}

		int getInRegion(const kmb::Region* region, std::set<T> &tset) const{
			int res = 0;
			if( region == NULL ){
				return res;
			}
			if( children != NULL )
			{
				for(int i=0;i<8;++i){
					if( region->intersect( children[i]->box ) ){
						res += children[i]->getInRegion( region, tset );
					}
				}
			}
			else if( idarray != NULL )
			{
				for(unsigned int i=0;i<count;++i){
					if( octree->getIntersectToRegion( region, idarray[i] ) ){
						tset.insert( idarray[i] );
						++res;
					}
				}
			}
			return res;
		}

		void debugPrint(void) const {
			for(int i=0;i<layer;++i){
				std::cout << " ";
			}
			std::cout << getCount() << std::endl;
			if( children != NULL ){
				for(int i=0;i<8;++i){
					children[i]->debugPrint();
				}
			}
		}

	private:
		int getChildIndex(const double x,const double y,const double z) const{
			return
				( ( x > center.x() ) ? 4 : 0 ) +
				( ( y > center.y() ) ? 2 : 0 ) +
				( ( z > center.z() ) ? 1 : 0 ) ;
		}

		int getChildIndex(const bool bx,const bool by,const bool bz) const{
			return
				( ( bx ) ? 4 : 0 ) +
				( ( by ) ? 2 : 0 ) +
				( ( bz ) ? 1 : 0 );
		}

		// 子の階層を作る
		void createChildren(void){
			if( children == NULL )
			{
				// 原則として子の階層を作ったら center の値は変えてはいけない
				box.getCenter( center );
				children = new typename kmb::Octree<T>::OctreeNode*[8];
				children[0] = new typename kmb::Octree<T>::OctreeNode(octree,
					box.minX(), box.minY(), box.minZ(), center.x(), center.y(), center.z(), layer+1 );
				children[1] = new typename kmb::Octree<T>::OctreeNode(octree,
					box.minX(), box.minY(), box.maxZ(), center.x(), center.y(), center.z(), layer+1 );
				children[2] = new typename kmb::Octree<T>::OctreeNode(octree,
					box.minX(), box.maxY(), box.minZ(), center.x(), center.y(), center.z(), layer+1 );
				children[3] = new typename kmb::Octree<T>::OctreeNode(octree,
					box.minX(), box.maxY(), box.maxZ(), center.x(), center.y(), center.z(), layer+1 );
				children[4] = new typename kmb::Octree<T>::OctreeNode(octree,
					box.maxX(), box.minY(), box.minZ(), center.x(), center.y(), center.z(), layer+1 );
				children[5] = new typename kmb::Octree<T>::OctreeNode(octree,
					box.maxX(), box.minY(), box.maxZ(), center.x(), center.y(), center.z(), layer+1 );
				children[6] = new typename kmb::Octree<T>::OctreeNode(octree,
					box.maxX(), box.maxY(), box.minZ(), center.x(), center.y(), center.z(), layer+1 );
				children[7] = new typename kmb::Octree<T>::OctreeNode(octree,
					box.maxX(), box.maxY(), box.maxZ(), center.x(), center.y(), center.z(), layer+1 );
				// 子供に Id をコピー
				for(unsigned int i=0;i<count;++i)
				{
					octree->append(idarray[i],this);
				}
				this->count = 0;
				// children を持ったら idarray は NULL
				delete[] idarray;
				idarray = NULL;
			}
		}

		double getNearestSq(const double x,const double y,const double z,T &t) const{
			kmb::Minimizer min;
			if( octree == NULL ){
				return min.getMin();
			}
			if( children != NULL )
			{
				T tmp;
				bool bx = ( x > center.x() );
				bool by = ( y > center.y() );
				bool bz = ( z > center.z() );
//				REVOCAP_Debug_X("child index %d\n",getChildIndex(bx,by,bz));
				if( min.update( children[ getChildIndex(bx,by,bz) ]->getNearestSq(x,y,z,tmp) ) ){
					t = tmp;
				}
				// 隣接領域よりも近いかどうかを確かめる
				// min は更新されるので、毎回調べる
				// x
				if( (x-center.x())*(x-center.x()) < min.getMin() &&
					min.update( children[ getChildIndex(!bx,by,bz) ]->getNearestSq(x,y,z,tmp) ) )
				{
					t = tmp;
				}
				// y
				if( (y-center.y())*(y-center.y()) < min.getMin() &&
					min.update( children[ getChildIndex(bx,!by,bz) ]->getNearestSq(x,y,z,tmp) ) )
				{
					t = tmp;
				}
				// z
				if( (z-center.z())*(z-center.z()) < min.getMin() &&
					min.update( children[ getChildIndex(bx,by,!bz) ]->getNearestSq(x,y,z,tmp) ) )
				{
					t = tmp;
				}
				// x y
				if( (x-center.x())*(x-center.x()) < min.getMin() &&
					(y-center.y())*(y-center.y()) < min.getMin() &&
					min.update( children[ getChildIndex(!bx,!by,bz) ]->getNearestSq(x,y,z,tmp) ) )
				{
					t = tmp;
				}
				// y z
				if( (y-center.y())*(y-center.y()) < min.getMin() &&
					(z-center.z())*(z-center.z()) < min.getMin() &&
					min.update( children[ getChildIndex(bx,!by,!bz) ]->getNearestSq(x,y,z,tmp) ) )
				{
					t = tmp;
				}
				// z x
				if( (z-center.z())*(z-center.z()) < min.getMin() &&
					(x-center.x())*(x-center.x()) < min.getMin() &&
					min.update( children[ getChildIndex(!bx,by,!bz) ]->getNearestSq(x,y,z,tmp) ) )
				{
					t = tmp;
				}
				// x y z
				if( (x-center.x())*(x-center.x()) < min.getMin() &&
					(y-center.y())*(y-center.y()) < min.getMin() &&
					(z-center.z())*(z-center.z()) < min.getMin() &&
					min.update( children[ getChildIndex(!bx,!by,!bz) ]->getNearestSq(x,y,z,tmp) ) )
				{
					t = tmp;
				}
				return min.getMin();
			}
			else if( idarray != NULL )
			{
//				REVOCAP_Debug_X("tree count %d\n",count);
				for(unsigned int i=0;i<count;++i)
				{
					if( min.update( octree->getDistanceSq(x,y,z,idarray[i]) ) ){
						t = idarray[i];
					}
				}
//				REVOCAP_Debug_X("min = %f\n",min.getMin());
				return min.getMin();
			}
			return min.getMin();
		}
	};
protected:
	// OctreeNode は NULL にしない
	OctreeNode* topNode;
	size_t maxCount;
	// 子の階層に行くにしたがって maxCount を増やす
	bool maxExpand;
public:
	Octree(size_t mCount=256)
		: topNode(NULL)
		, maxCount(mCount)
		, maxExpand(false)
	{
		topNode = new typename kmb::Octree<T>::OctreeNode(this);
	};
	virtual ~Octree(void){
		if( topNode ){
			delete topNode;
			topNode = NULL;
		}
	};
	void clear(void){
		if( topNode ){
			delete topNode;
			topNode = NULL;
		}
		topNode = new typename kmb::Octree<T>::OctreeNode(this);
	}
	void setRegion(const kmb::BoxRegion &b){
		topNode->setRegion(b.minX(),b.minY(),b.minZ(),b.maxX(),b.maxY(),b.maxZ());
	}
	void setRegion(const double minx,const double miny,const double minz,const double maxx,const double maxy,const double maxz){
		topNode->setRegion(minx,miny,minz,maxx,maxy,maxz);
	}
	size_t getCount(void) const{
		return topNode->getCount();
	}
	// id を与えて追加
	// これは特殊化するクラスで実装する
	virtual int append(const T t,typename kmb::Octree<T>::OctreeNode* octNode=NULL) = 0;
	// コンテナの要素すべて追加
	// これは特殊化するクラスで実装する
	virtual int appendAll(typename kmb::Octree<T>::OctreeNode* octNode=NULL) = 0;
	// id を与えて距離の２乗を返す
	// これは特殊化するクラスで実装する
	// 注：getNearest から呼んでいるオーバーヘッドを減らせば少し速くなる
	virtual double getDistanceSq(const double x,const double y,const double z,const T t) const = 0;
	// 距離を返す
	double getNearest(const double x,const double y,const double z,T &t) const{
		return topNode->getNearest(x,y,z,t);
	}
	// 領域との関係を返す
	virtual bool getIntersectToRegion(const kmb::Region* region,const T t) const{
		return false;
	}
	// debug print
	void debugPrint(void){
		if( topNode ){
			topNode->debugPrint();
		}
	}

	class iterator
	{
	private:
		typename std::set<T>::iterator iter;
	public:
		iterator(void){}
		virtual ~iterator(void){}
		const T& get(void) const{ return *iter; };
		iterator& operator++(void){ ++iter; return *this; };

		iterator  operator++(int n){
			typename kmb::Octree<T>::iterator itClone;
			itClone.iter = iter; 
			iter++;
			return itClone;
		};

		bool operator==(const iterator& other) const{
			return this->iter == other.iter;
		};

		bool operator!=(const iterator& other) const{
			return this->iter != other.iter;
		};

		iterator& operator=(const iterator& other){
			iter = other.iter;
			return *this;
		};
	};

	class const_iterator
	{
	private:
		typename std::set<T>::const_iterator iter;
	public:
		const_iterator(void){}
		virtual ~const_iterator(void){}
		const T& get(void) const{ return *iter; };

		const_iterator& operator++(void){ ++iter; return *this; };

		const_iterator operator++(int n){
			typename kmb::Octree<T>::iterator itClone;
			itClone.iter = iter; 
			iter++;
			return itClone;
		};

		bool operator==(const const_iterator& other) const{
			return this->iter == other.iter;
		};

		bool operator!=(const const_iterator& other) const{
			return this->iter != other.iter;
		};

		const_iterator& operator=(const const_iterator& other){
			iter = other.iter;
			return *this;
		};

		const_iterator& operator=(const iterator& other){
			iter = other.iter;
			return *this;
		};
	};
};

}

#include "Geometry/kmbPoint3DContainer.h"

namespace kmb{

class OctreePoint3D : public Octree<kmb::nodeIdType>
{
private:
	const kmb::Point3DContainer* container;
public:
	OctreePoint3D(size_t mCount=256)
	: Octree<kmb::nodeIdType>(mCount)
	, container(NULL)
	{};

	virtual ~OctreePoint3D(void){};

	void setContainer(const kmb::Point3DContainer* c){
		container = c;
	}

	int append(const kmb::nodeIdType nodeId,kmb::Octree<kmb::nodeIdType>::OctreeNode* octNode=NULL)
	{
		if( container != NULL ){
			if( octNode == NULL ){
				octNode = topNode;
			}
			kmb::Point3D pt;
			container->getPoint(nodeId,pt);
			return octNode->appendByPoint(pt,nodeId);
		}
		return 0;
	}

	int appendAll(kmb::Octree<kmb::nodeIdType>::OctreeNode* octNode=NULL)
	{
		int res = 0;
		if( container != NULL ){
			if( octNode == NULL ){
				octNode = topNode;
			}
			kmb::Point3D pt;
			kmb::Point3DContainer::const_iterator pIter = container->begin();
			while( !pIter.isFinished() ){
				if( pIter.getPoint(pt) ){
					res += octNode->appendByPoint(pt,pIter.getId());
				}
				++pIter;
			}
		}
		return res;
	}

	double getDistanceSq(const double x,const double y,const double z,const kmb::nodeIdType nodeId) const
	{
		if( container != NULL ){
			kmb::Point3D pt;
			container->getPoint(nodeId,pt);
			return pt.distanceSq(x,y,z);
		}
		return DBL_MAX;
	}

	bool getIntersectToRegion(const kmb::Region* region,const kmb::nodeIdType nodeId) const{
		if( region == NULL || container == NULL ){
			return false;
		}
		kmb::Point3D pt;
		container->getPoint(nodeId,pt);
		return (region->intersect(pt) != kmb::Region::OUTSIDE);
	}

	void getNodesInRegion(const kmb::Region* region, std::set<kmb::nodeIdType> &nodes) const
	{
		if( region == NULL || topNode == NULL ){
			return;
		}
		topNode->getInRegion(region,nodes);
	}

};

}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif
