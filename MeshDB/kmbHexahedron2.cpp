/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : Hexahedron2                                             #
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
#include "MeshDB/kmbHexahedron2.h"
#include "MeshDB/kmbTetrahedron.h"
#include "MeshDB/kmbWedge.h"
#include "MeshDB/kmbHexahedron.h"
#include "MeshDB/kmbMeshDB.h"
#include "MeshDB/kmbElementRelation.h"

/********************************************************************************
=begin

=== 2次六面体要素 (HEXAHEDRON2)

接続行列

	{ 0, 1, 0, 1, 1, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0},
	{ 1, 0, 1, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
	{ 0, 1, 0, 1, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0},
	{ 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0 ,0, 2},
	{ 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0},
	{ 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0},
	{ 0, 0, 1, 0, 0, 1, 0, 1, 0 ,0 ,0 ,0 ,0, 2, 2, 0, 0, 0, 2, 0},
	{ 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2},
	{ 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},

面

	{ 3, 2, 1, 0,10, 9, 8,11},
	{ 4, 5, 6, 7,12,13,14,15},
	{ 1, 5, 4, 0,17,12,16, 8},
	{ 1, 2, 6, 5, 9,18,13,17},
	{ 3, 7, 6, 2,19,14,18,10},
	{ 4, 7, 3, 0,15,19,11,16}

辺

	{ 0, 1, 8},
	{ 1, 2, 9},
	{ 2, 3, 10},
	{ 0, 3, 11},
	{ 4, 5, 12},
	{ 5, 6, 13},
	{ 6, 7, 14},
	{ 4, 7, 15},
	{ 0, 4, 16},
	{ 1, 5, 17},
	{ 2, 6, 18},
	{ 3, 7, 19}

=end

旧バージョン
	{ 3, 2, 1, 0,10, 9, 8,11},
	{ 4, 5, 6, 7,12,13,14,15},
	{ 0, 1, 5, 4, 8,17,12,16},
	{ 1, 2, 6, 5, 9,18,13,17},
	{ 2, 3, 7, 6,10,19,14,18},
	{ 0, 4, 7, 3,16,15,19,11}

形状関数：
0 : 1/8(1-s)(1-t)(1-u)(-2-s-t-u) => (s,t,u) = (-1,-1,-1)
1 : 1/8(1+s)(1-t)(1-u)(-2+s-t-u) => (s,t,u) = ( 1,-1,-1)
2 : 1/8(1+s)(1+t)(1-u)(-2+s+t-u) => (s,t,u) = ( 1, 1,-1)
3 : 1/8(1-s)(1+t)(1-u)(-2-s+t-u) => (s,t,u) = (-1, 1,-1)
4 : 1/8(1-s)(1-t)(1+u)(-2-s-t+u) => (s,t,u) = (-1,-1, 1)
5 : 1/8(1+s)(1-t)(1+u)(-2+s-t+u) => (s,t,u) = ( 1,-1, 1)
6 : 1/8(1+s)(1+t)(1+u)(-2+s+t+u) => (s,t,u) = ( 1, 1, 1)
7 : 1/8(1-s)(1+t)(1+u)(-2-s+t+u) => (s,t,u) = (-1, 1, 1)
8 : 1/4(1-s*s)(1-t)(1-u)         => (s,t,u) = ( 0,-1,-1)
9 : 1/4(1+s)(1-t*t)(1-u)         => (s,t,u) = ( 1, 0,-1)
10: 1/4(1-s*s)(1+t)(1-u)         => (s,t,u) = ( 0, 1,-1)
11: 1/4(1-s)(1-t*t)(1-u)         => (s,t,u) = (-1, 0,-1)
12: 1/4(1-s*s)(1-t)(1+u)         => (s,t,u) = ( 0,-1, 1)
13: 1/4(1+s)(1-t*t)(1+u)         => (s,t,u) = ( 1, 0, 1)
14: 1/4(1-s*s)(1+t)(1+u)         => (s,t,u) = ( 0, 1, 1)
15: 1/4(1-s)(1-t*t)(1+u)         => (s,t,u) = (-1, 0, 1)
16: 1/4(1-s)(1-t)(1-u*u)         => (s,t,u) = (-1,-1, 0)
17: 1/4(1+s)(1-t)(1-u*u)         => (s,t,u) = ( 1,-1, 0)
18: 1/4(1+s)(1+t)(1-u*u)         => (s,t,u) = ( 1, 1, 0)
19: 1/4(1-s)(1+t)(1-u*u)         => (s,t,u) = (-1, 1, 0)

*********************************************************************************/
const int kmb::Hexahedron2::nodeCount = 20;


