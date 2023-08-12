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

#include <optional>
#include <type_traits>
#include <exception>
#include <utility>
#include <type_traits>

namespace dpp {

namespace detail {
	template <typename R>
	struct coroutine_promise;

	template <typename R>
	/**
	 * @brief Alias for the coroutine_handle of a coroutine.
	 */
	using coroutine_handle = std_coroutine::coroutine_handle<coroutine_promise<R>>;

} // namespace detail

/**
 * @brief Base type for a coroutine, starts on co_await.
 *
 * @warning - This feature is EXPERIMENTAL. The API may change at any time and there may be bugs. Please report any to <a href="https://github.com/brainboxdotcc/DPP/issues">GitHub issues</a> or to the <a href="https://discord.gg/dpp">D++ Discord server</a>.
 * @warning - Using co_await on this object more than once is undefined behavior.
 * @tparam R Return type of the coroutine. Can be void, or a complete object that supports move construction and move assignment.
 */
template <typename R>
class coroutine {
	/**
	 * @brief Promise has friend access for the constructor
	 */
	friend struct detail::coroutine_promise<R>;

	/**
	 * @brief Coroutine handle.
	 */
	detail::coroutine_handle<R> handle{nullptr};

	/**
	 * @brief Construct from a handle. Internal use only.
	 */
	coroutine(detail::coroutine_handle<R> h) : handle{h} {}

public:
	/**
	 * @brief Default constructor, creates an empty coroutine.
	 */
	coroutine() = default;

	/**
	 * @brief Copy constructor is disabled
	 */
	coroutine(const coroutine &) = delete;

	/**
	 * @brief Move constructor, grabs another coroutine's handle
	 *
	 * @param other Coroutine to move the handle from
	 */
	coroutine(coroutine &&other) noexcept : handle(std::exchange(other.handle, nullptr)) {}

	/**
	 * @brief Destructor, destroys the handle.
	 */
	~coroutine() {
		if (handle)
			handle.destroy();
	}

	/**
	 * @brief Copy assignment is disabled
	 */
	coroutine &operator=(const coroutine &) = delete;

	/**
	 * @brief Move assignment, grabs another coroutine's handle
	 *
	 * @param other Coroutine to move the handle from
	 */
	coroutine &operator=(coroutine &&other) noexcept {
		handle = std::exchange(other.handle, nullptr);
		return (*this);
	}

	/**
	 * @brief First function called by the standard library when the task is co_await-ed.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @return true Always suspend, we need to start the coroutine.
	 */
	bool await_ready() const noexcept {
		assert(handle && "cannot co_await an empty coroutine");
		assert(!handle.done() && "cannot co_await a finished coroutine");
		return (false);
	}

	/**
	 * @brief Second function called by the standard library when the task is co_await-ed.
	 *
	 * Stores the calling coroutine in the promise to resume when this task suspends.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @param caller The calling coroutine, now suspended
	 */
	template <typename T>
	void await_suspend(detail::std_coroutine::coroutine_handle<T> caller) {
		if constexpr (requires (T t) { t.is_sync = false; })
			caller.promise().is_sync = false;
		handle.promise().parent = caller;
		handle.resume();
	}

	/**
	 * @brief Function called by the standard library when the coroutine is resumed.
	 *
	 * @remark Do not call this manually, use the co_await keyword instead.
	 * @throw Throws any exception thrown or uncaught by the coroutine
	 * @return R The result of the coroutine. It is given to the caller as a result to `co_await`
	 */
	R await_resume() {
		if (handle.promise().exception)
			std::rethrow_exception(handle.promise().exception);
		if constexpr (!std::is_same_v<R, void>)
			return std::move(handle.promise().result).value();
	}
};

namespace detail {
	template <typename R>
	struct coroutine_final_awaiter;
	
	/**
	 * @brief Promise type for coroutine.
	 */
	template <typename R>
	struct coroutine_promise {
		/**
		 * @brief Handle of the coroutine co_await-ing this coroutine.
		 */
		std_coroutine::coroutine_handle<> parent{nullptr};

		/**
		 * @brief Return value of the coroutine
		 */
		std::optional<R> result{};

		/**
		 * @brief Pointer to an uncaught exception thrown by the coroutine
		 */
		std::exception_ptr exception{nullptr};

