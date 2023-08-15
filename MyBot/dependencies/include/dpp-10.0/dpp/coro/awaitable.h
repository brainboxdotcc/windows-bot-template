/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
 * Copyright 2022 Craig Edwards and D++ contributors
 * (https://github.com/brainboxdotcc/DPP/graphs/contributors)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************************/

#ifdef DPP_CORO
#pragma once

#include "coro.h"

#include <functional>
#include <utility>
#include <type_traits>
#include <optional>

namespace dpp {

struct confirmation_callback_t;

/**
 * @brief A co_await-able object handling an API call.
 *
 * @remark - The coroutine may be resumed in another thread, do not rely on thread_local variables.
 * @warning - This feature is EXPERIMENTAL. The API may change at any time and there may be bugs. Please report any to <a href="https://github.com/brainboxdotcc/DPP/issues">GitHub issues</a> or to the <a href="https://discord.gg/dpp">D++ Discord server</a>.
 * @tparam R The return type of the API call. Defaults to confirmation_callback_t
 */
template <typename R>
struct awaitable {
	/**
	 * @brief Alias for the request wrapped in a callable object.
	 */
	using request_t = std::function<void(std::function<void(R)>)>;

	/**
	 * @brief Callable object that will be responsible for the API call when co_await-ing.
	 */
	request_t request;

	/**
	 * @brief Construct an awaitable object from a callable. This can be used to manually wrap an async call.
	 *
	 * Callable should be an invokeable object, taking a parameter that is the callback to be passed to the async call.
	 * For example : `[cluster, message](auto &&cb) { cluster->message_create(message, cb); }
	 *
	 * @warning This callback is to be executed <b>later</b>, on co_await. <a href="/lambdas-and-locals.html">Be mindful of reference captures</a>.
	 */
	explicit awaitable(std::invocable<std::function<void(R)>> auto &&fun) : request{fun} {}

	/**
	 * @brief Copy constructor.
	 */
	awaitable(const awaitable&) = default;

	/**
	 * @brief Move constructor.
	 */
	awaitable(awaitable&&) noexcept = default;

	/**
	 * @brief Copy assignment operator.
	 */
	awaitable& operator=(const awaitable&) = default;

	/**
	 * @brief Move assignment operator.
	 */
	awaitable& operator=(awaitable&&) noexcept = default;

	/**
	 * @brief Awaitable object returned by operator co_await.
	 *
	 * @warning Do not use this directly, it is made to work with co_await.
	 */
	struct awaiter {
		/**
		 * @brief Reference to the callable object that will be responsible for the API call when co_await-ing.
		 */
		const request_t& fun;

		/**
		 * @brief Optional containing the result of the API call.
		 */
		std::optional<R> result = std::nullopt;

		/**
		 * @brief First function called by the standard library when this object is co_await-ed. Returns whether or not we can skip suspending the caller.
		 *
		 * @return false Always return false, we send the API call on suspend.
		 */
		bool await_ready() const noexcept {
			return false;
		}

		/**
		 * @brief Second function called by the standard library when this object is co_await-ed. Suspends and sends the API call.
		 */
		template <typename T>
		void await_suspend(detail::std_coroutine::coroutine_handle<T> caller) noexcept(noexcept(std::invoke(fun, std::declval<std::function<void(R)>&&>()))) {
			std::invoke(fun, [this, caller] <typename R_> (R_&& api_result) mutable {
				result = std::forward<R_>(api_result);
				caller.resume();
			});
		}

		/**
		 * @brief Function called by the standard library when the handle is resumed. Returns the API result as an rvalue.
		 */
		R await_resume() {
			return *std::exchange(result, std::nullopt);
		}
	};

	/**
	 * @brief Overload of co_await for this object, the caller is suspended and the API call is executed. On completion the whole co_await expression evaluates to the result of the API call as an rvalue.
	 *
	 * In contrast with dpp::async, it is fine to co_await this object more than once.
	 */
	auto operator co_await() const noexcept {
		return awaiter{request};
	}

};

} // namespace dpp

#endif /* DPP_CORO */