const int kmb::Hexahedron2::connectionTable[20][20] =
{
	{ 0, 1, 0, 1, 1, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 2, 0, 0, 0},
	{ 1, 0, 1, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0},
	{ 0, 1, 0, 1, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0},
	{ 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0 ,0, 2},
	{ 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0},
	{ 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0},
	{ 0, 0, 1, 0, 0, 1, 0, 1, 0 ,0 ,0 ,0 ,0, 2, 2, 0, 0, 0, 2, 0},
	{ 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 2},
	{ 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
	{ 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0 ,0, 0, 0, 0, 0, 0, 0, 0},
};




const int kmb::Hexahedron2::faceTable[6][8] =
{
	{ 3, 2, 1, 0,10, 9, 8,11},
	{ 4, 5, 6, 7,12,13,14,15},
	{ 1, 5, 4, 0,17,12,16, 8},
	{ 1, 2, 6, 5, 9,18,13,17},
	{ 3, 7, 6, 2,19,14,18,10},
	{ 4, 7, 3, 0,15,19,11,16}
};

const int kmb::Hexahedron2::edgeTable[12][3] =
{
	{ 0, 1, 8},
	{ 1, 2, 9},
	{ 2, 3, 10},
	{ 0, 3, 11},
	{ 4, 5, 12},
	{ 5, 6, 13},
	{ 6, 7, 14},
	{ 4, 7, 15},
	{ 0, 4, 16},
	{ 1, 5, 17},
	{ 2, 6, 18},
	{ 3, 7, 19}
};

kmb::Hexahedron2::Hexahedron2(void)
: kmb::Element(kmb::HEXAHEDRON2)
{
	cell = new kmb::nodeIdType[20];
}

kmb::Hexahedron2::Hexahedron2(kmb::nodeIdType *ary)
: kmb::Element(kmb::HEXAHEDRON2)
{
	cell = ary;
}

kmb::Hexahedron2::~Hexahedron2(void)
{
}

void
kmb::Hexahedron2::shapeFunction(double s,double t,double u,double* coeff)
{
	coeff[0] = 0.125*(1.0-s)*(1.0-t)*(1.0-u)*(-2.0-s-t-u);
	coeff[1] = 0.125*(1.0+s)*(1.0-t)*(1.0-u)*(-2.0+s-t-u);
	coeff[2] = 0.125*(1.0+s)*(1.0+t)*(1.0-u)*(-2.0+s+t-u);
	coeff[3] = 0.125*(1.0-s)*(1.0+t)*(1.0-u)*(-2.0-s+t-u);
	coeff[4] = 0.125*(1.0-s)*(1.0-t)*(1.0+u)*(-2.0-s-t+u);
	coeff[5] = 0.125*(1.0+s)*(1.0-t)*(1.0+u)*(-2.0+s-t+u);
	coeff[6] = 0.125*(1.0+s)*(1.0+t)*(1.0+u)*(-2.0+s+t+u);
	coeff[7] = 0.125*(1.0-s)*(1.0+t)*(1.0+u)*(-2.0-s+t+u);
	coeff[8] = 0.25*(1.0-s*s)*(1.0-t)*(1.0-u);
	coeff[9] = 0.25*(1.0+s)*(1.0-t*t)*(1.0-u);
	coeff[10] = 0.25*(1.0-s*s)*(1.0+t)*(1.0-u);
	coeff[11] = 0.25*(1.0-s)*(1.0-t*t)*(1.0-u);
	coeff[12] = 0.25*(1.0-s*s)*(1.0-t)*(1.0+u);
	coeff[13] = 0.25*(1.0+s)*(1.0-t*t)*(1.0+u);
	coeff[14] = 0.25*(1.0-s*s)*(1.0+t)*(1.0+u);
	coeff[15] = 0.25*(1.0-s)*(1.0-t*t)*(1.0+u);
	coeff[16] = 0.25*(1.0-s)*(1.0-t)*(1.0-u*u);
	coeff[17] = 0.25*(1.0+s)*(1.0-t)*(1.0-u*u);
	coeff[18] = 0.25*(1.0+s)*(1.0+t)*(1.0-u*u);
	coeff[19] = 0.25*(1.0-s)*(1.0+t)*(1.0-u*u);
}












































































































































































































































































































































































































































































