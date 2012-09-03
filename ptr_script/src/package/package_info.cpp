#include "package_info.hpp"


namespace ptrs
{
	package_info::package_info(
		const guid &id,
		std::string title,
		dependency_set dependencies
		)
		: m_id(id)
		, m_title(std::move(title))
		, m_dependencies(std::move(dependencies))
	{
	}

	package_info::package_info()
	{
	}

	const guid &package_info::id() const
	{
		return m_id;
	}

	const std::string &package_info::title() const
	{
		return m_title;
	}

	const dependency_set &package_info::dependencies() const
	{
		return m_dependencies;
	}


	bool operator == (const package_info &left, const package_info &right)
	{
		return
			(left.id() == right.id()) &&
			(left.title() == right.title()) &&
			(left.dependencies() == right.dependencies());
	}


	std::ostream &operator << (std::ostream &os, const package_info &p)
	{
		os << "v0 " << p.id() << "\n"
			<< p.title() << "\n"
			;

		auto &deps = p.dependencies();
		for (auto i = deps.begin(); i != deps.end(); ++i)
		{
			os << *i << "\n";
		}

		return os << "\n";
	}

	std::istream &operator >> (std::istream &is, package_info &p)
	{
		std::string version;
		is >> version;

		if (version != "v0")
		{
			is.setf(std::ios::failbit);
			return is;
		}

		guid id;
		std::string title;
		dependency_set deps;

		is >> id;
		is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (!getline(is, title))
		{
			return is;
		}
		
		try
		{
			std::string dependency;
			while (getline(is, dependency) &&
				!dependency.empty())
			{
				deps.insert(guid(dependency));
			}
		}
		catch (const invalid_guid_error &)
		{
			is.setf(std::ios::failbit);
			return is;
		}

		p = package_info(id, std::move(title), std::move(deps));
		return is;
	}
}
