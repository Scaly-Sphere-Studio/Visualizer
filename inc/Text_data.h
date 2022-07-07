#pragma once

#include <iostream>
#include <fstream>
#include <filesystem>
#include <array>
#include <string>
#include <stdint.h>

#include <SSS/commons.hpp>
#include <nlohmann/json.hpp>

struct Text_data {
public:
	std::string text_ID;
	std::string text;
	std::string comment;
	uint32_t category = UINT32_MAX;
	/*Evaluation text_eval;*/


	//LOG AND OPERATOR OVERLOADS
	friend std::ostream& operator<<(std::ostream& output, const Text_data& txt) {
		output << "[ID]:" << txt.text_ID <<
			", [TEXT]:" << txt.text;
		return output;
	}
};

class Traduction_data {
public:
	Traduction_data();
	Traduction_data(std::string& path);
	~Traduction_data();

	// ID is the reference used to track the Traduction and the user
	// Language is the 3 number ISO code
	// Magnitude represent the order to which the traduction has been passed
	// Order 0 is the mother text, Order 1 is first traduction, more is a traduction of a traduction
	// Evaluation of the traduction is the mean of all text data evaluations
	void print();
	//DATA
	std::string trad_ID;
	std::string User_ID;
	std::string language;
	uint16_t magnitude = 0;
	float trad_evaluation = 0.0f;
	std::vector<Text_data> text_data;
	std::map<int, std::string> categories;

	//OPERATOR OVERLOAD
	friend std::ostream& operator<<(std::ostream& output, const Traduction_data& trad) {
		output << "[TRAD_ID]:" << trad.trad_ID <<
			", [LANGUAGE]:" << trad.language;
		if (trad.magnitude == 0) {
			output << ", [MOTHER_FILE]" << std::endl;
		}

		for (Text_data txt : trad.text_data) {
			output << txt << std::endl;
		}
		return output;
	}

	//Parse the traduction data into a json file
	void parse_traduction_data_to_json(const std::string& path, const bool prettify = false);
	//Fill the Traduction data structure with the data from a given json
	void parse_traduction_data_from_json(const std::string& path);
	void push_text_data(std::string text, std::string comment, std::string ID, uint32_t cat);
	void remove_text_data(std::string ID);
};


class Trad_info {
public:
	std::string trad_ID;
	std::string mother_language;
	//Need to save it as a map, because the json lib is bugged with string vectors
	//Stock the language shortcut and its traduction order
	std::map<std::string, uint32_t> trad_languages;
	std::string fl;
	std::string sl;

	//Parse the traduction data into a json file
	void parse_info_data_to_json(const std::string& path, const bool prettify = false);
	//Fill the Traduction data structure with the data from a given json
	void parse_info_data_from_json(const std::string& path);
};

//JSON CONVERTION
//Text data convertion
void to_json(nlohmann::json& j, const Text_data& t);
void from_json(const nlohmann::json& j, Text_data& t);

//Traduction data convertion
void to_json(nlohmann::json& j, const Traduction_data& t);
void from_json(const nlohmann::json& j, Traduction_data& t);

//Traduction infos
void to_json(nlohmann::json& j, const Trad_info& t);
void from_json(const nlohmann::json& j, Trad_info& t);
