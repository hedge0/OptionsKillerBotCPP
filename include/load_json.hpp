#ifndef LOAD_JSON_H
#define LOAD_JSON_H

#include <string>
#include <config.hpp>
#include <jsonifier_specializations.hpp>

/**
 * @brief Global pointer to the head of the circular linked list.
 */
StockNode* stocks_data_head = nullptr;

/**
 * @brief Loads a JSON file into memory and stores its data in a circular linked list.
 *
 * This function reads a JSON file, parses its content, and stores the resulting
 * data into a circular linked list. Each JSON object is mapped to a node in the
 * linked list containing "ticker", "date", "option_type", "min_overpriced",
 * "min_underpriced", and "min_oi" keys.
 *
 * @param file_path The path to the JSON file to be loaded.
 */
OPTS_BOT_ALWAYS_INLINE static void load_json_file(const std::string& file_path) {
	std::ifstream file(file_path);
	if (!file.is_open()) {
		std::cerr << "Could not open JSON file: " << file_path << std::endl;
		return;
	}

	StockNode* tail = nullptr;

	try {
		std::stringstream stream{};
		stream << file.rdbuf();
		std::vector<StockNode> values{};
		parser.parseJson(values, stream.str());
		for (const auto& item: values) {
			StockNode* new_node		  = new StockNode;
			new_node->ticker		  = item.ticker;
			new_node->date_index	  = item.date;
			new_node->date			  = item.date;
			new_node->option_type	  = item.option_type;
			new_node->min_overpriced  = item.min_overpriced;
			new_node->min_underpriced = item.min_underpriced;
			new_node->min_oi		  = item.min_oi;
			new_node->next			  = nullptr;

			if (stocks_data_head == nullptr) {
				stocks_data_head = new_node;
				tail			 = new_node;
			} else {
				tail->next = new_node;
				tail	   = new_node;
			}
		}

		StockNode* tail = nullptr;

		if (tail != nullptr) {
			tail->next = stocks_data_head;
		}
	} catch (const std::exception& e) {
		std::cerr << "Error parsing JSON file: " << e.what() << std::endl;
	}

	file.close();
}

#endif
