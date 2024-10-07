/*
	MIT License

	DiscordCoreAPI, A bot library for Discord, written in C++, and featuring explicit multithreading through the usage of custom, asynchronous C++ CoRoutines.

	Copyright 2022, 2023 Chris M. (RealTimeChris)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
/// HttpsClient.hpp - Header file for the "Https stuff".
/// May 12, 2021
/// https://discordcoreapi.com
/// \file HttpsClient.hpp
#pragma once

#include <rate_limit_queue.hpp>

enum class https_workload_class : uint8_t { Get = 0, Put = 1, Post = 2, Patch = 3, Delete = 4 };

enum class payload_type : uint8_t { Application_Json = 1, Multipart_Form = 2 };

class https_workload_data {
  public:
	friend class https_client;

	std::unordered_map<jsonifier::string, jsonifier::string> headersToInsert{};
	payload_type payloadType{ payload_type::Application_Json };
	https_workload_class workloadClass{};
	jsonifier::string relativePath{};
	jsonifier::string callStack{};
	jsonifier::string baseUrl{};
	jsonifier::string content{};

	https_workload_data() = default;

	https_workload_data& operator=(const https_workload_data& other) = delete;
	https_workload_data(const https_workload_data& other)			 = delete;

	https_workload_data& operator=(https_workload_data&& other) noexcept {
		if (this != &other) {
			headersToInsert = std::move(other.headersToInsert);
			relativePath	= std::move(other.relativePath);
			callStack		= std::move(other.callStack);
			baseUrl			= std::move(other.baseUrl);
			content			= std::move(other.content);
			workloadClass	= other.workloadClass;
			workloadType	= other.workloadType;
			payloadType		= other.payloadType;
		}
		return *this;
	}

	https_workload_data(https_workload_data&& other) noexcept {
		*this = std::move(other);
	}

	https_workload_data& operator=(https_workload_type type) {
		workloadType = type;
		return *this;
	}

	https_workload_type getWorkloadType() const {
		return workloadType;
	}

	https_workload_data(https_workload_type type) {
		*this = type;
	}

  protected:
	https_workload_type workloadType{};
};

jsonifier::vector<jsonifier::string_view> tokenize(jsonifier::string_view in, const char* sep = "\r\n") {
	jsonifier::vector<jsonifier::string_view> result{};
	jsonifier::string_view::size_type b = 0;
	jsonifier::string_view::size_type e = 0;
	while ((b = in.findFirstNotOf(sep, e)) != jsonifier::string_view::npos) {
		e = in.findFirstOf(sep, b);
		if (e == jsonifier::string_view::npos) {
			break;
		}
		result.emplace_back(in.substr(b, e - b));
	}
	return result;
}

uint64_t parseCode(jsonifier::string_view string) {
	uint64_t start = string.find(' ');
	if (start == jsonifier::string_view::npos) {
		return 0;
	}

	while (std::isspace(string[start])) {
		start++;
	}

	uint64_t end = start;
	while (std::isdigit(string[end])) {
		end++;
	}
	jsonifier::string_view codeStr = string.substr(start, end - start);
	uint64_t code				   = jsonifier::strToUint64(codeStr.data());
	return code;
}

/// \brief Voice websocket close codes.
class https_response_code {
  public:
	/// \brief Voice websocket close codes.
	enum class https_response_codes : uint64_t {
		Unset				= std::numeric_limits<uint64_t>::max(),
		ok					= 200,///< The request completed successfully.
		created				= 201,///< The entity was created successfully.
		No_Content			= 204,///< The request completed successfully but returned no content.
		Not_Modifies		= 304,///< The entity was not modified (no action was taken).
		Bad_Request			= 400,///< The request was improperly formatted, or the server couldn't understand it.
		unauthorized		= 401,///< The authorization header was missing or invalid.
		forbidden			= 403,///< The authorization token you passed did not have permission to the resource.
		Not_Found			= 404,///< The resource at the location specified doesn't exist.
		Method_Not_Allowed	= 405,///< The https method used is not valid for the location specified.
		Too_Many_Requests	= 429,///< You are being rate limited, see rate limits.
		Gateway_Unavailable = 502,///< There was not a gateway available to process your request. wait a bit and retry.
	};

	inline static std::unordered_map<https_response_codes, jsonifier::string> outputErrorValues{ { static_cast<https_response_codes>(200), "The request completed successfully" },
		{ static_cast<https_response_codes>(201), "The entity was created successfully" },
		{ static_cast<https_response_codes>(204), "The request completed successfully but returned no content" },
		{ static_cast<https_response_codes>(304), "The entity was not modified (no action was taken)" },
		{ static_cast<https_response_codes>(400), "The request was improperly formatted, or the server couldn't understand it" },
		{ static_cast<https_response_codes>(401), "The authorization header was missing or invalid" },
		{ static_cast<https_response_codes>(403), "The authorization token you passed did not have permission to the resource" },
		{ static_cast<https_response_codes>(404), "The resource at the location specified doesn't exist" },
		{ static_cast<https_response_codes>(405), "The https method used is not valid for the location specified" },
		{ static_cast<https_response_codes>(429), "you are being rate limited, see rate limits" },
		{ static_cast<https_response_codes>(502), "There was not a gateway available to process your request.wait a bit and retry" },
		{ static_cast<https_response_codes>(500), "The server had an error processing your request(these are rare)" } };

	https_response_codes value{};

	OPTS_BOT_ALWAYS_INLINE https_response_code() = default;

	OPTS_BOT_ALWAYS_INLINE https_response_code& operator=(uint64_t valueNew) {
		value = static_cast<https_response_codes>(valueNew);
		return *this;
	}

	OPTS_BOT_ALWAYS_INLINE https_response_code(uint64_t value) {
		*this = value;
	}

	OPTS_BOT_ALWAYS_INLINE operator jsonifier::string() {
		return jsonifier::string{ "Code: " + jsonifier::toString(static_cast<uint32_t>(value)) + jsonifier::string{ ", message: " } +
			static_cast<jsonifier::string>(https_response_code::outputErrorValues[value]) };
	}

	OPTS_BOT_ALWAYS_INLINE operator uint64_t() {
		return static_cast<uint64_t>(value);
	}
};

class https_connection_manager;
class https_connection;
class https_rnr_builder;
struct rate_limit_data;

enum class https_state { Collecting_Headers = 0, Collecting_Contents = 1, Collecting_Chunked_Contents = 2, complete = 3 };

class https_error : public std::runtime_error {
  public:
	https_response_code errorCode{};
	OPTS_BOT_ALWAYS_INLINE https_error(const jsonifier::string_view& message, std::source_location location = std::source_location::current())
		: std::runtime_error{ message.data() } {};
};

struct https_response_data {
	friend class https_rnr_builder;
	friend class https_connection;
	friend class https_client;

	https_response_code responseCode{ std::numeric_limits<uint32_t>::max() };
	std::unordered_map<jsonifier::string, jsonifier::string> responseHeaders{};
	https_state currentState{ https_state::Collecting_Headers };
	jsonifier::string responseData{};
	uint64_t contentLength{};

  protected:
	bool isItChunked{};
};

class https_rnr_builder {
  public:
	friend class https_client;

	https_rnr_builder() = default;

	void updateRateLimitData(rate_limit_data& rateLimitData);

	https_response_data finalizeReturnValues(rate_limit_data& rateLimitData);

	jsonifier::string buildRequest(const https_workload_data& workload);

	bool parseHeaders();

	virtual ~https_rnr_builder() = default;

  protected:
	bool parseChunk();

	bool parseContents();
};

class https_connection : public https_rnr_builder, public tcp_connection<https_connection> {
  public:
	template<typename value_type> friend class https_tcp_connection;

	rate_limit_data* currentRateLimitData{};
	const int32_t maxReconnectTries{ 3 };
	jsonifier::string inputBufferReal{};
	jsonifier::string currentBaseUrl{};
	int32_t currentReconnectTries{};
	https_workload_data workload{};
	https_response_data data{};

	https_connection() = default;

	https_connection(const jsonifier::string& baseUrlNew, const uint16_t portNew) : tcp_connection<https_connection>{ baseUrlNew, portNew } {
	}

	void handleBuffer() {
		jsonifier_internal::stop_watch<std::chrono::milliseconds> stopWatch{ 9500 };
		jsonifier::string_base<uint8_t> stringNew{};
		stopWatch.reset();
		do {
			stringNew = getInputBuffer();
			inputBufferReal += stringNew;
			switch (data.currentState) {
				case https_state::Collecting_Headers: {
					if (!parseHeaders()) {
						return;
					}
					break;
				}
				case https_state::Collecting_Contents: {
					if (!parseContents()) {
						return;
					}
					break;
				}
				case https_state::Collecting_Chunked_Contents: {
					if (!parseChunk()) {
						return;
					}
					break;
				}
				case https_state::complete: {
					inputBufferReal.clear();
					return;
				}
			}
		} while (stringNew.size() > 0 && !stopWatch.hasTimeElapsed());
		return;
	}

	bool areWeConnected() {
		return tcp_connection::areWeStillConnected();
	}

	void disconnect() {
		tcp_connection::disconnect();
		tcp_connection::reset();
	}

	void resetValues(https_workload_data&& workloadDataNew, rate_limit_data* rateLimitDataNew) {
		currentRateLimitData = rateLimitDataNew;
		if (currentBaseUrl != workloadDataNew.baseUrl) {
			tcp_connection::reset();
			currentBaseUrl = workloadDataNew.baseUrl;
		}
		workload = std::move(workloadDataNew);
		if (workload.baseUrl == "") {
			workload.baseUrl = "https://discord.com/api/v10";
		}
		inputBufferReal.clear();
		data = https_response_data{};
	}

	virtual ~https_connection() = default;
};
void https_rnr_builder::updateRateLimitData(rate_limit_data& rateLimitData) {
	auto connection{ static_cast<https_connection*>(this) };
	if (connection->data.responseHeaders.contains("x-ratelimit-bucket")) {
		rateLimitData.bucket = connection->data.responseHeaders.at("x-ratelimit-bucket");
	}
	if (connection->data.responseHeaders.contains("x-ratelimit-reset-after")) {
		rateLimitData.sRemain.store(
			std::chrono::seconds{ static_cast<int64_t>(ceil(jsonifier::strToDouble(connection->data.responseHeaders.at("x-ratelimit-reset-after").data()))) },
			std::memory_order_release);
	}
	if (connection->data.responseHeaders.contains("x-ratelimit-remaining")) {
		rateLimitData.getsRemaining.store(static_cast<int64_t>(jsonifier::strToInt64(connection->data.responseHeaders.at("x-ratelimit-remaining").data())),
			std::memory_order_release);
	}
	if (rateLimitData.getsRemaining.load(std::memory_order_acquire) <= 1 || rateLimitData.areWeASpecialBucket.load(std::memory_order_acquire)) {
		rateLimitData.doWeWait.store(true, std::memory_order_release);
	}
}

https_response_data https_rnr_builder::finalizeReturnValues(rate_limit_data& rateLimitData) {
	auto connection{ static_cast<https_connection*>(this) };
	if (connection->data.responseData.size() >= connection->data.contentLength && connection->data.contentLength > 0) {
		connection->data.responseData = connection->data.responseData.substr(0, connection->data.contentLength);
	} else {
		auto pos1 = connection->data.responseData.findFirstOf('{');
		auto pos2 = connection->data.responseData.findLastOf('}');
		auto pos3 = connection->data.responseData.findFirstOf('[');
		auto pos4 = connection->data.responseData.findLastOf(']');
		if (pos1 != jsonifier::string_view::npos && pos2 != jsonifier::string_view::npos && pos1 < pos3) {
			connection->data.responseData = connection->data.responseData.substr(pos1, pos2 + 1);
		} else if (pos3 != jsonifier::string_view::npos && pos4 != jsonifier::string_view::npos) {
			connection->data.responseData = connection->data.responseData.substr(pos3, pos4 + 1);
		}
	}
	updateRateLimitData(rateLimitData);
	if (connection->data.responseCode != 204 && connection->data.responseCode != 200 && connection->data.responseCode != 201) {
		throw std::runtime_error{ std::string{
			"Sorry, but that https request threw the following error: " + connection->data.responseCode.operator jsonifier::string() + connection->data.responseData }
									  .data() };
	}
	return std::move(connection->data);
}

jsonifier::string https_rnr_builder::buildRequest(const https_workload_data& workload) {
	jsonifier::string baseUrlNew{};
	if (workload.baseUrl.find(".com") != jsonifier::string_view::npos) {
		baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + jsonifier::string_view("https://").size(),
			workload.baseUrl.find(".com") + jsonifier::string_view(".com").size() - jsonifier::string_view("https://").size());
	} else if (workload.baseUrl.find(".org") != jsonifier::string_view::npos) {
		baseUrlNew = workload.baseUrl.substr(workload.baseUrl.find("https://") + jsonifier::string_view("https://").size(),
			workload.baseUrl.find(".org") + jsonifier::string_view(".org").size() - jsonifier::string_view("https://").size());
	}
	jsonifier::string returnString{};
	if (workload.workloadClass == https_workload_class::Get || workload.workloadClass == https_workload_class::Delete) {
		if (workload.workloadClass == https_workload_class::Get) {
			returnString += "GET " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
		} else if (workload.workloadClass == https_workload_class::Delete) {
			returnString += "DELETE " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
		}
		for (auto& [key, value]: workload.headersToInsert) {
			returnString += key + ": " + value + "\r\n";
		}
		returnString += "Pragma: no-cache\r\n";
		returnString += "Connection: keep-alive\r\n";
		returnString += "Host: " + baseUrlNew + "\r\n\r\n";
	} else {
		if (workload.workloadClass == https_workload_class::Patch) {
			returnString += "PATCH " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
		} else if (workload.workloadClass == https_workload_class::Post) {
			returnString += "POST " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
		} else if (workload.workloadClass == https_workload_class::Put) {
			returnString = "PUT " + workload.baseUrl + workload.relativePath + " HTTP/1.1\r\n";
		}
		for (auto& [key, value]: workload.headersToInsert) {
			returnString += key + ": " + value + "\r\n";
		}
		returnString += "Pragma: no-cache\r\n";
		returnString += "Connection: keep-alive\r\n";
		returnString += "Host: " + baseUrlNew + "\r\n";
		returnString += "Content-Length: " + jsonifier::toString(workload.content.size()) + "\r\n\r\n";
		returnString += workload.content + "\r\n\r\n";
	}
	return returnString;
}

bool https_rnr_builder::parseHeaders() {
	auto connection{ static_cast<https_connection*>(this) };
	jsonifier::string& stringViewNew = connection->inputBufferReal;
	if (stringViewNew.find("\r\n\r\n") != jsonifier::string_view::npos) {
		auto headers = tokenize(stringViewNew);
		if (headers.size() && (headers.at(0).find("HTTP/1") != jsonifier::string_view::npos)) {
			uint64_t parseCodeNew{};
			try {
				parseCodeNew = parseCode(headers.at(0));
			} catch (const std::invalid_argument& error) {
				std::cout << error.what() << std::endl;
				connection->data.currentState = https_state::complete;
			}
			headers.erase(headers.begin());
			if (headers.size() >= 3 && parseCodeNew) {
				for (uint64_t x = 0; x < headers.size(); ++x) {
					jsonifier::string_view::size_type sep = headers.at(x).find(": ");
					if (sep != jsonifier::string_view::npos) {
						jsonifier::string key		 = static_cast<jsonifier::string>(headers.at(x).substr(0, sep));
						jsonifier::string_view value = headers.at(x).substr(sep + 2, headers.at(x).size());
						for (auto& valueNew: key) {
							valueNew = static_cast<char>(std::tolower(static_cast<int32_t>(valueNew)));
						}
						connection->data.responseHeaders.emplace(key, value);
					}
				}
				connection->data.responseCode = parseCodeNew;
				if (connection->data.responseCode == 302) {
					connection->workload.baseUrl = connection->data.responseHeaders.at("location");
					connection->disconnect();
					return false;
				}
				if (connection->data.responseCode == 204) {
					connection->data.currentState = https_state::complete;
				} else if (connection->data.responseHeaders.contains("content-length")) {
					connection->data.contentLength = jsonifier::strToUint64(connection->data.responseHeaders.at("content-length").data());
					connection->data.currentState  = https_state::Collecting_Contents;
				} else {
					connection->data.isItChunked   = true;
					connection->data.contentLength = std::numeric_limits<uint32_t>::max();
					connection->data.currentState  = https_state::Collecting_Chunked_Contents;
				}
				connection->inputBufferReal.erase(connection->inputBufferReal.begin() + static_cast<int64_t>(stringViewNew.find("\r\n\r\n")) + 4);
			}
		}
		return true;
	}
	return false;
}

bool https_rnr_builder::parseChunk() {
	auto connection{ static_cast<https_connection*>(this) };
	jsonifier::string_view stringViewNew01{ connection->inputBufferReal };
	if (auto finalPosition = stringViewNew01.find("\r\n0\r\n\r\n"); finalPosition != jsonifier::string_view::npos) {
		uint64_t pos{ 0 };
		while (pos < stringViewNew01.size() || connection->data.responseData.size() < connection->data.contentLength) {
			uint64_t lineEnd = stringViewNew01.find("\r\n", pos);
			if (lineEnd == jsonifier::string_view::npos) {
				break;
			}

			jsonifier::string_view sizeLine{ stringViewNew01.data() + pos, lineEnd - pos };
			uint64_t chunkSize = jsonifier::strToUint64<16>(static_cast<jsonifier::string>(sizeLine));
			connection->data.contentLength += chunkSize;

			if (chunkSize == 0) {
				break;
			}

			pos = lineEnd + 2;

			jsonifier::string_view newString{ stringViewNew01.data() + pos, chunkSize };
			connection->data.responseData += newString;
			pos += chunkSize + 2;
		}
		connection->data.currentState = https_state::complete;
		return true;
	}
	return false;
}

bool https_rnr_builder::parseContents() {
	auto connection{ static_cast<https_connection*>(this) };
	if (connection->inputBufferReal.size() >= connection->data.contentLength || !connection->data.contentLength) {
		connection->data.responseData += jsonifier::string_view{ connection->inputBufferReal.data(), connection->data.contentLength };
		connection->data.currentState = https_state::complete;
		return true;
	} else {
		return false;
	}
}

/// @class https_connection_manager.
/// @brief For managing the collection of Https connections.
class https_connection_manager {
  public:
	friend class https_client;

	https_connection_manager() = default;

	https_connection_manager(rate_limit_queue* rateLimitDataQueueNew) {
		rateLimitQueue = rateLimitDataQueueNew;
	}

	rate_limit_queue& getRateLimitQueue() {
		return *rateLimitQueue;
	}

	https_connection& getConnection(https_workload_type workloadType) {
		std::unique_lock lock{ accessMutex };
		if (!httpsConnections.contains(workloadType)) {
			httpsConnections.emplace(workloadType, makeUnique<https_connection>());
		}
		httpsConnections.at(workloadType)->currentReconnectTries = 0;
		return *httpsConnections.at(workloadType).get();
	}

  protected:
	std::unordered_map<https_workload_type, unique_ptr<https_connection>> httpsConnections{};
	rate_limit_queue* rateLimitQueue{};
	std::mutex accessMutex{};
};

class https_connection_stack_holder {
  public:
	https_connection_stack_holder(https_connection_manager& connectionManager, https_workload_data&& workload) {
		connection		   = &connectionManager.getConnection(workload.getWorkloadType());
		rateLimitQueue	   = &connectionManager.getRateLimitQueue();
		auto rateLimitData = connectionManager.getRateLimitQueue().getEndpointAccess(workload.getWorkloadType());
		if (!rateLimitData) {
			throw std::runtime_error{ "Failed to gain endpoint access." };
		}
		connection->resetValues(std::move(workload), rateLimitData);
		if (!connection->areWeConnected()) {
			*static_cast<tcp_connection<https_connection>*>(connection) = https_connection{ connection->workload.baseUrl, static_cast<uint16_t>(443) };
		}
	}

	~https_connection_stack_holder() {
		rateLimitQueue->releaseEndPointAccess(connection->workload.getWorkloadType());
	}

	https_connection& getConnection() {
		return *connection;
	}

  protected:
	rate_limit_queue* rateLimitQueue{};
	https_connection* connection{};
};

class https_client_core {
  public:
	https_client_core(jsonifier::string_view botTokenNew) {
		botToken = botTokenNew;
	}

	OPTS_BOT_ALWAYS_INLINE https_response_data submitWorkloadAndGetResult(https_workload_data&& workloadNew) {
		https_connection connection{};
		rate_limit_data rateLimitData{};
		connection.resetValues(std::move(workloadNew), &rateLimitData);
		auto returnData = httpsRequestInternal(connection);
		if (returnData.responseCode != 200 && returnData.responseCode != 204 && returnData.responseCode != 201) {
			jsonifier::string errorMessage{};
			if (connection.workload.callStack != "") {
				errorMessage += connection.workload.callStack + " ";
			}
			errorMessage += "Https error: " + returnData.responseCode.operator jsonifier::string() + "\nThe request: base url: " + connection.workload.baseUrl + "\n";
			if (!connection.workload.relativePath.empty()) {
				errorMessage += "Relative Url: " + connection.workload.relativePath + "\n";
			}
			if (!connection.workload.content.empty()) {
				errorMessage += "Content: " + connection.workload.content + "\n";
			}
			if (!returnData.responseData.empty()) {
				errorMessage += "The Response: " + static_cast<jsonifier::string>(returnData.responseData);
			}
			https_error theError{ errorMessage };
			theError.errorCode = returnData.responseCode;
		}
		return returnData;
	}

  protected:
	jsonifier::string botToken{};

	https_response_data httpsRequestInternal(https_connection& connection) {
		if (connection.workload.baseUrl == "https://discord.com/api/v10") {
			connection.workload.headersToInsert.emplace("Authorization", "Bot " + botToken);
			connection.workload.headersToInsert.emplace("User-Agent", "DiscordCoreAPI (https://discordcoreapi.com/1.0)");
			if (connection.workload.payloadType == payload_type::Application_Json) {
				connection.workload.headersToInsert.emplace("Content-Type", "application/json");
			} else if (connection.workload.payloadType == payload_type::Multipart_Form) {
				connection.workload.headersToInsert.emplace("Content-Type", "multipart/form-data; boundary=boundary25");
			}
		}
		if (connection.currentReconnectTries >= connection.maxReconnectTries) {
			connection.disconnect();
			return https_response_data{};
		}
		if (!connection.areWeConnected()) {
			connection.currentBaseUrl									 = connection.workload.baseUrl;
			*static_cast<tcp_connection<https_connection>*>(&connection) = https_connection{ connection.workload.baseUrl, static_cast<uint16_t>(443) };
			if (connection.currentStatus != connection_status::NO_Error || !connection.areWeConnected()) {
				++connection.currentReconnectTries;
				connection.disconnect();
				return httpsRequestInternal(connection);
			}
		}
		auto request = connection.buildRequest(connection.workload);
		if (connection.areWeConnected()) {
			connection.writeData(static_cast<jsonifier::string_view>(request), true);
			if (connection.currentStatus != connection_status::NO_Error || !connection.areWeConnected()) {
				++connection.currentReconnectTries;
				connection.disconnect();
				return httpsRequestInternal(connection);
			}
			auto result = getResponse(connection);
			if (static_cast<int64_t>(result.responseCode) == -1 || !connection.areWeConnected()) {
				++connection.currentReconnectTries;
				connection.disconnect();
				return httpsRequestInternal(connection);
			} else {
				return result;
			}
		} else {
			++connection.currentReconnectTries;
			connection.disconnect();
			return httpsRequestInternal(connection);
		}
	}

	https_response_data recoverFromError(https_connection& connection) {
		if (connection.currentReconnectTries >= connection.maxReconnectTries) {
			connection.disconnect();
			return connection.finalizeReturnValues(*connection.currentRateLimitData);
		}
		++connection.currentReconnectTries;
		connection.disconnect();
		std::this_thread::sleep_for(std::chrono::milliseconds{ 150 });
		return httpsRequestInternal(connection);
	}

	https_response_data getResponse(https_connection& connection) {
		while (connection.data.currentState != https_state::complete) {
			if (connection.areWeConnected()) {
				auto newState = connection.processIO(10);
				switch (newState) {
					case connection_status::NO_Error: {
						continue;
					}
					case connection_status::CONNECTION_Error:
						[[fallthrough]];
					case connection_status::POLLERR_Error:
						[[fallthrough]];
					case connection_status::POLLHUP_Error:
						[[fallthrough]];
					case connection_status::POLLNVAL_Error:
						[[fallthrough]];
					case connection_status::READ_Error:
						[[fallthrough]];
					case connection_status::WRITE_Error:
						[[fallthrough]];
					case connection_status::SOCKET_Error:
						[[fallthrough]];
					default: {
						return recoverFromError(connection);
					}
				}
			} else {
				return recoverFromError(connection);
			}
		}
		return connection.finalizeReturnValues(*connection.currentRateLimitData);
	}
};

/**
		 * \addtogroup discord_core_internal
		 * @{
		 */

