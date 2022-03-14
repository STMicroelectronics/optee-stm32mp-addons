// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2022, STMicroelectronics
 */

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

#define PTA_CALIB_UUID		{ 0xee4e317c, 0x568b, 0x485f, \
				  { 0xb0, 0xb3, 0xd4, 0x92, 0x42, 0x6c, \
				    0x47, 0x96 } }
#define PTA_CALIB_REQUEST	0

int main(void)
{
	TEEC_Result res = TEEC_SUCCESS;
	TEEC_Context ctx = {0};
	TEEC_Session sess = {0};
	TEEC_Operation op = {0};
	TEEC_UUID uuid = PTA_CALIB_UUID;
	uint32_t err_origin = 0;

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

	/* Clear the TEEC_Operation struct */
	memset(&op, 0, sizeof(op));

	res = TEEC_InvokeCommand(&sess, PTA_CALIB_REQUEST, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
		     res, err_origin);

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return 0;
}
