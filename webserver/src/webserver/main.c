#include "settings.h"
#include "server.h"
#include "common/buffer.h"
#include "common/load_file.h"
#include <stdio.h>

#ifdef WEBSERVER_WITH_VLD
#include <vld.h>
#endif


static bool load_settings(
	settings_t *settings,
	char const *file_name,
	log_t *log
	)
{
	buffer_t settings_content;

	buffer_create(&settings_content);

	if (!load_buffer_from_file_name(&settings_content, file_name))
	{
		log_write(log, "Could not load settings file '%s'", file_name);
		buffer_destroy(&settings_content);
		return false;
	}

	if (!settings_create(settings, settings_content.data, settings_content.data + settings_content.size))
	{
		buffer_destroy(&settings_content);
		return false;
	}

	buffer_destroy(&settings_content);
	return true;
}

int main(int argc, char **argv)
{
	const unsigned short acceptor_port = ((argc >= 2) ? (unsigned short)atoi(argv[1]) : 8080);
	char const * const settings_file_name = ((argc >= 3) ? argv[2] : "settings.txt");
	settings_t settings;
	server_t server;
	log_t log;

	log_create(&log, stderr);

	if (!load_settings(&settings, settings_file_name, &log))
	{
		return 1;
	}

	if (!server_create(&server, &log, &settings, acceptor_port))
	{
		settings_destroy(&settings);
		return 1;
	}

	settings_destroy(&settings);

	server_run(&server);
	server_destroy(&server);

	log_destroy(&log);
	return 0;
}
