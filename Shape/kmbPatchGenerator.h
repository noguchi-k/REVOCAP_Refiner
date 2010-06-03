/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : PatchGenerator                                          #
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

namespace kmb{

class MeshData;
class ShapeData;

class PatchGenerator
{
private:
	double deflection;
	double incremental;
	double tolerance;
public:
	PatchGenerator(void);
	virtual ~PatchGenerator(void);
	/*
	 * mesh には生成した CAD 面の HashCode を BODYVARIABLE の Data 名 FaceHashCode で与える
	 */
	bool execute(kmb::ShapeData& shape,kmb::MeshData& mesh);
	/*
	 * 三角形化する時の辺の長さの最小値を与える
	 * BRepTools::Triangulation
	 */
	void setDeflection(double d);
	double getDeflection(void);
	/*
	 * BRepMesh::Mesh
	 */
	void setIncremental(double d);
	double getIncremental(void);
	/*
	 * 頂点を同一視する閾値
	 */
	void setTolerance(double d);
	double getTolerance(void);
};

}

#endif
