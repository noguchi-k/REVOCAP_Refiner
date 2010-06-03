/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : ShapeData                                               #
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

#ifdef OPENCASCADE

#include <vector>

class TopoDS_Shape;

namespace kmb{

class Surface3D;
class MeshData;

class ShapeData
{
private:
	TopoDS_Shape* topo_shape;
public:
	ShapeData(void);
	virtual ~ShapeData(void);
	TopoDS_Shape& getShape(void) const;
	void test(void) const;
	void convertToBezier(void);
	void fixShape(double precision,double tolerance);
	void dropSmallEdge(double precision,double tolerance);
	bool isClosed(void) const;
	bool isValid(void) const;


	int getSurfaces( std::vector<kmb::Surface3D*> &surfaces) const;
	int saveToRNF(const char* filename) const;
};

}

#endif
