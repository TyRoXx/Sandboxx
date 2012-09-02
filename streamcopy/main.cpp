#include <iostream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <vector>

#ifdef _WIN32
#	include <Windows.h>
#else
#	include <sys/time.h>
#	include <unistd.h>
#endif

namespace
{
#ifdef _WIN32
	class Clock
	{
		LARGE_INTEGER _frequency;
		LARGE_INTEGER _start;

	public:
		Clock();
		void reset();
		double elapsed() const;
	};

	Clock::Clock()
	{
		QueryPerformanceFrequency(&_frequency);
		reset();
	}

	void Clock::reset()
	{
		QueryPerformanceCounter(&_start);
	}

	double Clock::elapsed() const
	{
		LARGE_INTEGER end;
		QueryPerformanceCounter(&end);
		return static_cast<double>(end.QuadPart - _start.QuadPart) / _frequency.QuadPart;
	}

#else

	class Clock
	{
		struct timespec _start;

	public:
		Clock();
		void reset();
		double elapsed() const;
	};

	Clock::Clock()
	{
		reset();
	}

	void Clock::reset()
	{
		clock_gettime(CLOCK_MONOTONIC, &_start);
	}

	double Clock::elapsed() const
	{
		struct timespec end;
		clock_gettime(CLOCK_MONOTONIC, &end);
		return (end.tv_sec - _start.tv_sec - 1) + (1000000000 + end.tv_nsec - _start.tv_nsec) / 1e9;
	}
#endif


	template <class Name, class Case>
	void benchmarkCopy(const Name &name, std::istream &source, const Case &c)
	{
		source.clear();
		source.seekg(0, std::ios::beg);
		std::ostringstream sink;
		Clock clock;
		c(static_cast<std::ostream &>(sink));
		const auto elapsed = clock.elapsed();
		std::cout << name << ": " << elapsed << "\n";
	}

	void runBenchmarks(std::istream &source)
	{
		benchmarkCopy("std::istream_iterator + std::ostream_iterator",
			source,
			[&source](std::ostream &sink)
		{
			std::copy( std::istream_iterator<char>(source>>std::noskipws), (std::istream_iterator<char>()), std::ostream_iterator<char>(sink) );
		});

		benchmarkCopy("std::istream_iterator + std::ostreambuf_iterator",
			source,
			[&source](std::ostream &sink)
		{
			std::copy( std::istream_iterator<char>(source>>std::noskipws), (std::istream_iterator<char>()), std::ostreambuf_iterator<char>(sink) );
		});

		benchmarkCopy("std::istreambuf_iterator + std::ostream_iterator",
			source,
			[&source](std::ostream &sink)
		{
			std::copy( std::istreambuf_iterator<char>(source>>std::noskipws), (std::istreambuf_iterator<char>()), std::ostream_iterator<char>(sink) );
		});

		benchmarkCopy("std::istreambuf_iterator + std::ostreambuf_iterator",
			source,
			[&source](std::ostream &sink)
		{
			std::copy( std::istreambuf_iterator<char>(source>>std::noskipws), (std::istreambuf_iterator<char>()), std::ostreambuf_iterator<char>(sink) );
		});

		benchmarkCopy("rdbuf",
			source,
			[&source](std::ostream &sink)
		{
			sink << source.rdbuf();
		});

		{
			const auto benchmarkCopyWithBuffer = [&](std::size_t bufferSize)
			{
				auto &source_ = source;
				benchmarkCopy("read + write (" + std::to_string(static_cast<unsigned long long>(bufferSize)) + ")",
					source,
					[bufferSize, &source_](std::ostream &sink)
				{
					std::vector<char> buffer(bufferSize);

					while (source_.read(buffer.data(), bufferSize) &&
						(source_.gcount() > 0))
					{
						sink.write(buffer.data(), source_.gcount());
					}
				});
			};

			benchmarkCopyWithBuffer(1UL);
			benchmarkCopyWithBuffer(1UL << 12);
			benchmarkCopyWithBuffer(1UL << 17);
		}

		benchmarkCopy("get + put",
			source,
			[&source](std::ostream &sink)
		{
			for (;;)
			{
				const char c = source.get();
				if (!source)
				{
					break;
				}
				sink.put(c);
			}
		});
	}
}

int main(int argc, char* argv[])
{
	std::istringstream source(std::string(104857600 / 10, ' '));
	runBenchmarks(source);
}
