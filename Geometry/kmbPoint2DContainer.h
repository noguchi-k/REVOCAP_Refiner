/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Point2DContainer                                        #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2012/03/23     #
#                                                                      #
#      Contact Address: IIS, The University of Tokyo CISS              #
#                                                                      #
#      "Large Scale Assembly, Structural Correspondence,               #
#                                     Multi Dynamics Simulator"        #
#                                                                      #
# Software Name : RevocapMesh version 1.2                              #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2008/03/14     #
#                                           T. Takeda   2008/03/14     #
#                                           K. Amemiya  2008/03/14     #
#                                                                      #
#      Contact Address: RSS21 Project, IIS, The University of Tokyo    #
#                                                                      #
#      "Innovative General-Purpose Coupled Analysis System"            #
#                                                                      #
----------------------------------------------------------------------*/
#pragma once

//
// Point2D に ID を付けてメモリに格納するコンテナ類
//

#include <map>
#include <vector>
#include <cfloat>
#include <cstddef>
#include "Geometry/kmbGeometry2D.h"
#include "Geometry/kmbBoundingBox.h"
#include "Geometry/kmbIdTypes.h"

namespace kmb{

class Point2DContainer
{
protected:
	kmb::BoundingBox2D boundBox;
public:
	Point2DContainer(void);
	virtual ~Point2DContainer(void);
	virtual bool initialize(size_t size) = 0;
	virtual nodeIdType addPoint(const double x,const double y) = 0;
	virtual nodeIdType addPoint(const kmb::Point2D& point) = 0;
	// id を指定して追加
	// 失敗したら -1 を返す
	virtual nodeIdType addPoint(const double x,const double y,const nodeIdType id) = 0;
	virtual nodeIdType addPoint(const kmb::Point2D& point,const nodeIdType id) = 0;

	virtual bool getXY(nodeIdType id,double &x,double &y) const = 0;
	virtual bool getPoint(nodeIdType id,kmb::Point2D &point) const = 0;
	virtual nodeIdType getMaxId(void) const = 0;
	virtual nodeIdType getMinId(void) const = 0;
	virtual size_t getCount(void) const = 0;
	virtual void clear(void) = 0;
	virtual bool deletePoint(nodeIdType id) = 0;
	virtual void getBoundingBox(kmb::BoundingBox2D& bound) const = 0;
	const kmb::BoundingBox2D* getBoundingBoxPtr(void) const{ return &boundBox; };
	const kmb::BoundingBox2D& getBoundingBox(void) const{ return boundBox; };
	virtual double getNearest(const kmb::Point2D* target,kmb::Point2D*& result,nodeIdType &nearestId) const = 0;
	virtual double getNearest(const double x,const double y,kmb::Point2D*& result,nodeIdType &nearestId) const = 0;
	// 線分に対して最近点を探す
	// param*t0 + (1-param)*t1 が最近点
	// d は初期距離：距離がそれ以下のを探す
	virtual double getNearestToSegment(const Point2D& t0, const Point2D& t1,kmb::Point2D*& result,nodeIdType &nearestId,double &param,double d=DBL_MAX) const = 0;

	virtual const char* getContainerType(void) const = 0;

	class _iterator
	{
	public:
		virtual ~_iterator(void){};
		virtual nodeIdType getId(void) const = 0;
		virtual bool getXY(double &x,double &y) const = 0;
		virtual bool getPoint(kmb::Point2D &point) const = 0;
		virtual bool setXY(double x,double y) const = 0;
		virtual bool setPoint(kmb::Point2D &point) const = 0;
		virtual double x(void) const = 0;
		virtual double y(void) const = 0;
		virtual _iterator* operator++(void) = 0;
		virtual _iterator* operator++(int n) = 0;
		virtual _iterator* clone(void) = 0;
	};

	class const_iterator;
	// iterator pointer の wrapper
	class iterator
	{
		friend class Point2DContainer;
		friend class Point2DContainer::const_iterator;
	private:
		_iterator* _it;
	public:
		iterator(_iterator* _i=NULL) : _it(_i){}
		iterator(const iterator& other);
		~iterator(void){ if( _it ) delete _it; }
		nodeIdType getId(void) const;
		bool getXY(double &x,double &y) const;
		bool getPoint(kmb::Point2D &point) const;
		double x(void) const;
		double y(void) const;
		bool setXY(double x,double y) const;
		bool setPoint(kmb::Point2D &point) const;
		iterator& operator=(const iterator& other);
		iterator& operator++(void); // 前置++
		iterator operator++(int n); // 後置++
		bool operator==(const iterator &other) const;
		bool operator!=(const iterator &other) const;
		// 終了判定 == end() でもいいけど、こっちの方が速い
		bool isFinished(void){ return (_it==NULL); }
	};