/// @class https_client
/// @brief For sending Https requests.
class https_client : public https_client_core {
  public:
	https_client(jsonifier::string_view botTokenNew) : https_client_core(botTokenNew), connectionManager(&rateLimitQueue) {
		rateLimitQueue.initialize();
	}

	template<typename value_type, typename string_type> void getParseErrors(jsonifier::jsonifier_core<false>& parser, value_type& value, string_type& stringNew) {
		parser.parseJson(value, parser.minifyJson(parser.prettifyJson(stringNew)));
		if (auto result = parser.getErrors(); result.size() > 0) {
			for (auto& valueNew: result) {
				std::cout << valueNew.reportError() << std::endl;
			}
		}
	}

	template<typename workload_type, typename... args> void submitWorkloadAndGetResult(workload_type&& workload, args&... argsNew) {
		https_connection_stack_holder stackHolder{ connectionManager, std::move(workload) };
		https_response_data returnData = httpsRequest(stackHolder.getConnection());
		if (static_cast<uint32_t>(returnData.responseCode) != 200 && static_cast<uint32_t>(returnData.responseCode) != 204 &&
			static_cast<uint32_t>(returnData.responseCode) != 201) {
			jsonifier::string errorMessage{};
			if (stackHolder.getConnection().workload.callStack != "") {
				errorMessage += stackHolder.getConnection().workload.callStack + " ";
			}
			errorMessage +=
				"Https error: " + returnData.responseCode.operator jsonifier::string() + "\nThe request: base url: " + stackHolder.getConnection().workload.baseUrl + "\n";
			if (!stackHolder.getConnection().workload.relativePath.empty()) {
				errorMessage += "Relative Url: " + stackHolder.getConnection().workload.relativePath + "\n";
			}
			if (!stackHolder.getConnection().workload.content.empty()) {
				errorMessage += "Content: " + stackHolder.getConnection().workload.content + "\n";
			}
			if (!returnData.responseData.empty()) {
				errorMessage += "The Response: " + static_cast<jsonifier::string>(returnData.responseData);
			}
			https_error theError{ errorMessage };
			theError.errorCode = returnData.responseCode;
		}

		if constexpr ((( !std::is_void_v<args> ) || ...)) {
			if (returnData.responseData.size() > 0) {
				//(getParseErrors(parser, argsNew, returnData.responseData), ...);
			}
		}
	}

