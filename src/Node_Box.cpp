#include "Node_Box.h"

Node_Box::Node_Box(SceneGraph* p_Sg):
	Node_Block(p_Sg)
{
	Node_Text* curr = new Node_Text(p_Sg, "Id");
	curr = new Node_Text(p_Sg, "Text");
	curr = new Node_Text(p_Sg, "Comment");
	curr = new Node_Text(p_Sg, "Tag");
	
	p_Sg->Text("bonjour");
	p_Sg->Text("ola");
	p_Sg->Text("guten tag");
}

void Node_Box::_subjectUpdate(SSS::Subject const& subject, int event_id)
{


}