		/**
		 * @brief Function called by the standard library when reaching the end of a coroutine
		 *
		 * @return coroutine_final_awaiter<R> Resumes any coroutine co_await-ing on this
		 */
		coroutine_final_awaiter<R> final_suspend() const noexcept;

		/**
		 * @brief Function called by the standard library when the coroutine start
		 *
		 * @return suspend_always Always suspend at the start, for a lazy start
		 */
		std_coroutine::suspend_always initial_suspend() const noexcept {
			return {};
		}

		/**
		 * @brief Function called when an exception escapes the coroutine
		 *
		 * Stores the exception to throw to the co_await-er
		 */
		void unhandled_exception() noexcept {
			exception = std::current_exception();
		}

		/**
		 * @brief Function called when a value is returned
		 *
		 * Stores the value to return to the co_await-er
		 */
		void return_value(auto &&expr) noexcept(std::is_nothrow_assignable_v<std::optional<R>, decltype(expr)>) requires(std::is_assignable_v<std::optional<R>, decltype(expr)>) {
			result = expr;
		}

		/**
		 * @brief Function called to get the coroutine object
		 */
		coroutine<R> get_return_object() {
			return coroutine<R>{coroutine_handle<R>::from_promise(*this)};
		}
	};

	/**
	 * @brief Struct returned by a coroutine's final_suspend, resumes the continuation
	 */
	template <typename R>
	struct coroutine_final_awaiter {
		/**
		 * @brief First function called by the standard library when reaching the end of a coroutine
		 *
		 * @return false Always return false, we need to suspend
		 */
		bool await_ready() const noexcept {
			return (false);
		}

		/**
		 * @brief Second function called by the standard library when reaching the end of a coroutine.
		 * 
		 * @return std::coroutine_handle<> Coroutine handle to resume, this is either the parent if present or std::noop_coroutine()
		 */
		std_coroutine::coroutine_handle<> await_suspend(std_coroutine::coroutine_handle<coroutine_promise<R>> handle) const noexcept {
			auto parent = handle.promise().parent;

			return parent ? parent : std_coroutine::noop_coroutine();
		}

		/**
		 * @brief Function called by the standard library when this object is resumed
		 */
		void await_resume() const noexcept {}
	};

	template <typename R>
	coroutine_final_awaiter<R> coroutine_promise<R>::final_suspend() const noexcept {
		return {};
	}
	

	/**
	 * @brief Struct returned by a coroutine's final_suspend, resumes the continuation
	 */
	template <>
	struct coroutine_promise<void> {
		/**
		 * @brief Handle of the coroutine co_await-ing this coroutine.
		 */
		std_coroutine::coroutine_handle<> parent{nullptr};

		/**
		 * @brief Pointer to an uncaught exception thrown by the coroutine
		 */
		std::exception_ptr exception{nullptr};

		/**
		 * @brief Function called by the standard library when reaching the end of a coroutine
		 *
		 * @return coroutine_final_awaiter<R> Resumes any coroutine co_await-ing on this
		 */
		coroutine_final_awaiter<void> final_suspend() const noexcept {
			return {};
		}

		/**
		 * @brief Function called by the standard library when the coroutine start
		 *
		 * @return suspend_always Always suspend at the start, for a lazy start
		 */
		std_coroutine::suspend_always initial_suspend() const noexcept {
			return {};
		}

		/**
		 * @brief Function called when an exception escapes the coroutine
		 *
		 * Stores the exception to throw to the co_await-er
		 */
		void unhandled_exception() noexcept {
			exception = std::current_exception();
		}

		/**
		 * @brief Function called when co_return is used
		 */
		void return_void() const noexcept {}
		

		/**
		 * @brief Function called to get the coroutine object
		 */
		coroutine<void> get_return_object() {
			return coroutine<void>{coroutine_handle<void>::from_promise(*this)};
		}
	};

} // namespace detail

} // namespace dpp

/**
 * @brief Specialization of std::coroutine_traits, helps the standard library figure out a promise type from a coroutine function.
 */
template<typename R, typename... Args>
struct dpp::detail::std_coroutine::coroutine_traits<dpp::coroutine<R>, Args...> {
	using promise_type = dpp::detail::coroutine_promise<R>;
};

#endif /* DPP_CORO */
