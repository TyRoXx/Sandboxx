#ifndef SHARED_HPP
#define SHARED_HPP


#include <memory>


namespace dungeons
{
	template <class T, class ...Args>
	std::unique_ptr<T> make_unique(Args &&...args)
	{
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}
}


#endif
