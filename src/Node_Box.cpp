#include "Node_Box.h"

Node_Box::Node_Box(SceneGraph* p_Sg):
	Node_Block(p_Sg)
{
	p_Sg->Text("bonsoir");
	p_Sg->Text("bonjour");
	p_Sg->Text("ola");
	p_Sg->Text("guten tag");
}

void Node_Box::_subjectUpdate(SSS::Subject const& subject, int event_id)
{


}
