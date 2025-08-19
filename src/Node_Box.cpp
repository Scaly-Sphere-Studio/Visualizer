#include "Node_Box.h"

Node_Box::Node_Box(SceneGraph* p_Sg):
	Node_Block(p_Sg)
{
	Node_UI* first = new Node_Text(p_Sg, "Id");
	Node_UI* textNode = new Node_Text(p_Sg, "Text");
	textNode->setVerticalOffset(first->_key);
	Node_UI* comNode = new Node_Text(p_Sg, "Comment");
	comNode->setVerticalOffset(textNode->_key);
	Node_UI* tagNode = new Node_Text(p_Sg, "Tag");
	tagNode->setVerticalOffset(comNode->_key);

	float min = std::max({ first->_size.x, textNode->_size.x,  comNode->_size.x,  tagNode->_size.x});

	//first->setWrappingMin(static_cast<int>(min));
	//textNode->setWrappingMin(static_cast<int>(min));
	//comNode->setWrappingMin(static_cast<int>(min));
	//tagNode->setWrappingMin(static_cast<int>(min));

}

void Node_Box::_subjectUpdate(SSS::Subject const& subject, int event_id)
{


}
