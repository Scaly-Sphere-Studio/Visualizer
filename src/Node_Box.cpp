#include "Node_Box.h"

Node_Box::Node_Box(SceneGraph* p_Sg):
	Node_Block(p_Sg)
{
	Node_UI* first = new Node_Text(p_Sg, "Id");
	Node_UI* textNode = new Node_Text(p_Sg, "Text");
	textNode->setHorizontalOffset(first->_key);
	Node_UI* comNode = new Node_Text(p_Sg, "Comment");
	comNode->setHorizontalOffset(textNode->_key);
	Node_UI* tagNode = new Node_Text(p_Sg, "Tag");
	tagNode->setHorizontalOffset(comNode->_key);

	
	//p_Sg->Text("bonjour");
	//p_Sg->Text("ola");
	//p_Sg->Text("guten tag");
}

void Node_Box::_subjectUpdate(SSS::Subject const& subject, int event_id)
{


}
