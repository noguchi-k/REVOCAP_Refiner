/*----------------------------------------------------------------------
#                                                                      #
# Software Name : REVOCAP_PrePost version 1.4                          #
# Class Name : RevocapCouplerIO                                        #
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

#include "RevocapIO/kmbMicroAVSIO.h"
#include "MeshDB/kmbElement.h"
#include "MeshDB/kmbMeshData.h"
#include "MeshDB/kmbDataBindings.h"
#include "MeshDB/kmbPhysicalValue.h"

#include <fstream>
#include <sstream>
#include <iostream>

kmb::MicroAVSIO::MicroAVSIO(void)
: dtype(UNDEFINED)
, nodeOffset(1)
, elementOffset(1)
, asVector3(false)
, version(-1)
, nodeCount(0)
, elementCount(0)
, nodeDimCount(0)
, elementDimCount(0)
{
}

kmb::MicroAVSIO::~MicroAVSIO(void)
{
}

int
kmb::MicroAVSIO::getVersion(std::ifstream &input)
{
	std::string line;
	if( !input.eof() ){
		std::getline( input, line );
		if( line.find( "#" ) != std::string::npos ){
			return 9;
		}else if( line.find_first_of("0123456789") != std::string::npos ){
			if( line.find_first_not_of("0123456789") == std::string::npos ){

				return 9;
			}else{
				return 8;
			}
		}
	}
	return -1;
}

void
kmb::MicroAVSIO::setReadAsVector3( bool flag )
{
	this->asVector3 = flag;
}

int
kmb::MicroAVSIO::readHeader(std::ifstream &input)
{
	if( version == 9 ){
		int step = -1;
		std::string line;
		while( !input.eof() ){
			std::getline( input, line );
			if( line.length() > 0 &&  line.at(0) == '#' ){
				continue;
			}

			if( step == -1 ){
				std::istringstream stream(line);
				stream >> step;
				break;
			}
		}
		std::getline( input, line );
		if( line.find("data") != std::string::npos ){
			dtype = kmb::MicroAVSIO::DATA;
		}else if( line.find("geom") != std::string::npos ){
			dtype = kmb::MicroAVSIO::GEOM;
		}else if( line.find("data_geom") != std::string::npos ){
			dtype = kmb::MicroAVSIO::DATA_GEOM;
		}else{
			return -1;
		}
		return step;
	}else if( version == 8 ){
		int modelDataCount;
		std::string line;
		if( !input.eof() ){
			std::getline( input, line );
			std::istringstream stream(line);
			stream >> nodeCount >> elementCount >> nodeDimCount >> elementDimCount >> modelDataCount;
		}
		dtype = kmb::MicroAVSIO::DATA_GEOM;
		return 1;
	}
	return -1;
}

int
kmb::MicroAVSIO::readGeom(std::ifstream &input,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}
	if( version == 9 ){
		std::string etype,line;
		kmb::nodeIdType nodeId=kmb::nullNodeId;
		double x=0.0,y=0.0,z=0.0;
		input >> nodeCount >> elementCount;

		mesh->beginNode( nodeCount );
		for(int i=0;i<nodeCount;++i){
			input >> nodeId >> x >> y >> z;
			mesh->addNodeWithId(x,y,z,nodeId-nodeOffset);
		}
		mesh->endNode();
		std::getline( input, line );

		kmb::elementIdType elementId=kmb::Element::nullElementId;
		int material = 0;
		kmb::nodeIdType nodeTable[20] =
		{
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId
		};
		mesh->beginElement( elementCount );
		for(int i=0;i<elementCount;++i){
			input >> elementId;
			input >> material;
			input >> etype;
			if( etype == "pt" ){
			}else if( etype == "line" ){
				input >> nodeTable[0] >> nodeTable[1];
				for(int j=0;j<2;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::SEGMENT, nodeTable, elementId-elementOffset );
			}else if( etype == "line2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<3;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::SEGMENT2, nodeTable, elementId-elementOffset );
			}else if( etype == "tri" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<3;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TRIANGLE, nodeTable, elementId-elementOffset );
			}else if( etype == "tri2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				input >> nodeTable[5] >> nodeTable[3] >> nodeTable[4];
				for(int j=0;j<6;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TRIANGLE2, nodeTable, elementId-elementOffset );
			}else if( etype == "quad" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				for(int j=0;j<4;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::QUAD, nodeTable, elementId-elementOffset );
			}else if( etype == "quad2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				for(int j=0;j<8;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::QUAD2, nodeTable, elementId-elementOffset );
			}else if( etype == "tet" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[3] >> nodeTable[2];
				for(int j=0;j<4;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TETRAHEDRON, nodeTable, elementId-elementOffset );
			}else if( etype == "tet2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[3] >> nodeTable[2];
				input >> nodeTable[9] >> nodeTable[6] >> nodeTable[4] >> nodeTable[5] >> nodeTable[7] >> nodeTable[8];
				for(int j=0;j<10;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TETRAHEDRON2, nodeTable, elementId-elementOffset );
			}else if( etype == "pyr" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3] >> nodeTable[4];
				for(int j=0;j<5;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::PYRAMID, nodeTable, elementId-elementOffset );
			}else if( etype == "pyr2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3] >> nodeTable[4];
				input >> nodeTable[5] >> nodeTable[6] >> nodeTable[7] >> nodeTable[8];
				input >> nodeTable[9] >> nodeTable[10] >> nodeTable[11] >> nodeTable[12];
				for(int j=0;j<13;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::PYRAMID2, nodeTable, elementId-elementOffset );
			}else if( etype == "prism" ){
				input >> nodeTable[3] >> nodeTable[4] >> nodeTable[5];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<6;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::WEDGE, nodeTable, elementId-elementOffset );
			}else if( etype == "prism2" ){
				input >> nodeTable[3] >> nodeTable[4] >> nodeTable[5];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				input >> nodeTable[11] >> nodeTable[9] >> nodeTable[10];
				input >> nodeTable[8] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[12] >> nodeTable[13] >> nodeTable[14];
				for(int j=0;j<15;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::WEDGE2, nodeTable, elementId-elementOffset );
			}else if( etype == "hex" ){
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				for(int j=0;j<8;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::HEXAHEDRON, nodeTable, elementId-elementOffset );
			}else if( etype == "hex2" ){
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				input >> nodeTable[12] >> nodeTable[13] >> nodeTable[14] >> nodeTable[15];
				input >> nodeTable[8] >> nodeTable[9] >> nodeTable[10] >> nodeTable[11];
				input >> nodeTable[16] >> nodeTable[17] >> nodeTable[18] >> nodeTable[19];
				for(int j=0;j<20;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::HEXAHEDRON2, nodeTable, elementId-elementOffset );
			}
			std::getline( input, line );
		}
		mesh->endElement();
	}else if( version == 8 ){
		std::string etype,line;
		kmb::nodeIdType nodeId=kmb::nullNodeId;
		double x=0.0,y=0.0,z=0.0;
		mesh->beginNode( nodeCount );
		for(int i=0;i<nodeCount;++i){
			input >> nodeId >> x >> y >> z;
			mesh->addNodeWithId(x,y,z,nodeId-nodeOffset);
		}
		mesh->endNode();
		std::getline( input, line );

		kmb::elementIdType elementId=kmb::Element::nullElementId;
		int material = 0;
		kmb::nodeIdType nodeTable[20] =
		{
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId,
			kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId, kmb::nullNodeId
		};
		mesh->beginElement( elementCount );
		for(int i=0;i<elementCount;++i){
			input >> elementId;
			input >> material;
			input >> etype;
			if( etype == "pt" ){
			}else if( etype == "line" ){
				input >> nodeTable[0] >> nodeTable[1];
				for(int j=0;j<2;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::SEGMENT, nodeTable, elementId-elementOffset );
			}else if( etype == "line2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<3;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::SEGMENT2, nodeTable, elementId-elementOffset );
			}else if( etype == "tri" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<3;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TRIANGLE, nodeTable, elementId-elementOffset );
			}else if( etype == "tri2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				input >> nodeTable[5] >> nodeTable[3] >> nodeTable[4];
				for(int j=0;j<6;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TRIANGLE2, nodeTable, elementId-elementOffset );
			}else if( etype == "quad" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				for(int j=0;j<4;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::QUAD, nodeTable, elementId-elementOffset );
			}else if( etype == "quad2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				for(int j=0;j<8;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::QUAD2, nodeTable, elementId-elementOffset );
			}else if( etype == "tet" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[3] >> nodeTable[2];
				for(int j=0;j<4;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TETRAHEDRON, nodeTable, elementId-elementOffset );
			}else if( etype == "tet2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[3] >> nodeTable[2];
				input >> nodeTable[9] >> nodeTable[6] >> nodeTable[4] >> nodeTable[5] >> nodeTable[7] >> nodeTable[8];
				for(int j=0;j<10;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::TETRAHEDRON2, nodeTable, elementId-elementOffset );
			}else if( etype == "pyr" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3] >> nodeTable[4];
				for(int j=0;j<5;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::PYRAMID, nodeTable, elementId-elementOffset );
			}else if( etype == "pyr2" ){
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3] >> nodeTable[4];
				input >> nodeTable[5] >> nodeTable[6] >> nodeTable[7] >> nodeTable[8];
				input >> nodeTable[9] >> nodeTable[10] >> nodeTable[11] >> nodeTable[12];
				for(int j=0;j<13;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::PYRAMID2, nodeTable, elementId-elementOffset );
			}else if( etype == "prism" ){
				input >> nodeTable[3] >> nodeTable[4] >> nodeTable[5];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				for(int j=0;j<6;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::WEDGE, nodeTable, elementId-elementOffset );
			}else if( etype == "prism2" ){
				input >> nodeTable[3] >> nodeTable[4] >> nodeTable[5];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2];
				input >> nodeTable[11] >> nodeTable[9] >> nodeTable[10];
				input >> nodeTable[8] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[12] >> nodeTable[13] >> nodeTable[14];
				for(int j=0;j<15;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::WEDGE2, nodeTable, elementId-elementOffset );
			}else if( etype == "hex" ){
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				for(int j=0;j<8;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::HEXAHEDRON, nodeTable, elementId-elementOffset );
			}else if( etype == "hex2" ){
				input >> nodeTable[4] >> nodeTable[5] >> nodeTable[6] >> nodeTable[7];
				input >> nodeTable[0] >> nodeTable[1] >> nodeTable[2] >> nodeTable[3];
				input >> nodeTable[12] >> nodeTable[13] >> nodeTable[14] >> nodeTable[15];
				input >> nodeTable[8] >> nodeTable[9] >> nodeTable[10] >> nodeTable[11];
				input >> nodeTable[16] >> nodeTable[17] >> nodeTable[18] >> nodeTable[19];
				for(int j=0;j<20;++j){
					nodeTable[j] -= nodeOffset;
				}
				mesh->addElementWithId( kmb::HEXAHEDRON2, nodeTable, elementId-elementOffset );
			}
			std::getline( input, line );
		}
		mesh->endElement();
	}
	return 0;
}

int
kmb::MicroAVSIO::skipGeom(std::ifstream &input)
{
	std::string line;
	if( version == 9 ){
		input >> nodeCount >> elementCount;
		std::getline( input, line );
		for(int i=0;i<nodeCount;++i){
			std::getline( input, line );
		}
		for(int i=0;i<elementCount;++i){
			std::getline( input, line );
		}
	}else if( version == 8 ){
		for(int i=0;i<nodeCount;++i){
			std::getline( input, line );
		}
		for(int i=0;i<elementCount;++i){
			std::getline( input, line );
		}
	}
	return 0;
}


void
kmb::MicroAVSIO::getDataName(std::string &line)
{
	size_t comma = line.find_first_of(",");
	if(comma!=std::string::npos){
		line.erase(comma);
	}
	size_t not_space = line.find_first_not_of(" ");
	if(not_space!=std::string::npos){
		line.erase(0,not_space);
	}
}

int
kmb::MicroAVSIO::readData(std::ifstream &input,kmb::MeshData* mesh)
{
	struct dataItem{
		std::string name;
		int dim;
		kmb::DataBindings *data;
	};
	if( mesh == NULL ){
		return -1;
	}
	if( version == 9 ){
		int nodeDataCount = 0, elementDataCount = 0;

		input >> nodeDimCount >> elementDimCount;
		if( nodeDimCount > 0 ){

			input >> nodeDataCount;
			if( nodeDataCount>0 ){
				std::string line;
				dataItem* items = new dataItem[nodeDataCount];
				for(int i=0;i<nodeDataCount;++i){
					input >> items[i].dim;
				}
				std::getline( input, line );

				for(int i=0;i<nodeDataCount;++i){
					std::getline( input, line );
					getDataName(line);
					items[i].name = line;
					items[i].data = NULL;
				}

				for(int i=0;i<nodeDataCount-2;++i){
					size_t semiColon = items[i].name.find(";");
					if( semiColon != std::string::npos ){
						items[i].name.erase(0,semiColon+1);
						items[i].dim += items[i+1].dim;
						items[i].dim += items[i+2].dim;
						items[i+1].dim = 0;
						items[i+2].dim = 0;
					}
				}

				for(int i=0;i<nodeDataCount;++i){
					size_t bar = items[i].name.find("_1");
					if( bar != std::string::npos ){
						items[i].name.erase(bar);
						for(int j=i+1;j<nodeDataCount;++j){
							if( items[j].name.find( items[i].name + "_" ) != std::string::npos ){
								items[i].dim += items[j].dim;
								items[j].dim = 0;
							}else{
								i = j-1;
								break;
							}
							if( j==i+5 ){
								i = j;
								break;
							}
						}
					}
				}

				if( asVector3 && nodeDataCount == 3 && items[0].dim == 1 && items[1].dim == 1 && items[1].dim == 1 ){
					items[0].dim = 3;
					items[1].dim = 0;
					items[2].dim = 0;
				}
				for(int i=0;i<nodeDataCount;++i){
					switch( items[i].dim ){
					case 1:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::NODEVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::SCALAR ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NODEVARIABLE, kmb::PhysicalValue::SCALAR, "post" );
						}
						break;
					case 2:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::NODEVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::VECTOR2 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NODEVARIABLE, kmb::PhysicalValue::VECTOR2, "post" );
						}
						break;
					case 3:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::NODEVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::VECTOR3 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NODEVARIABLE, kmb::PhysicalValue::VECTOR3, "post" );
						}
						break;
					case 6:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::NODEVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::TENSOR6 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NODEVARIABLE, kmb::PhysicalValue::TENSOR6, "post" );
						}
						break;
					default:
						break;
					}
				}
				int nodeCount = static_cast<int>( mesh->getNodeCount() );
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				double* values = new double[nodeDimCount];
				for(int i = 0;i<nodeCount;++i){
					input >> nodeId;
					for(int j=0;j<nodeDimCount;++j){
						input >> values[j];
					}
					int index = 0;
					for(int k=0;k<nodeDataCount;++k){
						if( items[k].data != NULL ){
							items[k].data->setPhysicalValue( nodeId-nodeOffset, &(values[index]) );
						}
						index += items[k].dim;
					}
					std::getline( input, line );
				}
				delete[] values;
				delete[] items;
			}
		}
		if( elementDimCount > 0 ){

			input >> elementDataCount;
			if( elementDataCount>0 ){
				std::string line;
				dataItem* items = new dataItem[elementDataCount];
				for(int i=0;i<elementDataCount;++i){
					input >> items[i].dim;
				}
				std::getline( input, line );

				for(int i=0;i<elementDataCount;++i){
					std::getline( input, line );
					getDataName(line);
					items[i].name = line;
					items[i].data = NULL;
				}

				for(int i=0;i<elementDataCount-2;++i){
					size_t semiColon = items[i].name.find(";");
					if( semiColon != std::string::npos ){
						items[i].name.erase(0,semiColon+1);
						items[i].dim += items[i+1].dim;
						items[i].dim += items[i+2].dim;
						items[i+1].dim = 0;
						items[i+2].dim = 0;
					}
				}

				for(int i=0;i<elementDataCount;++i){
					size_t bar = items[i].name.find("_1");
					if( bar != std::string::npos ){
						items[i].name.erase(bar);
						for(int j=i+1;j<elementDataCount;++j){
							if( items[j].name.find( items[i].name + "_" ) != std::string::npos ){
								items[i].dim += items[j].dim;
								items[j].dim = 0;
							}else{
								i = j-1;
								break;
							}
							if( j==i+5 ){
								i = j;
								break;
							}
						}
					}
				}
				for(int i=0;i<elementDataCount;++i){
					switch( items[i].dim ){
					case 1:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::SCALAR ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ELEMENTVARIABLE, kmb::PhysicalValue::SCALAR, "post" );
						}
						break;
					case 2:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::VECTOR2 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ELEMENTVARIABLE, kmb::PhysicalValue::VECTOR2, "post" );
						}
						break;
					case 3:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::VECTOR3 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ELEMENTVARIABLE, kmb::PhysicalValue::VECTOR3, "post" );
						}
						break;
					case 6:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::TENSOR6 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ELEMENTVARIABLE, kmb::PhysicalValue::TENSOR6, "post" );
						}
						break;
					default:
						break;
					}
				}
				int elementCount = static_cast<int>( mesh->getElementCount() );
				kmb::elementIdType elementId = kmb::Element::nullElementId;
				double* values = new double[elementDimCount];
				for(int i = 0;i<elementCount;++i){
					input >> elementId;
					for(int j=0;j<elementDimCount;++j){
						input >> values[j];
					}
					int index = 0;
					for(int k=0;k<elementDataCount;++k){
						if( items[k].data != NULL ){
							items[k].data->setPhysicalValue( elementId-elementOffset, &(values[index]) );
						}
						index += items[k].dim;
					}
					std::getline( input, line );
				}
				delete[] values;
				delete[] items;
			}
		}
	}else if( version == 8 ){
		int nodeDataCount = 0, elementDataCount = 0;
		if( nodeDimCount > 0 ){

			input >> nodeDataCount;
			if( nodeDataCount>0 ){
				std::string line;
				dataItem* items = new dataItem[nodeDataCount];
				for(int i=0;i<nodeDataCount;++i){
					input >> items[i].dim;
				}
				std::getline( input, line );

				for(int i=0;i<nodeDataCount;++i){
					std::getline( input, line );
					getDataName(line);
					items[i].name = line;
					items[i].data = NULL;
				}

				for(int i=0;i<nodeDataCount-2;++i){
					size_t semiColon = items[i].name.find(";");
					if( semiColon != std::string::npos ){
						items[i].name.erase(0,semiColon+1);
						items[i].dim += items[i+1].dim;
						items[i].dim += items[i+2].dim;
						items[i+1].dim = 0;
						items[i+2].dim = 0;
					}
				}

				for(int i=0;i<nodeDataCount;++i){
					size_t bar = items[i].name.find("_1");
					if( bar != std::string::npos ){
						items[i].name.erase(bar);
						for(int j=i+1;j<nodeDataCount;++j){
							if( items[j].name.find( items[i].name + "_" ) != std::string::npos ){
								items[i].dim += items[j].dim;
								items[j].dim = 0;
							}else{
								i = j-1;
								break;
							}
							if( j==i+5 ){
								i = j;
								break;
							}
						}
					}
				}

				if( asVector3 && nodeDataCount == 3 && items[0].dim == 1 && items[1].dim == 1 && items[1].dim == 1 ){
					items[0].dim = 3;
					items[1].dim = 0;
					items[2].dim = 0;
				}
				for(int i=0;i<nodeDataCount;++i){
					switch( items[i].dim ){
					case 1:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::NODEVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::SCALAR ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NODEVARIABLE, kmb::PhysicalValue::SCALAR, "post" );
						}
						break;
					case 2:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::NODEVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::VECTOR2 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NODEVARIABLE, kmb::PhysicalValue::VECTOR2, "post" );
						}
						break;
					case 3:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::NODEVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::VECTOR3 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NODEVARIABLE, kmb::PhysicalValue::VECTOR3, "post" );
						}
						break;
					case 6:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::NODEVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::TENSOR6 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::NODEVARIABLE, kmb::PhysicalValue::TENSOR6, "post" );
						}
						break;
					default:
						break;
					}
				}
				int nodeCount = static_cast<int>( mesh->getNodeCount() );
				kmb::nodeIdType nodeId = kmb::nullNodeId;
				double* values = new double[nodeDimCount];
				for(int i = 0;i<nodeCount;++i){
					input >> nodeId;
					for(int j=0;j<nodeDimCount;++j){
						input >> values[j];
					}
					int index = 0;
					for(int k=0;k<nodeDataCount;++k){
						if( items[k].data != NULL ){
							items[k].data->setPhysicalValue( nodeId-nodeOffset, &(values[index]) );
						}
						index += items[k].dim;
					}
					std::getline( input, line );
				}
				delete[] values;
				delete[] items;
			}
		}
		if( elementDimCount > 0 ){

			input >> elementDataCount;
			if( elementDataCount>0 ){
				std::string line;
				dataItem* items = new dataItem[elementDataCount];
				for(int i=0;i<elementDataCount;++i){
					input >> items[i].dim;
				}
				std::getline( input, line );

				for(int i=0;i<elementDataCount;++i){
					std::getline( input, line );
					getDataName(line);
					items[i].name = line;
					items[i].data = NULL;
				}

				for(int i=0;i<elementDataCount-2;++i){
					size_t semiColon = items[i].name.find(";");
					if( semiColon != std::string::npos ){
						items[i].name.erase(0,semiColon+1);
						items[i].dim += items[i+1].dim;
						items[i].dim += items[i+2].dim;
						items[i+1].dim = 0;
						items[i+2].dim = 0;
					}
				}

				for(int i=0;i<elementDataCount;++i){
					size_t bar = items[i].name.find("_1");
					if( bar != std::string::npos ){
						items[i].name.erase(bar);
						for(int j=i+1;j<elementDataCount;++j){
							if( items[j].name.find( items[i].name + "_" ) != std::string::npos ){
								items[i].dim += items[j].dim;
								items[j].dim = 0;
							}else{
								i = j-1;
								break;
							}
							if( j==i+5 ){
								i = j;
								break;
							}
						}
					}
				}
				for(int i=0;i<elementDataCount;++i){
					switch( items[i].dim ){
					case 1:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::SCALAR ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ELEMENTVARIABLE, kmb::PhysicalValue::SCALAR, "post" );
						}
						break;
					case 2:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::VECTOR2 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ELEMENTVARIABLE, kmb::PhysicalValue::VECTOR2, "post" );
						}
						break;
					case 3:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::VECTOR3 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ELEMENTVARIABLE, kmb::PhysicalValue::VECTOR3, "post" );
						}
						break;
					case 6:
						items[i].data = mesh->getDataBindingsPtr( items[i].name.c_str(), "post" );
						if( items[i].data &&
							( items[i].data->getBindingMode() != kmb::DataBindings::ELEMENTVARIABLE ||
							  items[i].data->getValueType() != kmb::PhysicalValue::TENSOR6 ) )
						{
							items[i].data = NULL;
							mesh->deleteData( items[i].name.c_str(), "post" );

						}
						if( items[i].data == NULL ){
							items[i].data = mesh->createDataBindings( items[i].name.c_str(), kmb::DataBindings::ELEMENTVARIABLE, kmb::PhysicalValue::TENSOR6, "post" );
						}
						break;
					default:
						break;
					}
				}
				int elementCount = static_cast<int>( mesh->getElementCount() );
				kmb::elementIdType elementId = kmb::Element::nullElementId;
				double* values = new double[elementDimCount];
				for(int i = 0;i<elementCount;++i){
					input >> elementId;
					for(int j=0;j<elementDimCount;++j){
						input >> values[j];
					}
					int index = 0;
					for(int k=0;k<elementDataCount;++k){
						if( items[k].data != NULL ){
							items[k].data->setPhysicalValue( elementId-elementOffset, &(values[index]) );
						}
						index += items[k].dim;
					}
					std::getline( input, line );
				}
				delete[] values;
				delete[] items;
			}
		}
	}
	return 0;
}

int
kmb::MicroAVSIO::loadFromFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		std::ifstream input( filename, std::ios_base::in );
		std::string line;
		if( input.fail() ){
			return -1;
		}
		version = getVersion(input);
		if( version == -1 ){
			input.close();
			return -1;
		}
		input.close();
		input.open( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		int step = readHeader(input);
		if( step == -1 ){
			input.close();
			return -1;
		}
		if( version == 8 ){
			readGeom(input,mesh);
			readData(input,mesh);
		}else if( version == 9 ){
			for(int i=0;i<step;++i){
				std::getline( input, line );
				if( line.find("step") == std::string::npos ){
					break;
				}
				if( i==0 || dtype & kmb::MicroAVSIO::GEOM ){
					readGeom(input,mesh);
				}
				if( i==0 || dtype & kmb::MicroAVSIO::DATA ){
					readData(input,mesh);
				}
			}
		}
		input.close();
		return 0;
	}
}

int
kmb::MicroAVSIO::loadPostFromFile(const char* filename,kmb::MeshData* mesh)
{
	if( mesh == NULL ){
		return -1;
	}else{
		std::ifstream input( filename, std::ios_base::in );
		std::string line;
		if( input.fail() ){
			return -1;
		}
		version = getVersion(input);
		if( version == -1 ){
			input.close();
			return -1;
		}
		input.close();
		input.open( filename, std::ios_base::in );
		if( input.fail() ){
			return -1;
		}
		int step = readHeader(input);
		if( step == -1 ){
			input.close();
			return -1;
		}
		if( version == 8 ){
			skipGeom(input);
			readData(input,mesh);
		}else if( version == 9 ){
			for(int i=0;i<step;++i){
				std::getline( input, line );
				if( line.find("step") == std::string::npos ){
					break;
				}
				if( i==0 || dtype & kmb::MicroAVSIO::DATA ){
					skipGeom(input);
				}
				if( i==0 || dtype & kmb::MicroAVSIO::GEOM ){
					readData(input,mesh);
				}
			}
		}
		input.close();
		return 0;
	}
}

int
kmb::MicroAVSIO::saveToFile(const char* filename,kmb::MeshData* mesh)
{
	return 0;
}