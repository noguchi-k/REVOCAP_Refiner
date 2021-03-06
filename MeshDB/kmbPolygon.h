/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Polygon                                                 #
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
//
// Segment の集合としての多角形を扱う
// 対角線を与えて分割することなどを行う
//

#pragma once

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbElementContainer.h"
#include "MeshDB/kmbElementContainerMap.h"
#include "MeshDB/kmbNodeNeighborInfo.h"

#include "Geometry/kmbPoint2DContainer.h"
#include "Geometry/kmbPoint3DContainer.h"
#include "Geometry/kmbPolygon2D.h"

#include <vector>

namespace kmb{

class Segment;

class Polygon
{
private:
	kmb::NodeNeighborInfo neighborInfo;
	kmb::ElementContainer* edges;
	bool edgeDeletable;
public:
	Polygon(void);
	virtual ~Polygon(void);
	size_t getSize(void) const;
	bool getCenter(const kmb::Point2DContainer* points,kmb::Point2D &center) const;
	bool getCenter(const kmb::Point3DContainer* points,kmb::Point3D &center) const;
	kmb::nodeIdType getNearestNode(const kmb::Point2DContainer* points,kmb::Point2D &point) const;
	kmb::nodeIdType getNearestNode(const kmb::Point3DContainer* points,kmb::Point3D &point) const;

	bool include(kmb::nodeIdType nodeId) const;
	kmb::nodeIdType getNodeId(kmb::nodeIdType nodeId,int offset=0) const;

	// setEdges は clone = true のときすべての要素の複製を作って登録する
	void setEdges(kmb::ElementContainer *edges,bool clone=true);
	// const の時は無条件で複製を作る
	void setEdges(const kmb::ElementContainer *edges);
	// FACE GROUP から登録する
	void setEdgesByFaceGroup(const kmb::DataBindings* facegroup,const kmb::ElementContainer* patches);
	const kmb::ElementContainer* getEdges(void) const;

	bool isAdjacent(kmb::nodeIdType n0,kmb::nodeIdType n1) const;

	void addSegment( kmb::nodeIdType n0, kmb::nodeIdType n1 );
	bool deleteSegment( kmb::nodeIdType n0, kmb::nodeIdType n1 );

	void dividePolygonsByDiagonals
		( const kmb::Point2DContainer* points,
		  std::vector< std::pair<kmb::nodeIdType, kmb::nodeIdType> > &diagonals,
		  std::vector< kmb::Polygon* > &polygons);
	// 閉じているかを確認
	bool isClosed(void) const;
	// 端点の取得
	// 連結なら true を返す。非連結なら false を返す。
	// loop のときは initial と end はともに kmb::nullNodeId
	bool getEndPoints( kmb::nodeIdType &initial, kmb::nodeIdType &end ) const;
	// 二重点があるかどうか
	kmb::nodeIdType hasDoubleNode(void) const;
private:

	// prevId, nodeId, nodePairs が与えられた時に
	// 次の節点 nextId を角度を考慮して求めて、
	// deleteflag = true のときは その辺 [nodeId, nextId] を nodePair から削除する
	static kmb::nodeIdType getNextNode(
			kmb::nodeIdType prevId,
			kmb::nodeIdType nodeId,
			const kmb::Point2DContainer* points,
			std::multimap< kmb::nodeIdType, kmb::nodeIdType > &nodePairs,
			bool deleteflag);

public:
	// topology だけから判断する
	static kmb::nodeIdType getNextNode
		( const kmb::ElementContainer* edges, const kmb::NodeNeighborInfo& neighborInfo, kmb::nodeIdType nodeID, bool order=true );
	static kmb::elementIdType getNextElement
		( const kmb::ElementContainer* edges, const kmb::NodeNeighborInfo& neighborInfo, kmb::elementIdType elementID, bool order=true );
	// order = true 始点が nodeID の要素を検索
	//         false 終点が nodeID の要素を検索
	static kmb::elementIdType getElementByNode
		( kmb::ElementContainer* edges, kmb::NodeNeighborInfo& neighborInfo, kmb::nodeIdType nodeID, bool order=true );
};

}