	class const_iterator
	{
		friend class Point2DContainer;
	private:
		_iterator* _it;
	public:
		const_iterator(_iterator* _i=NULL) : _it(_i){}
		const_iterator(const const_iterator& other);
		const_iterator(const iterator& other);
		~const_iterator(void){ if( _it ) delete _it;	}
		nodeIdType getId(void) const;
		bool getXY(double &x,double &y) const;
		bool getPoint(kmb::Point2D &point) const;
		double x(void) const;
		double y(void) const;
		const_iterator& operator=(const const_iterator& other);
		const_iterator& operator=(const iterator& other);
		const_iterator& operator++(void); // 前置++
		const_iterator operator++(int n); // 後置++
		bool operator==(const const_iterator &other) const;
		bool operator!=(const const_iterator &other) const;
		// 終了判定 == end() でもいいけど、こっちの方が速い
		bool isFinished(void){ return (_it==NULL); }
	};
private:
	static const iterator endIterator; // NULL 値
	static const const_iterator endConstIterator; // NULL 値
public:
	virtual iterator begin(void) = 0;
	virtual const_iterator begin(void) const = 0;
	virtual iterator find(kmb::nodeIdType nodeId) = 0;
	virtual const_iterator find(kmb::nodeIdType nodeId) const = 0;
	// 値返しにすると毎回コンストラクタが呼ばれるのでよくない
	const iterator& end(void){
		return endIterator;
	};
	const const_iterator& end(void) const{
		return endConstIterator;
	};

	// コピーメソッド container から this へ
	void copyPreservingId(Point2DContainer& container);
	void copyWithoutPreservingId(Point2DContainer& container);
	// 点の二重化
	nodeIdType duplicatePoint(nodeIdType nodeId);

	// 変換系
	void convertAllNodes( kmb::Matrix3x3& mat );
	void translateAllNodes( double x,double y );
	void scaleAllNodes( double r );
	// 更新系
	bool updateCoordinate( kmb::nodeIdType nodeId, double x, double y );

public:
	static bool copy(const kmb::Point2DContainer* org, kmb::Point2DContainer* points);
	static kmb::Point2DContainer* createContainer( const char* containerType );
};

class Point2DContainerVect : public Point2DContainer
{
private:
	std::vector< kmb::Point2D* > points;
public:
	static const char* CONTAINER_TYPE;
	Point2DContainerVect(void);
	virtual ~Point2DContainerVect(void);
	virtual bool initialize(size_t size);
	virtual nodeIdType addPoint(const double x,const double y);
	virtual nodeIdType addPoint(const kmb::Point2D& point);
	virtual nodeIdType addPoint(const double x,const double y,const nodeIdType id);
	virtual nodeIdType addPoint(const kmb::Point2D& point,const nodeIdType id);
	virtual bool getXY(nodeIdType id,double &x,double &y) const;
	virtual bool getPoint(nodeIdType id,kmb::Point2D &point) const;
	virtual nodeIdType getMaxId(void) const;
	virtual nodeIdType getMinId(void) const;
	virtual size_t getCount(void) const;
	virtual void clear(void);
	virtual bool deletePoint(nodeIdType id);
	virtual void getBoundingBox(kmb::BoundingBox2D& bound) const;
	virtual double getNearest(const kmb::Point2D* target,kmb::Point2D*& result,nodeIdType &nearestId) const;
	virtual double getNearest(const double x,const double y,kmb::Point2D*& result,nodeIdType &nearestId) const;
	virtual double getNearestToSegment(const Point2D& t0, const Point2D& t1,kmb::Point2D*& result,nodeIdType &nearestId,double &param,double d=DBL_MAX) const;

	virtual const char* getContainerType(void) const{
		return Point2DContainerVect::CONTAINER_TYPE;
	}

	class _iteratorVect : public Point2DContainer::_iterator
	{
		friend class Point2DContainerVect;
	public:
		virtual nodeIdType getId(void) const;
		virtual bool getXY(double &x,double &y) const;
		virtual bool getPoint(kmb::Point2D &point) const;
		virtual double x(void) const;
		virtual double y(void) const;
		virtual bool setXY(double x,double y) const;
		virtual bool setPoint(kmb::Point2D &point) const;
		virtual Point2DContainer::_iterator* operator++(void);
		virtual Point2DContainer::_iterator* operator++(int n);
		virtual Point2DContainer::_iterator* clone(void);
	private:
		_iteratorVect(void){};
		virtual ~_iteratorVect(void){};
		int index;
		const Point2DContainerVect* vect;
	};

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::nodeIdType nodeId);
	virtual const_iterator find(kmb::nodeIdType nodeId) const;

};

