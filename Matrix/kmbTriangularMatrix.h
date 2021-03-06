/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : UpperTriangularMatrix, LowerTriangularMatrix            #
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

#include "Matrix/kmbMatrix.h"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4100) // 使わない引数があっても警告を出さない for VC
#endif

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:869) // 使わない引数があっても警告を出さない for intel
#endif

namespace kmb{

class ColumnVector;

class UpperTriangularMatrix : public SquareMatrix
{
public:
	UpperTriangularMatrix(int size) : SquareMatrix(size){};
	virtual ~UpperTriangularMatrix(void){};
	virtual bool solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const;
	virtual double determinant(void) const;
	// 何もしないようにする（UpperTriangularでなくなってしまうため）
	virtual bool transpose(void);
	virtual bool row_exchange(int i0,int i1);
	virtual bool row_transf(int i0,int i1,double r);
	// 上三角成分との内積（対角成分を無視）
	virtual double rowProduct_upper(int i, const kmb::ColumnVector &vec) const;
	// 対角成分
	virtual double getDiagonal(int i) const;
};

// 対角成分が 1.0 であることを保証されている上三角行列
class UpperTriangularDiagonalUnitMatrix : public UpperTriangularMatrix
{
public:
	UpperTriangularDiagonalUnitMatrix(int size) : UpperTriangularMatrix(size){};
	virtual ~UpperTriangularDiagonalUnitMatrix(void){};
	virtual bool solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const;
	// 対角成分
	virtual double getDiagonal(int i) const;
};

class UpperTriangularMatrix_Wrapper : public UpperTriangularMatrix
{
private:
	const kmb::SquareMatrix* matrix;
public:
	UpperTriangularMatrix_Wrapper(const kmb::SquareMatrix* mtx);
	virtual ~UpperTriangularMatrix_Wrapper(void);
	virtual int init(int rowSize, int colSize);
	virtual const char* getContainerType(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool add(int i,int j,double val);
};

class UpperTriangularDiagonalUnitMatrix_Wrapper : public UpperTriangularDiagonalUnitMatrix
{
private:
	const kmb::SquareMatrix* matrix;
public:
	UpperTriangularDiagonalUnitMatrix_Wrapper(const kmb::SquareMatrix* mtx);
	virtual ~UpperTriangularDiagonalUnitMatrix_Wrapper(void);
	virtual int init(int rowSize, int colSize);
	virtual const char* getContainerType(void) const;
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool add(int i,int j,double val);
};

class LowerTriangularMatrix : public SquareMatrix
{
public:
	LowerTriangularMatrix(int size) : SquareMatrix(size){};
	virtual ~LowerTriangularMatrix(void){};
	virtual bool solve(const kmb::ColumnVector &b, kmb::ColumnVector &x) const;
	virtual double determinant(void) const;
	// 何もしないようにする（LowerTriangularでなくなってしまうため）
	virtual bool transpose(void);
	virtual bool row_exchange(int i0,int i1);
	virtual bool row_transf(int i0,int i1,double r);
	// 下三角成分との内積（対角成分を無視）
	virtual double rowProduct_lower(int i, const kmb::ColumnVector &vec) const;
	// 対角成分
	virtual double getDiagonal(int i) const;
};

class LowerTriangularMatrix_Wrapper : public LowerTriangularMatrix
{
private:
	const kmb::SquareMatrix* matrix;
public:
	LowerTriangularMatrix_Wrapper(const kmb::SquareMatrix* mtx);
	virtual ~LowerTriangularMatrix_Wrapper(void);
	virtual const char* getContainerType(void) const;
	virtual int init(int rowSize, int colSize);
	virtual double get(int i,int j) const;
	virtual bool set(int i,int j,double val);
	virtual bool add(int i,int j,double val);
};

}

#if defined _MSC_VER || defined __INTEL_COMPILER
#pragma warning(pop)
#endif
