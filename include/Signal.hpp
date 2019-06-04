#pragma once

#include <vector>
#include <functional>

namespace Framework
{
	template<typename> class CSignal;
	template<class T, class... Args>
	class CSignal<T (Args...)>
	{

	public:
		typedef std::function<T (Args...)> CSignalFunction;
		typedef std::shared_ptr<CSignalFunction> CSignalFunctionPtr;

		CSignal() = default;

		void connect(const CSignalFunction& func)
		{
			if(func)
				m_connections.emplace_back(std::make_shared<CSignalFunction>(func), false);
		}

		void connectOnce(const CSignalFunction& func)
		{
			if(func)
				m_connections.emplace_back(std::make_shared<CSignalFunction>(func), true);
		}

		void operator()(Args... args)
		{
			m_connections.erase(
				std::remove_if(
					m_connections.begin(), 
					m_connections.end(),
					[&](std::pair<CSignalFunctionPtr, bool> item)
					{
						CSignalFunctionPtr connection = item.first;
						bool oneShot = item.second;
						if(connection)
							(*connection)(args...);
						return !connection || oneShot;
					}
				), 
				m_connections.end()
			);
		}

	private:
		std::vector<std::pair<CSignalFunctionPtr, bool>> m_connections;

	};
}