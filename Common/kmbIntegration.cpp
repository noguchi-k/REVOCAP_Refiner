/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.6                          #
# Class Name : Integration                                             #
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
#include "Common/kmbIntegration.h"

double
kmb::Integration::Gauss_Point2[] = {
	-0.57735026918962573105886804114561527967453,
	0.57735026918962573105886804114561527967453
};

double
kmb::Integration::Gauss_Weight2[] = {
	1.0,
	1.0
};

double
kmb::Integration::Gauss_Point3[] = {
	-0.774596669241483404277914814883843064308167,
	0.0,
	0.774596669241483404277914814883843064308167
};

double
kmb::Integration::Gauss_Weight3[] = {
	0.888888888888888888888888888888888888888889,
	0.555555555555555555555555555555555555555556,
	0.888888888888888888888888888888888888888889
};

double
kmb::Integration::Gauss_Point4[] = {
	-0.861136311594052572537805190222570672631264,
	-0.339981043584856312822495283398893661797047,
	0.339981043584856312822495283398893661797047,
	0.861136311594052572537805190222570672631264
};

double
kmb::Integration::Gauss_Weight4[] = {
	0.347854845137453849712727560472558252513409,
	0.652145154862546205798423670785268768668175,
	0.652145154862546205798423670785268768668175,
	0.347854845137453849712727560472558252513409
};
