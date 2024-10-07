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
/// rate_limit_queue.hpp - Header file for the "rate_limit_queue stuff".
/// May 12, 2021
/// https://discordcoreapi.com
/// \file rate_limit_queue.hpp
#pragma once

#include <jsonifier/Index.hpp>
#include <tcp_connection.hpp>
#include <unique_ptr.hpp>
#include <mutex>

enum class https_workload_type : uint8_t {
	Unset											= 0,
	Last											= 172
};

struct rate_limit_data {
	friend class https_connection_stack_holder;
	friend class https_connection_manager;
	friend class rate_limit_stack_holder;
	friend class https_rnr_builder;
	friend class rate_limit_queue;
	friend class https_client;

  protected:
	std::unique_lock<std::mutex> lock{ accessMutex, std::defer_lock };
	std::atomic<std::chrono::milliseconds> sampledTimeInMs{ std::chrono::milliseconds{} };
	std::atomic<std::chrono::seconds> sRemain{ std::chrono::seconds{} };
	std::atomic_int64_t getsRemaining{ 1 };
	std::atomic_bool areWeASpecialBucket{};
	std::atomic_bool didWeHitRateLimit{};
	std::atomic_bool doWeWait{};
	std::string bucket{};
	std::mutex accessMutex{};
};

class rate_limit_queue {
  public:
	friend class https_client;

	OPTS_BOT_ALWAYS_INLINE rate_limit_queue() = default;

	OPTS_BOT_INLINE void initialize() {
		for (int64_t enumOne = static_cast<int64_t>(https_workload_type::Unset); enumOne != static_cast<int64_t>(https_workload_type::Last); enumOne++) {
			auto tempBucket = jsonifier::toString(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
			buckets.emplace(static_cast<https_workload_type>(enumOne), tempBucket);
			rateLimits.emplace(tempBucket, makeUnique<rate_limit_data>())
				.first->second->sampledTimeInMs.store(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()));
			std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
		}
	}

	OPTS_BOT_INLINE rate_limit_data* getEndpointAccess(https_workload_type workloadType) {
		jsonifier_internal::stop_watch<std::chrono::milliseconds> stopWatch{ std::chrono::milliseconds{ 25000 } };
		stopWatch.reset();
		auto targetTime =
			std::chrono::duration_cast<std::chrono::duration<int64_t, std::milli>>(rateLimits[buckets[workloadType]]->sampledTimeInMs.load(std::memory_order_acquire)) +
			std::chrono::duration_cast<std::chrono::duration<int64_t, std::milli>>(rateLimits[buckets[workloadType]]->sRemain.load(std::memory_order_acquire));
		if (rateLimits[buckets[workloadType]]->getsRemaining.load(std::memory_order_acquire) <= 0) {
			auto newNow = std::chrono::duration_cast<std::chrono::duration<int64_t, std::milli>>(std::chrono::system_clock::now().time_since_epoch());
			while ((newNow - targetTime).count() <= 0) {
				if (stopWatch.hasTimeElapsed()) {
					return nullptr;
				}
				newNow = std::chrono::duration_cast<std::chrono::duration<int64_t, std::milli>>(std::chrono::system_clock::now().time_since_epoch());
				std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			}
		}
		stopWatch.reset();
		while (!rateLimits[buckets[workloadType]]->accessMutex.try_lock()) {
			std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });
			if (stopWatch.hasTimeElapsed()) {
				return nullptr;
			}
		}
		return rateLimits.at(buckets.at(workloadType)).get();
	}

	OPTS_BOT_INLINE void releaseEndPointAccess(https_workload_type type) {
		rateLimits.at(buckets.at(type))->accessMutex.unlock();
	}

  protected:
	std::unordered_map<std::string, unique_ptr<rate_limit_data>> rateLimits{};
	std::unordered_map<https_workload_type, std::string> buckets{};
};
