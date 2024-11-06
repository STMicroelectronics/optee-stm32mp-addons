// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2022, STMicroelectronics
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>
#include <pta_invoke_tests.h>
/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>
#include <errno.h>
#include <assert.h>


static struct option options[] = {
	{
		.name    = "init",
		.has_arg = no_argument,
		.flag    = NULL,
		.val     = PTA_MBOX_TEST_COPRO_INIT,
	},
	{
		.name    = "copro_send_wait\0<iteration of test> in decimal",
		.has_arg = required_argument,
		.flag    = NULL,
		.val     = PTA_MBOX_TEST_COPRO_SEND_WAIT,
	},
	{
		.name    = "copro_wait_send\0<iteration of test> in decimal",
		.has_arg = required_argument,
		.flag    = NULL,
		.val     = PTA_MBOX_TEST_COPRO_WAIT_SEND,
	},
	{
		.name    = "recv_send\0<iteration of test> in decimal",
		.has_arg = required_argument,
		.flag    = NULL,
		.val     = PTA_MBOX_TEST_RECEIVE_SEND,
	},
	{
		.name    = "send_recv\0<iteration of test> in decimal",
		.has_arg = required_argument,
		.flag    = NULL,
		.val     = PTA_MBOX_TEST_SEND_RECEIVE,
	},
	{
		.name    = "ipcc_send_recv\0<iteration of test> in decimal",
		.has_arg = required_argument,
		.flag    = NULL,
		.val     = PTA_MBOX_TEST_IPCC_SEND_RECEIVE,
	},

	{ NULL, 0, 0, 0 },

};

static void  parse_options_or_die (int argc, char *argv[], uint32_t *a,
				   uint32_t *b)
{
	bool end_of_options = false;
	char *cursor = NULL;

	if (argc != 2)  {
		printf("mailbox_test --init && mailbox_test --recv_send=10 & sleep 0.1 && mailbox_test --copro_send_wait=10 &\r\n");
		printf("mailbox_test --init && mailbox_test --send_recv=10 & sleep 0.1 && mailbox_test --copro_wait_send=10 &\r\n");
		printf("mailbox_test --ipcc_send_recv=10\r\n");
		exit(EXIT_FAILURE);
	}

	do {
		int option_index = 0;
		int result = getopt_long(argc, argv, "", options, &option_index);
		switch (result) {
		case -1:
			end_of_options = true;
			break;
		case PTA_MBOX_TEST_COPRO_INIT:
			*a = result;
			break;

		case PTA_MBOX_TEST_RECEIVE_SEND:
		case PTA_MBOX_TEST_SEND_RECEIVE:
		case PTA_MBOX_TEST_COPRO_WAIT_SEND:
		case PTA_MBOX_TEST_COPRO_SEND_WAIT:
		case PTA_MBOX_TEST_IPCC_SEND_RECEIVE:
			*a = result;
			*b = strtoul(optarg, &cursor, 10);
			assert(cursor != NULL);

			if (errno != 0 || (cursor != NULL && *cursor != '\0')) {
				fprintf(stderr, "--%s: invalid iteration option\n",
					options[option_index].name);
				exit(EXIT_FAILURE);
			}
			break;
		default:
			exit(EXIT_FAILURE);
		}

	} while (!end_of_options);
	printf("a=%d, b=%d\n\r",*a,*b);
}

int main (int argc, char *argv[]) {

	TEEC_Result res = TEEC_SUCCESS;
	TEEC_Context ctx = {0};
	TEEC_Session sess = {0};
	TEEC_Operation op = {0};
	TEEC_UUID uuid = PTA_INVOKE_TESTS_UUID;
	uint32_t err_origin = 0;
	uint32_t a = 0;
	uint32_t b = 1;
	/* parse argument */
	parse_options_or_die (argc, argv, &a, &b);
	printf("a = %d\n",a);
	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
		     res, err_origin);

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);

	op.params[0].value.a = a;
	op.params[0].value.b = b;

	res = TEEC_InvokeCommand(&sess, PTA_INVOKE_TESTS_CMD_MBOX_TESTS, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
		     res, err_origin);

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return 0;
}
