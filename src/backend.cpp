#include "backend.hpp"

std::string time_stamp()
{
	//Access the TAI(Internation Atomic Time) and convert its value into a hex string
	return hex_convertor(std::chrono::steady_clock::now().time_since_epoch().count());
}

std::string ID_text_data_stringify(std::string cat, std::string sub_cat, std::string time_stamp)
{
	//Text ID : CAT_SUB_HEXTIMESTAMP
	return cat + "_"+ sub_cat + "_" + time_stamp;
}

std::string ID_trad_file_stringify(std::string userID, std::string time_stamp)
{
	return userID + "_" + time_stamp;
}

bool check_folder_exists(const std::string &path)
{	
	std::string abs_path_str = absolute_path(path);

	//Check if the folder exists using it's supposed path
	if (std::filesystem::exists(path)) {
		SSS::log_msg("[FILE] : " + abs_path_str + " folder already exists");
		return true;
	}
	else {
		SSS::log_msg("[FILE] : \"" + abs_path_str + "\" folder doesn't exists");
	}
	return false;
}

void create_folder(const std::string &path)
{
	std::filesystem::create_directory(path);

	SSS::log_msg("[FILE] : Generating : \"" + absolute_path(path) + "\" folder");
}


std::string absolute_path(const std::string& path)
{
	std::string absolute_path;

	//check if the path is a relative path and log the absolute path
	if (static_cast<std::filesystem::path>(path).is_relative()) {
		absolute_path = std::filesystem::current_path().string() + "\\" + path;
	}
	else {
		absolute_path = path;
	}

	return absolute_path;
}

void create_traduction_file(std::string& path, uint16_t language, Traduction_data& mother)
{
	//TODO ???
	Traduction_data t;
	t.language = language;
	if (mother.magnitude == 1) {
		//text copied from the mother language file
		t.magnitude = 0;
	}
	else {
		//text copied from another daughter file
		//Warning during creation process, loss of information during this traduction
		t.magnitude = 2;
	}

	t.trad_ID = mother.trad_ID;
	
	//init the traduction file
	//and create an empty copy of all the text data and their ID from the mother file
	t.text_data.reserve(mother.text_data.size());
	Text_data d;
	for (size_t i = 0; i < mother.text_data.size(); i++)
	{
		d.text_ID = mother.text_data[i].text_ID;
		t.text_data.emplace_back(d);
	}


}

void gen_info_traduction_file(const std::string& path)
{

}

void create_files(const std::string name, Traduction_data &data)
{

	// write prettified JSON to another file

}

std::map<std::string, std::string> retrieve_iso_codes(const std::string path)
{
	// Open the file with the ISO codes map
	std::ifstream fs(path);
	// Parse the json
	nlohmann::json j;
	fs >> j;

	return j.get<std::map<std::string, std::string>>();
}


void string_compare(const std::string& first, const std::string& second)
{
	//if ((first.size() || second.size()) == 0) {
	//	//Check if at least one of the two string is empty
	//	return;
	//} 
	//
	//else if (first == second) 
	//{
	//	//Simply check if the two strings are identicals
	//	std::cout << "The two strings are identical" << std::endl;
	//	return;
	//} else  if (abs(floor(first.length() - second.length()) < floor(first.length() * 0.1))) {
	//	return;
	//}
	//else {
	//	return;
	//}

	std::cout << first[0];
}


struct InputTextCallback_UserData
{
	std::string* Str;
	ImGuiInputTextCallback  ChainCallback;
	void* ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		// If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
		std::string* str = user_data->Str;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}
	else if (user_data->ChainCallback)
	{
		// Forward to user callback, if any
		data->UserData = user_data->ChainCallbackUserData;
		return user_data->ChainCallback(data);
	}
	return 0;
}

bool ImGui::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
	IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
	flags |= ImGuiInputTextFlags_CallbackResize;

	InputTextCallback_UserData cb_user_data;
	cb_user_data.Str = str;
	cb_user_data.ChainCallback = callback;
	cb_user_data.ChainCallbackUserData = user_data;
	return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

