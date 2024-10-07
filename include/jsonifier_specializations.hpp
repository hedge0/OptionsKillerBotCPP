#pragma once

#include <jsonifier/Index.hpp>

static thread_local jsonifier::jsonifier_core parser{};

struct observation_data {
	double value{};
};

struct risk_free_rate_data {
	observation_data observations{};
};

struct StockNode {
	std::string ticker;
	uint64_t date_index;
	uint64_t date;
	std::string option_type;
	double min_overpriced;
	double min_underpriced;
	double min_oi;
	StockNode* next;
};

namespace jsonifier {

	template<> struct core<StockNode> {
		using value_type				 = StockNode;
		static constexpr auto parseValue = createValue<&value_type::date, &value_type::date_index, &value_type::min_oi, &value_type::min_overpriced, &value_type::min_underpriced,
			&value_type::next, &value_type::option_type, &value_type::ticker>();
	};

	template<> struct core<observation_data> {
		using value_type				 = observation_data;
		static constexpr auto parseValue = createValue<&value_type::value>();
	};

	template<> struct core<risk_free_rate_data> {
		using value_type				 = risk_free_rate_data;
		static constexpr auto parseValue = createValue<&value_type::observations>();
	};
}