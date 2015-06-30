#ifndef __ARGS_H__
#define __ARGS_H__

template<typename VertexType, typename EdgeType, typename GatherType>
class Engine;

template<typename VertexType, typename EdgeType, typename GatherType>
class args{
	public:
	IdType tid;
	Engine<VertexType, EdgeType, GatherType>* context;
};

#endif
