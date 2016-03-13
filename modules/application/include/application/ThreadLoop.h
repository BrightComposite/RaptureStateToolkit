//---------------------------------------------------------------------------

#ifndef THREAD_LOOP_H
#define THREAD_LOOP_H

//---------------------------------------------------------------------------

#include <core/addition/Singleton.h>
#include <core/container/Array.h>
#include <core/action/Action.h>
#include <chrono>

//---------------------------------------------------------------------------

namespace Rapture
{
	using namespace std::literals;

	class ThreadLoop : public Singleton<ThreadLoop, ThreadLocalModel>
	{
		typedef function<int()> Iteration;
		friend Singleton<ThreadLoop, ThreadLocalModel>;

	public:
		static void run()
		{
			auto & loop = instance();

			if(loop.active)
				throw std::exception("ThreadLoop is already running!");

			loop.active = true;

			while(true)
			{
				auto & list = loop.iterations;

				if(list.empty())
				{
					loop.active = false;
					return;
				}

				for(auto & it = list.begin(); it != list.end(); ++it)
				{
					int result = (*it)();

					switch(result)
					{
					case 1:
						it = list.erase(it);

						if(it == list.end())
							break;
						break;

					case 2:
						loop.active = false;
						return;
					}

					if(!loop.active)
						return;
				}
			}
		}

		static bool isActive()
		{
			return instance().active;
		}

		static void stop()
		{
			instance().active = false;
		}

		static void add(const Iteration & iteration)
		{
			instance().iterations.emplace_back(iteration);
		}

		static void add(Iteration && iteration)
		{
			instance().iterations.emplace_back(forward<Iteration>(iteration));
		}

	protected:
		ThreadLoop() {}
		ThreadLoop(const ThreadLoop &) = delete;
		~ThreadLoop() {}

		ThreadLoop & operator = (const ThreadLoop &) = delete;

		vector<Iteration> iterations;
		bool active = false;
	};
}

//---------------------------------------------------------------------------
#endif