#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static void print_help(FILE *out)
{
	fprintf(out,
		"Syntax:\n"
		"  finddiff first second [more files...]\n"
		);
}

typedef long file_position;

static file_position find_diff(FILE **files, size_t file_count)
{
	file_position diff;
	for (diff = 0;; ++diff)
	{
		size_t f;

		int const value = fgetc(files[0]);
		if (value == EOF)
		{
			return diff;
		}

		for (f = 1; f < file_count; ++f)
		{
			if (value != fgetc(files[f]))
			{
				return diff;
			}
		}
	}
}

static void close_files(FILE **files, size_t file_count)
{
	size_t i;
	for (i = 0; i < file_count; ++i)
	{
		assert(files[i]);
		fclose(files[i]);
	}
}

int main(int argc, char const * const *argv)
{
	FILE **files;
	size_t file_count = (argc - 1);
	size_t i;
	file_position diff;

	if (file_count < 2)
	{
		print_help(stderr);
		return 1;
	}

	files = malloc(sizeof(*files) * file_count);
	if (!files)
	{
		fprintf(stderr, "Out of memory\n");
		return 1;
	}

	for (i = 0; i < file_count; ++i)
	{
		char const * const fileName = argv[i + 1];
		files[i] = fopen(fileName, "rb");
		if (!files[i])
		{
			fprintf(stderr, "Could not open %s\n", fileName);
			close_files(files, i);
			free(files);
			return 1;
		}
	}

	diff = find_diff(files, file_count);
	fprintf(stdout, "%u\n", (unsigned)diff);

	close_files(files, file_count);
	free(files);
	return 0;
}