class Point2DContainerMap : public Point2DContainer
{
public:
	enum idContinuityType{
		UNKNOWN        = -1,// よくわからない
		ZERO_LEADING   = 0, // 0 から始まる
		ONE_LEADING    = 1, // 1 から始まる
		OTHER_LEADING  = 2, // それ以外の数字から連続している
		NOT_CONTINUOUS = 3  // 連続していない
	};
private:
	nodeIdType minId;
	nodeIdType maxId;
	std::map< kmb::nodeIdType, kmb::Point2D* > points;
	idContinuityType idContinuity;
public:
	static const char* CONTAINER_TYPE;
	Point2DContainerMap(void);
	virtual ~Point2DContainerMap(void);
	virtual bool initialize(size_t size);
	virtual nodeIdType addPoint(const double x,const double y);
	virtual nodeIdType addPoint(const kmb::Point2D& point);
	virtual nodeIdType addPoint(const double x,const double y,const nodeIdType id);
	virtual nodeIdType addPoint(const kmb::Point2D& point,const nodeIdType id);
	virtual bool getXY(nodeIdType id,double &x,double &y) const;
	virtual bool getPoint(nodeIdType id,kmb::Point2D &point) const;
	virtual nodeIdType getMaxId(void) const;
	virtual nodeIdType getMinId(void) const;
	virtual size_t getCount(void) const;
	virtual void clear(void);
	virtual bool deletePoint(nodeIdType id);
	virtual kmb::Point2D* erasePoint(nodeIdType id);
	virtual void getBoundingBox(kmb::BoundingBox2D& bound) const;
	virtual double getNearest(const kmb::Point2D* target,kmb::Point2D*& result,nodeIdType &nearestId) const;
	virtual double getNearest(const double x,const double y,kmb::Point2D*& result,nodeIdType &nearestId) const;
	virtual double getNearestToSegment(const Point2D& t0, const Point2D& t1,kmb::Point2D*& result,nodeIdType &nearestId,double &param,double d=DBL_MAX) const;

	virtual const char* getContainerType(void) const{
		return Point2DContainerMap::CONTAINER_TYPE;
	}

	class _iteratorMap : public Point2DContainer::_iterator
	{
		friend class Point2DContainerMap;
	public:
		virtual nodeIdType getId(void) const;
		virtual bool getXY(double &x,double &y) const;
		virtual bool getPoint(kmb::Point2D &point) const;
		virtual double x(void) const;
		virtual double y(void) const;
		virtual bool setXY(double x,double y) const;
		virtual bool setPoint(kmb::Point2D &point) const;
		virtual Point2DContainer::_iterator* operator++(void);
		virtual Point2DContainer::_iterator* operator++(int n);
		virtual Point2DContainer::_iterator* clone(void);
	private:
		_iteratorMap(void){};
		virtual ~_iteratorMap(void){};
		std::map< kmb::nodeIdType, kmb::Point2D* >::const_iterator pointIter;
		std::map< kmb::nodeIdType, kmb::Point2D* >::const_iterator endIterator;
	};

	virtual iterator begin(void);
	virtual const_iterator begin(void) const;
	virtual iterator find(kmb::nodeIdType nodeId);
	virtual const_iterator find(kmb::nodeIdType nodeId) const;

// 以下は Map モードの時のみ定義された関数
	bool	replaceId(nodeIdType oldid,nodeIdType newid);
	bool	deleteId(nodeIdType id);
	void	updateMinMaxId(void);

	idContinuityType getIdContinuity(void) const{ return idContinuity; }

	void idDefragment(nodeIdType initId, std::map< nodeIdType, nodeIdType >& idmap);
};

// direct access
class Point2DContainerDirectAccessable : public Point2DContainer{
public:
	virtual double operator()(kmb::nodeIdType nodeId,int index) const = 0;
	virtual double& operator()(kmb::nodeIdType nodeId,int index) = 0;
	// nodeId までの値の設定を行って、iterator で参照できるようにする
	// 注：size は nodeId + 1 である
	virtual void commit(kmb::nodeIdType nodeId) = 0;
};

}

