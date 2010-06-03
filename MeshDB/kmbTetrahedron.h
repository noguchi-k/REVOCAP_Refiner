/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Tetrahedron                                             #
#                                                                      #
#                                Written by                            #
#                                           K. Tokunaga 2010/03/23     #
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
#include "MeshDB/kmbElement.h"
#include "Geometry/kmb_Point3DContainer.h"

namespace kmb{

class Tetrahedron : public Element
{
public:
	static const int nodeCount;
	Tetrahedron();
	Tetrahedron(kmb::nodeIdType n0, kmb::nodeIdType n1, kmb::nodeIdType n2, kmb::nodeIdType n3);
	Tetrahedron(kmb::nodeIdType *ary);
	virtual ~Tetrahedron(void);










	static bool isEquivalent(int index[4]);

	static void shapeFunction(double s,double t,double u,double* coeff);
	static bool getNaturalCoordinates(const double physicalCoords[3],const kmb::Point3D* points,double naturalCoords[3],double margin=1.0);
	static bool getPhysicalCoordinates(const double naturalCoords[3],const kmb::Point3D* points,double physicalCoords[3]);

	static const int	connectionTable[4][4];
	static const int	faceTable[4][4];
	static const int	edgeTable[6][2];
};

}
