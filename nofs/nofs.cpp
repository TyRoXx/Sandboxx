#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <functional>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <array>

namespace nofs
{
	typedef std::array<std::uint32_t, 5> sha1_digest;

	template <class InputIterator>
	typename std::enable_if<sizeof(typename std::iterator_traits<InputIterator>::value_type) == 4, sha1_digest>::type
	make_sha1_digest(InputIterator begin)
	{
		sha1_digest result;
		std::copy(begin, std::next(begin, 5), result.begin());
		return result;
	}

	sha1_digest make_sha1_digest(const boost::uuids::uuid &uuid)
	{
		sha1_digest result;
		throw std::runtime_error("not implemented");
	}

	sha1_digest parse_sha1_digest(const std::string &source)
	{
		return make_sha1_digest(boost::uuids::string_generator()(source));
	}

	std::string format_digest(const sha1_digest &digest)
	{
		std::ostringstream formatter;
		formatter.width(8);
		formatter.fill('0');
		formatter << std::hex << std::right;
		std::copy(std::begin(digest),
		          std::end(digest),
		          std::ostream_iterator<std::uint32_t>(formatter));
		return formatter.str();
	}

	void add_file(const boost::filesystem::path &cache, char const *begin, char const *end)
	{
		boost::filesystem::create_directories(cache);
		boost::uuids::detail::sha1 hash_state;
		hash_state.process_block(begin, end);
		unsigned digest[5];
		hash_state.get_digest(digest);
		const auto file_name = format_digest(make_sha1_digest(std::begin(digest)));
		const auto full_path = cache / file_name;
		std::ofstream file(full_path.string(), std::ios::binary);
		if (!file)
		{
			throw std::runtime_error("Could not write file " + full_path.string());
		}
		file.write(begin, std::distance(begin, end));
	}

	void cat_file(const boost::filesystem::path &cache, const sha1_digest &digest, std::ostream &out)
	{
		const auto full_name = (cache / format_digest(digest)).string();
		std::ifstream file(full_name, std::ios::binary);
		if (!file)
		{
			throw std::runtime_error("Could not read file " + full_name);
		}
		out << file.rdbuf();
	}
}

int main(int argc, char **argv)
{
	namespace po = boost::program_options;

	std::vector<std::string> added_files, cat_files;

	po::options_description desc("nofs options");
	desc.add_options()
	    ("help", "produce help message")
	    ("add", po::value(&added_files), "")
	    ("cat", po::value(&cat_files), "")
	;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
	    std::cerr << desc << "\n";
	    return 1;
	}

	const boost::filesystem::path cache = ".nofs";

	for (const auto &added_file : added_files)
	{
		boost::iostreams::mapped_file_params params;
		params.path = added_file;
		boost::iostreams::stream<boost::iostreams::mapped_file_source> file((params));
		nofs::add_file(cache, file->data(), file->data() + file->size());
	}

	for (const auto &cat_file : cat_files)
	{
		nofs::cat_file(cache, nofs::parse_sha1_digest(cat_file), std::cout);
	}
}
