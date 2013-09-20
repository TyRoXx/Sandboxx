#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/uuid/sha1.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/iostreams/stream.hpp>
#include <functional>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>

namespace nofs
{
	void add_file(const boost::filesystem::path &cache, char const *begin, char const *end)
	{
		boost::filesystem::create_directories(cache);
		boost::uuids::detail::sha1 hash_state;
		hash_state.process_block(begin, end);
		unsigned digest[5];
		hash_state.get_digest(digest);
		std::ostringstream fileNameBuffer;
		fileNameBuffer.width(8);
		fileNameBuffer.fill('0');
		fileNameBuffer << std::hex << std::right;
		std::copy(std::begin(digest),
		          std::end(digest),
		          std::ostream_iterator<unsigned>(fileNameBuffer));
		const auto fullPath = cache / fileNameBuffer.str();
		std::ofstream outFile(fullPath.string(), std::ios::binary);
		if (!outFile)
		{
			throw std::runtime_error("Could not write file " + fullPath.string());
		}
		outFile.write(begin, std::distance(begin, end));
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

	for (const auto &added_file : added_files)
	{
		boost::iostreams::mapped_file_params params;
		params.path = added_file;
		boost::iostreams::stream<boost::iostreams::mapped_file_source> file((params));
		nofs::add_file(".nofs", file->data(), file->data() + file->size());
	}

	for (const auto &cat_file : cat_files)
	{

	}
}
