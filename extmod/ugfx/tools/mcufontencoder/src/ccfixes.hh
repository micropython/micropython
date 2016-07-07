
#ifdef NEED_STRING_FIXES
	#include <string>
	#include <sstream>
	#include <stdexcept>
	#include <limits>
	#include <cstdlib>

	namespace std {
		template <typename T> inline std::string to_string(T value)
		{
			std::ostringstream os ;
			os << value ;
			return os.str() ;
		}

		inline int stoi( const std::string& str, std::size_t* pos = 0, int base = 10 )
		{
			const char* begin = str.c_str() ;
			char* end = nullptr ;
			long value = std::strtol( begin, &end, base ) ;

			if( errno == ERANGE || value > std::numeric_limits<int>::max() )
				throw std::out_of_range( "stoi: out ofrange" ) ;

			if( end == str.c_str() )
				throw std::invalid_argument( "stoi: invalid argument" ) ;

			if(pos) *pos = end - begin ;

			return value ;
		}
	}
#endif

#ifdef NEED_THREAD_FIXES
#ifndef WIN32STDTHREAD_H
	#define WIN32STDTHREAD_H

	#include <windows.h>
	#include <functional>
	#include <memory>
	#include <chrono>
	#include <system_error>
	#include <process.h>

	#define _STD_THREAD_INVALID_HANDLE 0
	namespace std
	{


		class thread
		{
		public:
			class id
			{
				DWORD mId;
				void clear() {mId = 0;}
				friend class thread;
			public:
				id(DWORD aId=0):mId(aId){}
				bool operator==(const id& other) const {return mId == other.mId;}
			};
		protected:
			HANDLE mHandle;
			id mThreadId;
		public:
			typedef HANDLE native_handle_type;
			id get_id() const noexcept {return mThreadId;}
			native_handle_type native_handle() const {return mHandle;}
			thread(): mHandle(_STD_THREAD_INVALID_HANDLE){}
			thread(thread& other)
			:mHandle(other.mHandle), mThreadId(other.mThreadId)
			{
				other.mHandle = _STD_THREAD_INVALID_HANDLE;
				other.mThreadId.clear();
			}
			template<class Function, class... Args>
			explicit thread(Function&& f, Args&&... args)
			{
				typedef decltype(std::bind(f, args...)) Call;
				Call* call = new Call(std::bind(f, args...));
				mHandle = (HANDLE)_beginthreadex(NULL, 0, threadfunc<Call>,
					(LPVOID)call, 0, (unsigned*)&(mThreadId.mId));
			}
			template <class Call>
			static unsigned int __stdcall threadfunc(void* arg)
			{
				std::unique_ptr<Call> upCall(static_cast<Call*>(arg));
				(*upCall)();
				return (unsigned long)0;
			}
			bool joinable() const {return mHandle != _STD_THREAD_INVALID_HANDLE;}
			void join()
			{
				if (get_id() == GetCurrentThreadId())
					throw system_error(EDEADLK, generic_category());
				if (mHandle == _STD_THREAD_INVALID_HANDLE)
					throw system_error(ESRCH, generic_category());
				if (!joinable())
					throw system_error(EINVAL, generic_category());
				WaitForSingleObject(mHandle, INFINITE);
				CloseHandle(mHandle);
				mHandle = _STD_THREAD_INVALID_HANDLE;
				mThreadId.clear();
			}

			~thread()
			{
				if (joinable())
					std::terminate();
			}
			thread& operator=(const thread&) = delete;
			thread& operator=(thread&& other) noexcept
			{
				if (joinable())
				  std::terminate();
				swap(std::forward<thread>(other));
				return *this;
			}
			void swap(thread&& other) noexcept
			{
				std::swap(mHandle, other.mHandle);
				std::swap(mThreadId.mId, other.mThreadId.mId);
			}
			static unsigned int hardware_concurrency() noexcept {return 1;}
			void detach()
			{
				if (!joinable())
					throw system_error();
				mHandle = _STD_THREAD_INVALID_HANDLE;
				mThreadId.clear();
			}
		};
		namespace this_thread
		{
			inline thread::id get_id() {return thread::id(GetCurrentThreadId());}
			inline void yield() {Sleep(0);}
			template< class Rep, class Period >
			void sleep_for( const std::chrono::duration<Rep,Period>& sleep_duration)
			{
				Sleep(chrono::duration_cast<chrono::milliseconds>(sleep_duration).count());
			}
			template <class Clock, class Duration>
			void sleep_until(const std::chrono::time_point<Clock,Duration>& sleep_time)
			{
				sleep_for(sleep_time-Clock::now());
		   }
		}
	}
#endif
#endif
