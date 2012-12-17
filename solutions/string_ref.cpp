#include <utility>
#include <iostream>
#include <array>
#include <cassert>
#include <cstring>

template <class C>
class basic_string_ref
{
public:

	typedef C value_type;
	typedef C &reference;
	typedef C *iterator;
	typedef typename std::add_const<C>::type *const_iterator;
	typedef std::size_t size_type;
	
	basic_string_ref();
	basic_string_ref(iterator begin, iterator end);
	
	template <std::size_t N>
	basic_string_ref(value_type (&data)[N]);
	
	basic_string_ref(iterator c_string)
		: m_begin(c_string)
		, m_end(c_string + std::strlen(c_string))
	{
	}
	
	size_type size() const
	{
		return std::distance(m_begin, m_end);
	}
	
	const_iterator data() const
	{
		return m_begin;
	}
	
	bool empty() const;
	iterator begin();
	const_iterator begin() const
	{
		return m_begin;
	}
	
	iterator end();
	const_iterator end() const
	{
		return m_end;
	}
	//...
	
private:

	iterator m_begin;
	iterator m_end;
};

template <class C>
std::basic_ostream<typename std::remove_const<C>::type> &operator << (
std::basic_ostream<typename std::remove_const<C>::type> &os, basic_string_ref<C> const &str)
{
	std::copy(str.begin(), str.end(),
		std::ostreambuf_iterator<typename std::remove_const<C>::type>(os));
	return os;
}

typedef basic_string_ref<char> string_ref;
typedef basic_string_ref<const char> cstring_ref;

namespace
{
	const std::array<cstring_ref, 2> emlist =
	{{
		"foo",
		"bar"
	}};
}

cstring_ref const &errormsg(unsigned errorcode)
{
	assert(errorcode < emlist.size());
	return emlist[errorcode];
}

int main()
{
	for (auto i = emlist.begin(); i != emlist.end(); ++i)
	{
		std::cout << *i << '\n';
	}
}
