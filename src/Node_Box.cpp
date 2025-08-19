#include "Node_Box.h"

Node_Box::Node_Box(SceneGraph* p_Sg):
	Node_Block(p_Sg)
{

	_color = { 0.979112982749939, 0.8841438889503479,	0.9663259983062744, 1.0 };
	glm::vec4 tex_col = SSS::RGBA_f(_color).to_HSL();
	glm::vec4 bg_col = tex_col;
	Node_Text* first = new Node_Text(p_Sg, "Id");
		tex_col.b = 0.3f;
		bg_col.b -= 0.15f;
		first->setTextColor(SSS::RGBA_f::from_HSL(tex_col));
		first->setBackgroundColor(SSS::RGBA_f::from_HSL((bg_col)));

	Node_Text* textNode = new Node_Text(p_Sg, "Text");
	textNode->setVerticalOffset(first->_key);

		//textNode->setTextColor(SSS::RGBA_f::from_HSL(tex_col));
		textNode->setBackgroundColor(SSS::RGBA_f{ _color });
	
	//Node_Text* comNode = new Node_Text(p_Sg, "Comment");
	//comNode->setVerticalOffset(textNode->_key);
	//	tex_col.b = 0.3f;
	//	bg_col.b -= 0.15f;
	//	comNode->setTextColor(SSS::RGBA_f::from_HSL(tex_col));
	//	comNode->setBackgroundColor(SSS::RGBA_f::from_HSL((bg_col)));

	//Node_Text* tagNode = new Node_Text(p_Sg, "Tag");
	//tagNode->setVerticalOffset(comNode->_key);
	//	tex_col.b = 0.3f;
	//	bg_col.b -= 0.15f;
	//	tagNode->setTextColor(SSS::RGBA_f::from_HSL(tex_col));
	//	tagNode->setBackgroundColor(SSS::RGBA_f::from_HSL((bg_col)));


		//float min = std::max({ first->_size.x, textNode->_size.x,  comNode->_size.x,  tagNode->_size.x });
		float min = std::max({ first->_size.x, textNode->_size.x });

	first->setWrappingMin(static_cast<int>(min));
	textNode->setWrappingMin(static_cast<int>(min));
	//comNode->setWrappingMin(static_cast<int>(min));
	//tagNode->setWrappingMin(static_cast<int>(min));

}

void Node_Box::_subjectUpdate(SSS::Subject const& subject, int event_id)
{


}
