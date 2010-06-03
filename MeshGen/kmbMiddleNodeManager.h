/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : MiddleNodeManager                                       #
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















#pragma once

#include "MeshDB/kmbTypes.h"
#include "MeshDB/kmbElement.h"
#include <map>

namespace kmb{

class Point3DContainer;
class ElementBase;

class MiddleNodeManager
{
protected:
	struct originalPair{
		kmb::nodeIdType n0;
		kmb::nodeIdType n1;

		kmb::elementIdType elementId;
	};

	typedef std::pair< kmb::nodeIdType, kmb::nodeIdType > NodePair;
	Point3DContainer* points;
	std::map< NodePair, kmb::nodeIdType > middlePoints;
	std::map< kmb::nodeIdType, kmb::MiddleNodeManager::originalPair > originalPoints;
protected:

	void appendMiddleNode( kmb::nodeIdType middle, kmb::nodeIdType org0, kmb::nodeIdType org1, kmb::elementIdType elemId=kmb::Element::nullElementId);
public:
	MiddleNodeManager(void);
	virtual ~MiddleNodeManager(void);
	void clear(void);

	void setNodeContainer(Point3DContainer* points);


	kmb::nodeIdType getDividedNode(kmb::nodeIdType a,kmb::nodeIdType b,kmb::elementIdType elemId=kmb::Element::nullElementId);


	kmb::nodeIdType getCenterNode(kmb::ElementBase &elem,kmb::elementIdType elementId);
	kmb::nodeIdType getCenterNode(kmb::ElementBase &elem,int faceIndex,kmb::elementIdType elementId);


	kmb::nodeIdType isDivided(kmb::nodeIdType a,kmb::nodeIdType b);

	void setDividedNode(kmb::nodeIdType middle, kmb::nodeIdType a,kmb::nodeIdType b);

	kmb::elementIdType getOriginalNode(kmb::nodeIdType centerId,kmb::nodeIdType &a,kmb::nodeIdType &b) const;
};

}
