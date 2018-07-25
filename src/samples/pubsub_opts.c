/*******************************************************************************
 * Copyright (c) 2012, 2018 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include "MQTTAsync.h"
#include "MQTTClientPersistence.h"
#include "pubsub_opts.h"

#include <string.h>
#include <stdlib.h>


void usage(struct pubsub_opts* opts, const char* version)
{
	printf("Eclipse Paho MQTT C %s\n", opts->publisher ? "publisher" : "subscriber");
	printf("Eclipse Paho C library version %s\n", version);

	printf("Usage: paho_c_pub <topicname> <options>, where options are:\n"
	"  -t (--topic) MQTT topic to publish to\n"
	"  -h (--host) host to connect to (default is %s)\n"
	"  -p (--port) network port to connect to (default is %s)\n"
	"  -c (--connection) connection string, overrides host/port e.g wss://hostname:port/ws\n"
	"  -q (--qos) MQTT QoS to publish on (0, 1 or 2) (default is %d)\n"
	"  -r (--retained) use MQTT retain option? (default is %s)\n"
	"  -i (--clientid) <clientid> (default is %s)\n"
	"  -u (--username) MQTT username (default is none)\n"
	"  -P (--password) MQTT password (default is none)\n"
	"  -k (--keepalive) MQTT keepalive timeout value (default is %d seconds)\n"
	"  --delimiter <delim> (default is \\n)\n"
	"  --maxdatalen <bytes> (default is %d)\n",
	opts->host, opts->port, opts->qos, opts->retained ? "on" : "off",
			opts->clientid, opts->maxdatalen, opts->keepalive);
	exit(EXIT_FAILURE);
}



int getopts(int argc, char** argv, struct pubsub_opts* opts)
{
	int count = 1;

	if (argv[1][0] != '-')
	{
		opts->topic = argv[1];
		count = 2;
	}

	while (count < argc)
	{
		if (strcmp(argv[count], "--verbose") == 0 || strcmp(argv[count], "-v") == 0)
			opts->verbose = 1;
		else if (strcmp(argv[count], "--qos") == 0 || strcmp(argv[count], "-q") == 0)
		{
			if (++count < argc)
			{
				if (strcmp(argv[count], "0") == 0)
					opts->qos = 0;
				else if (strcmp(argv[count], "1") == 0)
					opts->qos = 1;
				else if (strcmp(argv[count], "2") == 0)
					opts->qos = 2;
				else
					return 1;
			}
			else
				return 1;
		}
		else if (strcmp(argv[count], "--connection") == 0 || strcmp(argv[count], "-c") == 0)
		{
			if (++count < argc)
				opts->connection = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--host") == 0 || strcmp(argv[count], "-h") == 0)
		{
			if (++count < argc)
				opts->host = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--port") == 0 || strcmp(argv[count], "-p") == 0)
		{
			if (++count < argc)
				opts->port = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--clientid") == 0 || strcmp(argv[count], "-i") == 0)
		{
			if (++count < argc)
				opts->clientid = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--username") == 0 || strcmp(argv[count], "-u") == 0)
		{
			if (++count < argc)
				opts->username = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--password") == 0 || strcmp(argv[count], "-P") == 0)
		{
			if (++count < argc)
				opts->password = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--maxdatalen") == 0)
		{
			if (++count < argc)
				opts->maxdatalen = atoi(argv[count]);
			else
				return 1;
		}
		else if (strcmp(argv[count], "--delimiter") == 0)
		{
			if (++count < argc)
				opts->delimiter = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--keepalive") == 0 || strcmp(argv[count], "-k") == 0)
		{
			if (++count < argc)
				opts->keepalive = atoi(argv[count]);
			else
				return 1;
		}
		else if (strcmp(argv[count], "--topic") == 0 || strcmp(argv[count], "-t") == 0)
		{
			if (++count < argc)
				opts->topic = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--will-topic") == 0)
		{
			if (++count < argc)
				opts->will_topic = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--will-payload") == 0)
		{
			if (++count < argc)
				opts->will_payload = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--will-qos") == 0)
		{
			if (++count < argc)
				opts->will_qos = atoi(argv[count]);
			else
				return 1;
		}
		else if (strcmp(argv[count], "--will-retain") == 0)
		{
			if (++count < argc)
				opts->will_retain = 1;
			else
				return 1;
		}
		else if (strcmp(argv[count], "--insecure") == 0)
		{
			if (++count < argc)
				opts->insecure = 1;
			else
				return 1;
		}
		else if (strcmp(argv[count], "--capath") == 0)
		{
			if (++count < argc)
				opts->capath = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--cafile") == 0)
		{
			if (++count < argc)
				opts->cafile = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--cert") == 0)
		{
			if (++count < argc)
				opts->cert = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--key") == 0)
		{
			if (++count < argc)
				opts->key = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--keypass") == 0)
		{
			if (++count < argc)
				opts->keypass = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "--ciphers") == 0)
		{
			if (++count < argc)
				opts->ciphers = argv[count];
			else
				return 1;
		}
		else if (strcmp(argv[count], "-V") == 0)
		{
			if (++count < argc)
			{
				if (strcmp(argv[count], "mqttv31") == 0 || strcmp(argv[count], "31") == 0)
					opts->MQTTVersion = MQTTVERSION_3_1;
				else if (strcmp(argv[count], "mqttv311") == 0 || strcmp(argv[count], "311") == 0)
					opts->MQTTVersion = MQTTVERSION_3_1_1;
				else if (strcmp(argv[count], "mqttv5") == 0 || strcmp(argv[count], "5") == 0)
					opts->MQTTVersion = MQTTVERSION_5;
				else
					return 1;
			}
			else
				return 1;
		}
		else if (strcmp(argv[count], "--trace") == 0)
		{
			if (++count < argc)
			{
				if (strcmp(argv[count], "error") == 0)
					opts->tracelevel = MQTTASYNC_TRACE_ERROR;
				else if (strcmp(argv[count], "protocol") == 0)
					opts->tracelevel = MQTTASYNC_TRACE_PROTOCOL;
				else if (strcmp(argv[count], "min") == 0 || strcmp(argv[count], "on") == 0)
					opts->tracelevel = MQTTASYNC_TRACE_MINIMUM;
				else if (strcmp(argv[count], "max") == 0)
					opts->tracelevel = MQTTASYNC_TRACE_MAXIMUM;
				else
					return 1;
			}
			else
				return 1;
		}
		else if (opts->publisher == 0)
		{
			if (strcmp(argv[count], "--no-print-retained") == 0 || strcmp(argv[count], "-R") == 0)
				opts->retained = 1;
			else
			{
				fprintf(stderr, "Unknown option %s\n", argv[count]);
				return 1;
			}
		}
		else if (opts->publisher == 1)
		{
			if (strcmp(argv[count], "--retained") == 0 || strcmp(argv[count], "-r") == 0)
				opts->retained = 1;
			else if (strcmp(argv[count], "--user-property") == 0)
			{
				if (count + 2 < argc)
				{
					opts->user_property.name = argv[++count];
					opts->user_property.value = argv[++count];
				}
				else
					return 1;
			}
			else if (strcmp(argv[count], "--message-expiry") == 0)
			{
				if (++count < argc)
					opts->message_expiry = atoi(argv[count]);
				else
					return 1;
			}
			else if (strcmp(argv[count], "-m") == 0 || strcmp(argv[count], "--message") == 0)
			{
				if (++count < argc)
				{
					opts->stdin_lines = 0;
					opts->message = argv[count];
				}
				else
					return 1;
			}
			else if (strcmp(argv[count], "-f") == 0 || strcmp(argv[count], "--filename") == 0)
			{
				if (++count < argc)
				{
					opts->stdin_lines = 0;
					opts->message = argv[count];
				}
				else
					return 1;
			}
			else if (strcmp(argv[count], "-n") == 0 || strcmp(argv[count], "--null-message") == 0)
			{
				opts->stdin_lines = 0;
				opts->null_message = 1;
			}
			else
			{
				fprintf(stderr, "Unknown option %s\n", argv[count]);
				return 1;
			}
		}
		else
		{
			fprintf(stderr, "Unknown option %s\n", argv[count]);
			return 1;
		}

		count++;
	}

	if (opts->topic == NULL)
		return 1;

	return 0;
}