  protected:
	https_connection_manager connectionManager{};
	rate_limit_queue rateLimitQueue{};

	https_response_data httpsRequest(https_connection& connection) {
		https_response_data resultData = executeByRateLimitData(connection);
		return resultData;
	}

	https_response_data executeByRateLimitData(https_connection& connection) {
		https_response_data returnData{};
		std::chrono::milliseconds timeRemaining{};
		std::chrono::milliseconds currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		if (connection.currentRateLimitData->areWeASpecialBucket.load(std::memory_order_acquire)) {
			connection.currentRateLimitData->sRemain.store(std::chrono::seconds{ static_cast<int64_t>(ceil(4.0f / 4.0f)) }, std::memory_order_release);
			std::chrono::milliseconds targetTime{ connection.currentRateLimitData->sampledTimeInMs.load(std::memory_order_acquire) +
				std::chrono::duration_cast<std::chrono::milliseconds>(connection.currentRateLimitData->sRemain.load(std::memory_order_acquire)) };
			timeRemaining = targetTime - currentTime;
		} else if (connection.currentRateLimitData->doWeWait.load(std::memory_order_acquire)) {
			std::chrono::milliseconds targetTime{ connection.currentRateLimitData->sampledTimeInMs.load(std::memory_order_acquire) +
				std::chrono::duration_cast<std::chrono::milliseconds>(connection.currentRateLimitData->sRemain.load(std::memory_order_acquire)) };
			timeRemaining = targetTime - currentTime;
			connection.currentRateLimitData->doWeWait.store(false, std::memory_order_release);
		}
		if (timeRemaining.count() > 0) {
			std::cout << "we're waiting on rate-limit: " + jsonifier::toString(timeRemaining.count()) << std::endl;
			std::chrono::milliseconds targetTime{ currentTime + timeRemaining };
			while (targetTime > currentTime && targetTime.count() > 0 && currentTime.count() > 0 && timeRemaining.count() > 0) {
				currentTime	  = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
				timeRemaining = targetTime - currentTime;
				if (timeRemaining.count() <= 20) {
					continue;
				} else {
					std::this_thread::sleep_for(std::chrono::milliseconds{ static_cast<int64_t>(static_cast<double>(timeRemaining.count()) * 80.0f / 100.0f) });
				}
			}
		}
		returnData = httpsRequestInternal(connection);
		connection.currentRateLimitData->sampledTimeInMs.store(
			std::chrono::duration_cast<std::chrono::duration<int64_t, std::milli>>(std::chrono::system_clock::now().time_since_epoch()), std::memory_order_release);

		if (returnData.responseCode == 204 || returnData.responseCode == 201 || returnData.responseCode == 200) {
			std::cout << connection.workload.callStack + " success: " + static_cast<jsonifier::string>(returnData.responseCode) + ": " + returnData.responseData << std::endl;
		} else if (returnData.responseCode == 429) {
			if (connection.data.responseHeaders.contains("x-ratelimit-retry-after")) {
				connection.currentRateLimitData->sRemain.store(
					std::chrono::seconds{ jsonifier::strToInt64(connection.data.responseHeaders.at("x-ratelimit-retry-after").data()) / 1000LL }, std::memory_order_release);
			}
			connection.currentRateLimitData->doWeWait.store(true, std::memory_order_release);
			connection.currentRateLimitData->sampledTimeInMs.store(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()),
				std::memory_order_release);
			std::cout << connection.workload.callStack + "::httpsRequest(), we've hit rate limit! time remaining: " +
					jsonifier::toString(connection.currentRateLimitData->sRemain.load(std::memory_order_acquire).count())
					  << std::endl;
			connection.resetValues(std::move(connection.workload), connection.currentRateLimitData);
			returnData = executeByRateLimitData(connection);
		}
		return returnData;
	};
};