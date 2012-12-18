#include <vector>
#include <cassert>
#include <iostream>

template <class T>
struct linear_queue
{
	typedef T value_type;
	typedef T &reference;
	typedef T const &const_reference;
	typedef std::size_t size_type;
	
	linear_queue()
		: m_begin(0)
		, m_end(0)
	{
	}
	
	linear_queue(std::initializer_list<T> init)
		: m_entries(init)
		, m_begin(0)
		, m_end(m_entries.size())
	{
	}
	
	size_type size() const
	{
		return (m_begin > m_end) ?
			(m_entries.size() - m_begin + m_end) :
			(m_end - m_begin);
	}
	
	template <class V>
	void push(V &&element)
	{
		if (size() == m_entries.size())
		{
			m_entries.insert(
				m_entries.begin() + m_end,
				std::forward<V>(element));
			++m_end;
		}
		else
		{
			auto const new_index = (m_end % m_entries.size());
			m_entries[new_index] = std::forward<V>(element);
			m_end = new_index + 1;
			
			m_begin %= m_entries.size();
		}
		
		assert(!empty());
	}
	
	void pop()
	{
		assert(!empty());
		++m_begin;
	}
	
	const_reference front() const
	{
		return m_entries[m_begin];
	}
	
	bool empty() const
	{
		return (m_begin == m_end);
	}
	
private:

	std::vector<T> m_entries;
	size_type m_begin, m_end;
};

int main()
{
	linear_queue<int> q;
	assert(q.empty());
	assert(q.size() == 0);
	q.push(1);
	assert(!q.empty());
	assert(q.size() == 1);
	assert(q.front() == 1);
	q.push(2);
	assert(!q.empty());
	assert(q.size() == 2);
	assert(q.front() == 1);
	q.pop();
	assert(!q.empty());
	assert(q.size() == 1);
	assert(q.front() == 2);
	q.pop();
	assert(q.empty());
	assert(q.size() == 0);
	
	for (int i = 0; i < 1000; ++i)
	{
		q.push(i);
		assert(q.size() == static_cast<std::size_t>(i + 1));
		assert(!q.empty());
		assert(q.front() == 0);
	}
	
	for (int i = 0; !q.empty(); ++i)
	{
		assert(q.front() == i);
		q.pop();
	}
	
	assert(q.empty());
	
	linear_queue<int> p = {1, 2, 3};
	assert(!p.empty());
	assert(p.size() == 3);
	assert(p.front() == 1);
	p.pop();
	assert(!p.empty());
	assert(p.size() == 2);
	assert(p.front() == 2);
	p.pop();
	assert(!p.empty());
	assert(p.size() == 1);
	assert(p.front() == 3);
	p.pop();
	assert(p.empty());
	assert(p.size() == 0);
	
	q = p;
	assert(q.empty());
	assert(q.size() == 0);
	
	p = {0};
	assert(!p.empty());
	assert(p.size() == 1);
	assert(p.front() == 0);
	p.pop();
	assert(p.empty());
	assert(p.size() == 0);
}
