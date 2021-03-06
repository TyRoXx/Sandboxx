#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static char xor_size_t(size_t value)
{
	char result = 0;
	char *begin = (char *)&value;
	char * const end = (begin + sizeof(value));
	for (; begin != end; ++begin)
	{
		result ^= *begin;
	}
	return result;
}

static char xor_range(size_t const *begin, size_t size)
{
	size_t xored = 0;
	size_t const * const size_end = begin + (size / sizeof(*begin));
	size_t const rest = (size % sizeof(*begin));
	char result;
	size_t i;

	for (; begin != size_end; ++begin)
	{
		xored ^= *begin;
	}

	result = xor_size_t(xored);

	for (i = 0; i < rest; ++i)
	{
		result ^= ((char const *)begin)[i];
	}

	return result;
}

typedef struct test_result
{
	int success;
	char sum;
}
test_result;

static test_result test_fread(char const *file_name)
{
	test_result result = {0, 0};
	FILE * const file = fopen(file_name, "rb");

	if (!file)
	{
		fprintf(stderr, "Could not open file\n");
		return result;
	}

	for (;;)
	{
		size_t buffer[8192 / sizeof(size_t)];
		size_t const r = fread((char *)&buffer, 1, sizeof(buffer), file);
		if (r == 0)
		{
			break;
		}
		result.sum ^= xor_range(buffer, r);
	}

	fclose(file);
	result.success = 1;
	return result;
}

static test_result test_read(char const *file_name)
{
	test_result result = {0, 0};
	int const file = open(file_name, 0);

	if (file < 0)
	{
		fprintf(stderr, "Could not open file\n");
		return result;
	}

	for (;;)
	{
		size_t buffer[8192 / sizeof(size_t)];
		ssize_t const r = read(file, (char *)&buffer, sizeof(buffer));
		if (r <= 0)
		{
			break;
		}
		result.sum ^= xor_range(buffer, (size_t)r);
	}

	close(file);
	result.success = 1;
	return result;
}

static test_result test_mmap(char const *file_name)
{
	test_result result = {0, 0};
	int const file = open(file_name, 0);
	void *content;
	struct stat file_info;

	if (file < 0)
	{
		fprintf(stderr, "Could not open file\n");
		return result;
	}

	if (fstat(file, &file_info) != 0)
	{
		fprintf(stderr, "Could not determine file size\n");
		close(file);
		return result;
	}

	if (file_info.st_size > 0x7fffffff)
	{
		fprintf(stderr, "The file is too large\n");
		close(file);
		return result;
	}

	content = mmap(0, (size_t)file_info.st_size, PROT_READ, MAP_PRIVATE, file, 0);
	if (!content)
	{
		fprintf(stderr, "Could not map file into memory\n");
		close(file);
		return result;
	}

	if (madvise(content, (size_t)file_info.st_size, MADV_SEQUENTIAL) != 0)
	{
		fprintf(stderr, "Could not set mmap advise\n");
	}

	result.sum = xor_range(content, (size_t)file_info.st_size);

	munmap(content, (size_t)file_info.st_size);
	close(file);
	result.success = 1;
	return result;
}

typedef struct test_scenario
{
	char const *name;
	test_result (*run)(char const *);
}
test_scenario;

static test_scenario const scenarios[] =
{
	{"fread", test_fread},
	{"read", test_read},
	{"mmap", test_mmap}
};

int main(int argc, char **argv)
{
	char const *scenario, *file_name;
	size_t i;

	if (argc < 3)
	{
		fprintf(stderr, "Missing arguments\n");
		return 1;
	}

	scenario = argv[1];
	file_name = argv[2];

	for (i = 0; i < sizeof(scenarios) / sizeof(scenarios[0]); ++i)
	{
		if (!strcmp(scenario, scenarios[i].name))
		{
			test_result const result = scenarios[i].run(file_name);
			if (result.success)
			{
				printf("%u\n", (unsigned)(unsigned char)result.sum);
				return 0;
			}

			fprintf(stderr, "Test failed\n");
			return 1;
		}
	}

	fprintf(stderr, "Unknown scenario\n");
	return 1;
}
