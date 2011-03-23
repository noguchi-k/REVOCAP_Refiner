/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_Refiner version 1.0                          #
# Sample Program MultiByType                                           #
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
/*
 *
 * サンプル実行例＆テスト用プログラム
 * 複数種類の要素細分チェック用
 *
 */

#ifdef _CONSOLE

#include "rcapRefiner.h"
#include "rcapRefinerMacros.h"
#include <stdio.h>
#include <stdlib.h>  /* for calloc, free */
#include <assert.h>

int main(void)
{
	/* 六面体の上に三角柱を乗せる */
	float64_t coords[30] = {
		0.0, 0.0, -1.0,
		1.0, 0.0, -1.0,
		1.0, 1.0, -1.0,
		0.0, 1.0, -1.0,
		0.0, 0.0,  0.0,
		1.0, 0.0,  0.0,
		1.0, 1.0,  0.0,
		0.0, 1.0,  0.0,
		0.5, 0.0,  1.0,
		0.5, 1.0,  1.0,
	};
	size_t refineNodeCount = 0;
	float64_t* resultCoords = NULL;
	int32_t hexas[8] = {
		1, 2, 3, 4, 5, 6, 7, 8,
	};
	int32_t wedges[6] = {
		5, 9, 6, 8, 10, 7,
	};
	/* 細分後の六面体の節点配列：出力は1*8=8個 */
	int32_t* refineHexas = NULL;
	/* 細分後の三角柱の節点配列：出力は1*8=8個 */
	int32_t* refineWedges = NULL;
	/* 細分する要素の型(定数値) */
	int8_t etype = RCAP_HEXAHEDRON;
	int32_t nodeOffset = 1;
	int32_t elementOffset = 1;
	/* 初期節点の個数 */
	size_t nodeCount = 10;
	/* 初期要素の個数 */
	size_t elementCount = 2;
	/* 細分後の要素の個数 */
	size_t refineHexaCount = 0;
	size_t refineWedgeCount = 0;
	size_t refineElementCount = 0;

	/* 境界条件（節点グループ） */
	int32_t ng0[5] = {1,2,5,6,9};
	int32_t* result_ng0 = NULL;
	size_t ng0Count = 5;

	/* カウンタ */
	int32_t i,j;

	/* 節点番号のオフセット値を与える */
	rcapInitRefiner( nodeOffset, elementOffset );
	/* 座標値を Refiner に与える */
	rcapSetNode64( nodeCount, coords, NULL, NULL );

	printf("----- Original Model -----\n");
	/* 細分前の節点数 */
	nodeCount = rcapGetNodeCount();
	assert( nodeCount == 10 );
	printf("Node : Count = %"PRIsz"\n", nodeCount );
	for(i=0;(size_t)i<nodeCount;++i){
		printf("%d : %f, %f, %f\n", i+nodeOffset, coords[3*i], coords[3*i+1], coords[3*i+2] );
	}
	/* 細分前の要素数 */
	assert( elementCount == 2 );
	printf("Element : Count = %"PRIsz"\n", elementCount );
	j = 0;
	etype = RCAP_HEXAHEDRON;
	elementCount = 1;
	for(i=0;(size_t)i<elementCount;++i){
		printf("%d : (%d) %d, %d, %d, %d, %d, %d, %d, %d\n", j+elementOffset, etype,
			hexas[8*i], hexas[8*i+1], hexas[8*i+2], hexas[8*i+3],
			hexas[8*i+4], hexas[8*i+5], hexas[8*i+6], hexas[8*i+7] );
		j++;
	}
	etype = RCAP_WEDGE;
	elementCount = 1;
	for(i=0;(size_t)i<elementCount;++i){
		printf("%d : (%d) %d, %d, %d, %d, %d, %d\n", j+elementOffset, etype,
			wedges[6*i], wedges[6*i+1], wedges[6*i+2],
			wedges[6*i+3], wedges[6*i+4], wedges[6*i+5] );
		j++;
	}

	/* 節点グループの登録 */
	rcapAppendNodeGroup("ng0",ng0Count,ng0);
	ng0Count = rcapGetNodeGroupCount("ng0");
	printf("Node Group : Count = %"PRIsz"\n", ng0Count );
	assert( ng0Count == 5 );
	for(i=0;(size_t)i<ng0Count;++i){
		printf("%d\n", ng0[i]);
	}

	printf("----- Refined Model -----\n");
	/* 要素の細分 */
	etype = RCAP_HEXAHEDRON;
	elementCount = 1;
	refineHexaCount = rcapRefineElement( elementCount, etype, hexas, NULL );
	refineHexas = (int32_t*)calloc( 8*refineHexaCount, sizeof(int32_t) );
	etype = RCAP_WEDGE;
	elementCount = 1;
	refineWedgeCount = rcapRefineElement( elementCount, etype, wedges, NULL );
	refineWedges = (int32_t*)calloc( 6*refineWedgeCount, sizeof(int32_t) );
	refineElementCount = 0;
	etype = RCAP_HEXAHEDRON;
	elementCount = 1;
	refineHexaCount = rcapRefineElement( elementCount, etype, hexas, refineHexas );
	refineElementCount += refineHexaCount;
	etype = RCAP_WEDGE;
	elementCount = 1;
	refineWedgeCount = rcapRefineElement( elementCount, etype, wedges, refineWedges );
	refineElementCount += refineWedgeCount;
	rcapCommit();

	/* 細分後の節点 */
	refineNodeCount = rcapGetNodeCount();
	printf("Node : Count = %"PRIsz"\n", refineNodeCount );
	resultCoords = (float64_t*)calloc( 3*refineNodeCount, sizeof(float64_t) );
	rcapGetNodeSeq64( refineNodeCount, nodeOffset, resultCoords );
	for(j=0;(size_t)j<refineNodeCount;++j){
		printf("%d : %f, %f, %f\n", j+nodeOffset, resultCoords[3*j], resultCoords[3*j+1], resultCoords[3*j+2] );
	}
	free( resultCoords );

	/* 細分後の要素 */
	printf("Element : Count = %"PRIsz"\n", refineElementCount );
	j = 0;
	etype = RCAP_HEXAHEDRON;
	for(i=0;(size_t)i<refineHexaCount;++i){
		printf("%d : (%d) %d, %d, %d, %d, %d, %d, %d, %d\n", j+elementOffset, etype,
			refineHexas[8*i], refineHexas[8*i+1], refineHexas[8*i+2], refineHexas[8*i+3],
			refineHexas[8*i+4], refineHexas[8*i+5], refineHexas[8*i+6], refineHexas[8*i+7] );
		j++;
	}
	etype = RCAP_WEDGE;
	for(i=0;(size_t)i<refineWedgeCount;++i){
		printf("%d : (%d) %d, %d, %d, %d, %d, %d\n", j+elementOffset, etype,
			refineWedges[6*i], refineWedges[6*i+1], refineWedges[6*i+2],
			refineWedges[6*i+3], refineWedges[6*i+4], refineWedges[6*i+5] );
		j++;
	}

	/* 細分後の節点グループ */
	ng0Count = rcapGetNodeGroupCount("ng0");
	result_ng0 = (int32_t*)calloc( ng0Count, sizeof(int32_t) );
	printf("Node Group : Count = %"PRIsz"\n", ng0Count );
	rcapGetNodeGroup("ng0",ng0Count,result_ng0);
	for(j=0;(size_t)j<ng0Count;++j){
		printf("%d\n", result_ng0[j]);
	}
	free( result_ng0 );

	free( refineHexas );
	free( refineWedges );

	rcapTermRefiner();
	return 0;
}

#endif
