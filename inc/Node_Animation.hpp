#include "Animation.hpp"
#include <SSS/SceneGraph/Node_UI.h>




class Node_Animation : public SSS::Node_UI, public SSS::_EventRegistry<Node_Animation> {

public:
	friend _EventRegistry<Node_Animation>;

	Node_Animation() = default;
	//Track t;
private:
	static void _register();
};