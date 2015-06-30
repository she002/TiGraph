#ifndef __CSR_H__
#define __CSR_H__

#include <vector>
#include "reader.hpp"

template<typename VertexType, typename EdgeType> 
class CSR
{
	public:
		//constructor
		CSR(reference_data* ref_data);
		//CSR(reference_data* ref_data);

		//accessor
		VertexType getVertexValue(IdType Id);
		long int getOutEdgeDestId(IdType Id, int i);
		long int getInEdgeSrcId(IdType Id, int i);
		EdgeType getInEdgeValue(IdType Id, int i);
		EdgeType getOutEdgeValue(IdType Id, int i);
		int getNumInEdges(IdType Id);
		int getNumOutEdges(IdType Id);
		int getMaxId();
		int getNumEdges();
		//modifier
		void setVertexValue(IdType Id, VertexType value);
		void setInEdgeValue(IdType Id, int i, EdgeType value);
		void setOutEdgeValue(IdType Id, int i, EdgeType value);
		//destructor
		~CSR();

	private:
		int NumEdges;
		//int* InEdge;
		//int* OutEdge;
		IdType MaxId;
		IdType* InEdgeIdxs;
		IdType* OutEdgeIdxs;
		VertexType* VertexValues;
		IdType* SrcIndex;
		IdType* DestIndex;
		EdgeType* InEdgeValues;
		EdgeType* OutEdgeValues;
		
};

#endif
