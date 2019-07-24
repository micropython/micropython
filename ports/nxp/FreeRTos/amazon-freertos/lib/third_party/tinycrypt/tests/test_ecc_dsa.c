/* test_ecc_ecdsa.c - TinyCrypt implementation of some EC-DSA tests */

/* Copyright (c) 2014, Kenneth MacKay
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.*/

/*
 *  Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 *  test_ecc_ecdsa.c -- Implementation of some EC-DSA tests
 *
 */
#include <tinycrypt/ecc.h>
#include <tinycrypt/ecc_platform_specific.h>
#include <tinycrypt/ecc_dsa.h>
#include <tinycrypt/ecc_dh.h>
#include <tinycrypt/constants.h>
#include <tinycrypt/sha256.h>
#include <test_utils.h>
#include <test_ecc_utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>

/* Maximum size of message to be signed. */
#define BUF_SIZE 256

int sign_vectors(TCSha256State_t hash, char **d_vec, char **k_vec,
		 char **msg_vec, char **qx_vec, char **qy_vec, char **r_vec,
		 char **s_vec, int tests, bool verbose)
{

	unsigned int k[NUM_ECC_WORDS];
	unsigned int private[NUM_ECC_WORDS];
	uint8_t private_bytes[NUM_ECC_BYTES];
	unsigned int sig[2 * NUM_ECC_WORDS];
	uint8_t sig_bytes[2 * NUM_ECC_BYTES];
	unsigned int digest[TC_SHA256_DIGEST_SIZE / 4];
	uint8_t  digest_bytes[TC_SHA256_DIGEST_SIZE];
	unsigned int result = TC_PASS;

	/* expected outputs (converted input vectors) */
	unsigned int exp_r[NUM_ECC_WORDS];
	unsigned int exp_s[NUM_ECC_WORDS];

	uint8_t msg[BUF_SIZE];
	size_t msglen;

	for (int i = 0; i < tests; i++) {

		/* use keygen test to generate and validate pubkey */
		keygen_vectors(d_vec+i, qx_vec+i, qy_vec+i, 1, false);
		string2scalar(private, NUM_ECC_WORDS, d_vec[i]);
		uECC_vli_nativeToBytes(private_bytes, NUM_ECC_BYTES, private);

		/* validate ECDSA: hash message, sign digest, check r+s */
		memset(k, 0, NUM_ECC_BYTES);
		string2scalar(k, NUM_ECC_WORDS, k_vec[i]);
		string2scalar(exp_r, NUM_ECC_WORDS, r_vec[i]);
		string2scalar(exp_s, NUM_ECC_WORDS, s_vec[i]);

		msglen = hex2bin(msg, BUF_SIZE, msg_vec[i], strlen(msg_vec[i]));

		if (msglen == false) {
			TC_ERROR("failed to import message!\n");
			result = TC_FAIL;
			goto exitTest1;
		}

		tc_sha256_init(hash);
		tc_sha256_update(hash, msg, msglen);
		tc_sha256_final(digest_bytes, hash);

		/* if digest larger than ECC scalar, drop the end
		 * if digest smaller than ECC scalar, zero-pad front */
		int hash_dwords = TC_SHA256_DIGEST_SIZE / 4;
		if (NUM_ECC_WORDS < hash_dwords) {
			hash_dwords = NUM_ECC_WORDS;
		}

		memset(digest, 0, NUM_ECC_BYTES - 4 * hash_dwords);
		uECC_vli_bytesToNative(digest + (NUM_ECC_WORDS-hash_dwords),
				        digest_bytes, TC_SHA256_DIGEST_SIZE);

		if (uECC_sign_with_k(private_bytes, digest_bytes, 
		    sizeof(digest_bytes), k, sig_bytes, uECC_secp256r1()) == 0) {
			TC_ERROR("ECDSA_sign failed!\n");
			result = TC_FAIL;
			goto exitTest1;
		}

		uECC_vli_bytesToNative(sig, sig_bytes, NUM_ECC_BYTES);
		uECC_vli_bytesToNative(sig + NUM_ECC_WORDS, sig_bytes+NUM_ECC_BYTES, NUM_ECC_BYTES);

		result = check_ecc_result(i, "sig.r", exp_r, sig,  NUM_ECC_WORDS, verbose);
		if(result == TC_FAIL) {
		  goto exitTest1;
		}
		result = check_ecc_result(i, "sig.s", exp_s, sig + NUM_ECC_WORDS,  NUM_ECC_WORDS, verbose);
		if(result == TC_FAIL) {
		  goto exitTest1;
		}
  	}

 exitTest1:
	TC_END_RESULT(result);
	return result;
}

int cavp_sign(bool verbose)
{

	/*
	 * [P-256,SHA-256]
	 */
	char *d[] = {
		"519b423d715f8b581f4fa8ee59f4771a5b44c8130b4e3eacca54a56dda72b464",
		"0f56db78ca460b055c500064824bed999a25aaf48ebb519ac201537b85479813",
		"e283871239837e13b95f789e6e1af63bf61c918c992e62bca040d64cad1fc2ef",
		"a3d2d3b7596f6592ce98b4bfe10d41837f10027a90d7bb75349490018cf72d07",
		"53a0e8a8fe93db01e7ae94e1a9882a102ebd079b3a535827d583626c272d280d",
		"4af107e8e2194c830ffb712a65511bc9186a133007855b49ab4b3833aefc4a1d",
		"78dfaa09f1076850b3e206e477494cddcfb822aaa0128475053592c48ebaf4ab",
		"80e692e3eb9fcd8c7d44e7de9f7a5952686407f90025a1d87e52c7096a62618a",
		"5e666c0db0214c3b627a8e48541cc84a8b6fd15f300da4dff5d18aec6c55b881",
		"f73f455271c877c4d5334627e37c278f68d143014b0a05aa62f308b2101c5308",
		"b20d705d9bd7c2b8dc60393a5357f632990e599a0975573ac67fd89b49187906",
		"d4234bebfbc821050341a37e1240efe5e33763cbbb2ef76a1c79e24724e5a5e7",
		"b58f5211dff440626bb56d0ad483193d606cf21f36d9830543327292f4d25d8c",
		"54c066711cdb061eda07e5275f7e95a9962c6764b84f6f1f3ab5a588e0a2afb1",
		"34fa4682bf6cb5b16783adcd18f0e6879b92185f76d7c920409f904f522db4b1",
	};

	char *k[] = {
		"94a1bbb14b906a61a280f245f9e93c7f3b4a6247824f5d33b9670787642a68de",
		"6d3e71882c3b83b156bb14e0ab184aa9fb728068d3ae9fac421187ae0b2f34c6",
		"ad5e887eb2b380b8d8280ad6e5ff8a60f4d26243e0124c2f31a297b5d0835de2",
		"24fc90e1da13f17ef9fe84cc96b9471ed1aaac17e3a4bae33a115df4e5834f18",
		"5d833e8d24cc7a402d7ee7ec852a3587cddeb48358cea71b0bedb8fabe84e0c4",
		"e18f96f84dfa2fd3cdfaec9159d4c338cd54ad314134f0b31e20591fc238d0ab",
		"295544dbb2da3da170741c9b2c6551d40af7ed4e891445f11a02b66a5c258a77",
		"7c80fd66d62cc076cef2d030c17c0a69c99611549cb32c4ff662475adbe84b22",
		"2e7625a48874d86c9e467f890aaa7cd6ebdf71c0102bfdcfa24565d6af3fdce9",
		"62f8665fd6e26b3fa069e85281777a9b1f0dfd2c0b9f54a086d0c109ff9fd615",
		"72b656f6b35b9ccbc712c9f1f3b1a14cbbebaec41c4bca8da18f492a062d6f6f",
		"d926fe10f1bfd9855610f4f5a3d666b1a149344057e35537373372ead8b1a778",
		"e158bf4a2d19a99149d9cdb879294ccb7aaeae03d75ddd616ef8ae51a6dc1071",
		"646fe933e96c3b8f9f507498e907fdd201f08478d0202c752a7c2cfebf4d061a",
		"a6f463ee72c9492bc792fe98163112837aebd07bab7a84aaed05be64db3086f4",
 	};

	char *Msg[] = {
		"5905238877c77421f73e43ee3da6f2d9e2ccad5fc942dcec0cbd25482935faaf416983fe16"
		"5b1a045ee2bcd2e6dca3bdf46c4310a7461f9a37960ca672d3feb5473e253605fb1ddfd280"
		"65b53cb5858a8ad28175bf9bd386a5e471ea7a65c17cc934a9d791e91491eb3754d0379979"
		"0fe2d308d16146d5c9b0d0debd97d79ce8",
		"c35e2f092553c55772926bdbe87c9796827d17024dbb9233a545366e2e5987dd344deb72df"
		"987144b8c6c43bc41b654b94cc856e16b96d7a821c8ec039b503e3d86728c494a967d83011"
		"a0e090b5d54cd47f4e366c0912bc808fbb2ea96efac88fb3ebec9342738e225f7c7c2b011c"
		"e375b56621a20642b4d36e060db4524af1",
		"3c054e333a94259c36af09ab5b4ff9beb3492f8d5b4282d16801daccb29f70fe61a0b37ffe"
		"f5c04cd1b70e85b1f549a1c4dc672985e50f43ea037efa9964f096b5f62f7ffdf8d6bfb2cc"
		"859558f5a393cb949dbd48f269343b5263dcdb9c556eca074f2e98e6d94c2c29a677afaf80"
		"6edf79b15a3fcd46e7067b7669f83188ee",
		"0989122410d522af64ceb07da2c865219046b4c3d9d99b01278c07ff63eaf1039cb787ae9e"
		"2dd46436cc0415f280c562bebb83a23e639e476a02ec8cff7ea06cd12c86dcc3adefbf1a9e"
		"9a9b6646c7599ec631b0da9a60debeb9b3e19324977f3b4f36892c8a38671c8e1cc8e50fcd"
		"50f9e51deaf98272f9266fc702e4e57c30",
		"dc66e39f9bbfd9865318531ffe9207f934fa615a5b285708a5e9c46b7775150e818d7f24d2"
		"a123df3672fff2094e3fd3df6fbe259e3989dd5edfcccbe7d45e26a775a5c4329a084f057c"
		"42c13f3248e3fd6f0c76678f890f513c32292dd306eaa84a59abe34b16cb5e38d0e885525d"
		"10336ca443e1682aa04a7af832b0eee4e7",
		"600974e7d8c5508e2c1aab0783ad0d7c4494ab2b4da265c2fe496421c4df238b0be25f2565"
		"9157c8a225fb03953607f7df996acfd402f147e37aee2f1693e3bf1c35eab3ae360a2bd91d"
		"04622ea47f83d863d2dfecb618e8b8bdc39e17d15d672eee03bb4ce2cc5cf6b217e5faf3f3"
		"36fdd87d972d3a8b8a593ba85955cc9d71",
		"dfa6cb9b39adda6c74cc8b2a8b53a12c499ab9dee01b4123642b4f11af336a91a5c9ce0520"
		"eb2395a6190ecbf6169c4cba81941de8e76c9c908eb843b98ce95e0da29c5d4388040264e0"
		"5e07030a577cc5d176387154eabae2af52a83e85c61c7c61da930c9b19e45d7e34c8516dc3"
		"c238fddd6e450a77455d534c48a152010b",
		"51d2547cbff92431174aa7fc7302139519d98071c755ff1c92e4694b58587ea560f72f32fc"
		"6dd4dee7d22bb7387381d0256e2862d0644cdf2c277c5d740fa089830eb52bf79d1e75b859"
		"6ecf0ea58a0b9df61e0c9754bfcd62efab6ea1bd216bf181c5593da79f10135a9bc6e164f1"
		"854bc8859734341aad237ba29a81a3fc8b",
		"558c2ac13026402bad4a0a83ebc9468e50f7ffab06d6f981e5db1d082098065bcff6f21a7a"
		"74558b1e8612914b8b5a0aa28ed5b574c36ac4ea5868432a62bb8ef0695d27c1e3ceaf75c7"
		"b251c65ddb268696f07c16d2767973d85beb443f211e6445e7fe5d46f0dce70d58a4cd9fe7"
		"0688c035688ea8c6baec65a5fc7e2c93e8",
		"4d55c99ef6bd54621662c3d110c3cb627c03d6311393b264ab97b90a4b15214a5593ba2510"
		"a53d63fb34be251facb697c973e11b665cb7920f1684b0031b4dd370cb927ca7168b0bf8ad"
		"285e05e9e31e34bc24024739fdc10b78586f29eff94412034e3b606ed850ec2c1900e8e681"
		"51fc4aee5adebb066eb6da4eaa5681378e",
		"f8248ad47d97c18c984f1f5c10950dc1404713c56b6ea397e01e6dd925e903b4fadfe2c9e8"
		"77169e71ce3c7fe5ce70ee4255d9cdc26f6943bf48687874de64f6cf30a012512e787b8805"
		"9bbf561162bdcc23a3742c835ac144cc14167b1bd6727e940540a9c99f3cbb41fb1dcb00d7"
		"6dda04995847c657f4c19d303eb09eb48a",
		"3b6ee2425940b3d240d35b97b6dcd61ed3423d8e71a0ada35d47b322d17b35ea0472f35edd"
		"1d252f87b8b65ef4b716669fc9ac28b00d34a9d66ad118c9d94e7f46d0b4f6c2b2d339fd6b"
		"cd351241a387cc82609057048c12c4ec3d85c661975c45b300cb96930d89370a327c98b67d"
		"efaa89497aa8ef994c77f1130f752f94a4",
		"c5204b81ec0a4df5b7e9fda3dc245f98082ae7f4efe81998dcaa286bd4507ca840a53d21b0"
		"1e904f55e38f78c3757d5a5a4a44b1d5d4e480be3afb5b394a5d2840af42b1b4083d40afbf"
		"e22d702f370d32dbfd392e128ea4724d66a3701da41ae2f03bb4d91bb946c7969404cb544f"
		"71eb7a49eb4c4ec55799bda1eb545143a7",
		"72e81fe221fb402148d8b7ab03549f1180bcc03d41ca59d7653801f0ba853add1f6d29edd7"
		"f9abc621b2d548f8dbf8979bd16608d2d8fc3260b4ebc0dd42482481d548c7075711b57596"
		"49c41f439fad69954956c9326841ea6492956829f9e0dc789f73633b40f6ac77bcae6dfc79"
		"30cfe89e526d1684365c5b0be2437fdb01",
		"21188c3edd5de088dacc1076b9e1bcecd79de1003c2414c3866173054dc82dde85169baa77"
		"993adb20c269f60a5226111828578bcc7c29e6e8d2dae81806152c8ba0c6ada1986a1983eb"
		"eec1473a73a04795b6319d48662d40881c1723a706f516fe75300f92408aa1dc6ae4288d20"
		"46f23c1aa2e54b7fb6448a0da922bd7f34",
	};

	char *Qx[] = {
		"1ccbe91c075fc7f4f033bfa248db8fccd3565de94bbfb12f3c59ff46c271bf83",
		"e266ddfdc12668db30d4ca3e8f7749432c416044f2d2b8c10bf3d4012aeffa8a",
		"74ccd8a62fba0e667c50929a53f78c21b8ff0c3c737b0b40b1750b2302b0bde8",
		"322f80371bf6e044bc49391d97c1714ab87f990b949bc178cb7c43b7c22d89e1",
		"1bcec4570e1ec2436596b8ded58f60c3b1ebc6a403bc5543040ba82963057244",
		"a32e50be3dae2c8ba3f5e4bdae14cf7645420d425ead94036c22dd6c4fc59e00",
		"8bcfe2a721ca6d753968f564ec4315be4857e28bef1908f61a366b1f03c97479",
		"a88bc8430279c8c0400a77d751f26c0abc93e5de4ad9a4166357952fe041e767",
		"1bc487570f040dc94196c9befe8ab2b6de77208b1f38bdaae28f9645c4d2bc3a",
		"b8188bd68701fc396dab53125d4d28ea33a91daf6d21485f4770f6ea8c565dde",
		"51f99d2d52d4a6e734484a018b7ca2f895c2929b6754a3a03224d07ae61166ce",
		"8fb287f0202ad57ae841aea35f29b2e1d53e196d0ddd9aec24813d64c0922fb7",
		"68229b48c2fe19d3db034e4c15077eb7471a66031f28a980821873915298ba76",
		"0a7dbb8bf50cb605eb2268b081f26d6b08e012f952c4b70a5a1e6e7d46af98bb",
		"105d22d9c626520faca13e7ced382dcbe93498315f00cc0ac39c4821d0d73737",
	};

	char *Qy[] = {
		"ce4014c68811f9a21a1fdb2c0e6113e06db7ca93b7404e78dc7ccd5ca89a4ca9",
		"bfa86404a2e9ffe67d47c587ef7a97a7f456b863b4d02cfc6928973ab5b1cb39",
		"29074e21f3a0ef88b9efdf10d06aa4c295cc1671f758ca0e4cd108803d0f2614",
		"3c15d54a5cc6b9f09de8457e873eb3deb1fceb54b0b295da6050294fae7fd999",
		"8af62a4c683f096b28558320737bf83b9959a46ad2521004ef74cf85e67494e1",
		"d623bf641160c289d6742c6257ae6ba574446dd1d0e74db3aaa80900b78d4ae9",
		"0f67576a30b8e20d4232d8530b52fb4c89cbc589ede291e499ddd15fe870ab96",
		"2d365a1eef25ead579cc9a069b6abc1b16b81c35f18785ce26a10ba6d1381185",
		"ec81602abd8345e71867c8210313737865b8aa186851e1b48eaca140320f5d8f",
		"423f058810f277f8fe076f6db56e9285a1bf2c2a1dae145095edd9c04970bc4a",
		"4737da963c6ef7247fb88d19f9b0c667cac7fe12837fdab88c66f10d3c14cad1",
		"1f6daff1aa2dd2d6d3741623eecb5e7b612997a1039aab2e5cf2de969cfea573",
		"303e8ee3742a893f78b810991da697083dd8f11128c47651c27a56740a80c24c",
		"f26dd7d799930062480849962ccf5004edcfd307c044f4e8f667c9baa834eeae",
		"6c47f3cbbfa97dfcebe16270b8c7d5d3a5900b888c42520d751e8faf3b401ef4",
	};

  	char *R[] = {
		"f3ac8061b514795b8843e3d6629527ed2afd6b1f6a555a7acabb5e6f79c8c2ac",
		"976d3a4e9d23326dc0baa9fa560b7c4e53f42864f508483a6473b6a11079b2db",
		"35fb60f5ca0f3ca08542fb3cc641c8263a2cab7a90ee6a5e1583fac2bb6f6bd1",
		"d7c562370af617b581c84a2468cc8bd50bb1cbf322de41b7887ce07c0e5884ca",
		"18caaf7b663507a8bcd992b836dec9dc5703c080af5e51dfa3a9a7c387182604",
		"8524c5024e2d9a73bde8c72d9129f57873bbad0ed05215a372a84fdbc78f2e68",
		"c5a186d72df452015480f7f338970bfe825087f05c0088d95305f87aacc9b254",
		"9d0c6afb6df3bced455b459cc21387e14929392664bb8741a3693a1795ca6902",
		"2f9e2b4e9f747c657f705bffd124ee178bbc5391c86d056717b140c153570fd9",
		"1cc628533d0004b2b20e7f4baad0b8bb5e0673db159bbccf92491aef61fc9620",
		"9886ae46c1415c3bc959e82b760ad760aab66885a84e620aa339fdf102465c42",
		"490efd106be11fc365c7467eb89b8d39e15d65175356775deab211163c2504cb",
		"e67a9717ccf96841489d6541f4f6adb12d17b59a6bef847b6183b8fcf16a32eb",
		"b53ce4da1aa7c0dc77a1896ab716b921499aed78df725b1504aba1597ba0c64b",
		"542c40a18140a6266d6f0286e24e9a7bad7650e72ef0e2131e629c076d962663",
	};

	char *S[] = {
		"8bf77819ca05a6b2786c76262bf7371cef97b218e96f175a3ccdda2acc058903",
		"1b766e9ceb71ba6c01dcd46e0af462cd4cfa652ae5017d4555b8eeefe36e1932",
		"ee59d81bc9db1055cc0ed97b159d8784af04e98511d0a9a407b99bb292572e96",
		"b46d9f2d8c4bf83546ff178f1d78937c008d64e8ecc5cbb825cb21d94d670d89",
		"77c68928ac3b88d985fb43fb615fb7ff45c18ba5c81af796c613dfa98352d29c",
		"d18c2caf3b1072f87064ec5e8953f51301cada03469c640244760328eb5a05cb",
		"84a58f9e9d9e735344b316b1aa1ab5185665b85147dc82d92e969d7bee31ca30",
		"d7f9ddd191f1f412869429209ee3814c75c72fa46a9cccf804a2f5cc0b7e739f",
		"f5413bfd85949da8d83de83ab0d19b2986613e224d1901d76919de23ccd03199",
		"880e0bbf82a8cf818ed46ba03cf0fc6c898e36fca36cc7fdb1d2db7503634430",
		"2bf3a80bc04faa35ebecc0f4864ac02d349f6f126e0f988501b8d3075409a26c",
		"644300fc0da4d40fb8c6ead510d14f0bd4e1321a469e9c0a581464c7186b7aa7",
		"9ae6ba6d637706849a6a9fc388cf0232d85c26ea0d1fe7437adb48de58364333",
		"d7c246dc7ad0e67700c373edcfdd1c0a0495fc954549ad579df6ed1438840851",
		"4f7f65305e24a6bbb5cff714ba8f5a2cee5bdc89ba8d75dcbf21966ce38eb66f",
	};

	struct tc_sha256_state_struct sha256_ctx;

	TC_PRINT("Test #1: ECDSAsign ");
	TC_PRINT("NIST-p256, SHA2-256\n");

	return sign_vectors(&sha256_ctx, d, k, Msg, Qx, Qy, R, S, 15, verbose);
}

int vrfy_vectors(TCSha256State_t hash, char **msg_vec, char **qx_vec, char **qy_vec,
		  char **r_vec, char **s_vec, char **res_vec, int tests, bool verbose)
{

	const struct uECC_Curve_t * curve = uECC_secp256r1();
	unsigned int pub[2 * NUM_ECC_WORDS];
	uint8_t pub_bytes[2 * NUM_ECC_BYTES];
	unsigned int sig[2 * NUM_ECC_WORDS];
	uint8_t sig_bytes[2 * NUM_ECC_BYTES];
	uint8_t  digest_bytes[TC_SHA256_DIGEST_SIZE];
	unsigned int digest[TC_SHA256_DIGEST_SIZE / 4];
	unsigned int result = TC_PASS;

	int rc;
	int exp_rc;
	char tmp;

	uint8_t msg[BUF_SIZE];
	size_t msglen;

	for (int i = 0; i < tests; i++) {

		string2scalar(pub, NUM_ECC_WORDS, qx_vec[i]);
		string2scalar(pub + NUM_ECC_WORDS, NUM_ECC_WORDS, qy_vec[i]);
		string2scalar(sig, NUM_ECC_WORDS, r_vec[i]);
		string2scalar(sig + NUM_ECC_WORDS, NUM_ECC_WORDS, s_vec[i]);

		if (2 != sscanf(res_vec[i], "%c (%d ", &tmp, &exp_rc)) {
			TC_ERROR("Error: failed to parse CAVP response.\n");
			result = TC_FAIL;
			goto exitTest1;
		}

		/* validate ECDSA: hash message, verify r+s */
		msglen = hex2bin(msg, BUF_SIZE, msg_vec[i], strlen(msg_vec[i]));

		if (msglen == false) {
			TC_ERROR("failed to import message!\n");
			result = TC_FAIL;
			goto exitTest1;
		}

		tc_sha256_init(hash);
		tc_sha256_update(hash, msg, msglen);
		tc_sha256_final(digest_bytes, hash);

		/* if digest larger than ECC scalar, drop the end
		 * if digest smaller than ECC scalar, zero-pad front */
		int hash_dwords = TC_SHA256_DIGEST_SIZE / 4;
		if (NUM_ECC_WORDS < hash_dwords) {
			hash_dwords = NUM_ECC_WORDS;
		}

		memset(digest, 0, NUM_ECC_BYTES - 4 * hash_dwords);
		uECC_vli_bytesToNative(digest + (NUM_ECC_WORDS-hash_dwords), digest_bytes,
				       TC_SHA256_DIGEST_SIZE);

		uECC_vli_nativeToBytes(pub_bytes, NUM_ECC_BYTES, pub);
		uECC_vli_nativeToBytes(pub_bytes + NUM_ECC_BYTES, NUM_ECC_BYTES, 
				       pub + NUM_ECC_WORDS);

		/* adapt return codes to match CAVP error: */
		if (0 != uECC_valid_public_key(pub_bytes, curve)) {
			/* error 4 - Q changed */
			rc = 4;
		} else {
			uECC_vli_nativeToBytes(sig_bytes, NUM_ECC_BYTES, sig);
			uECC_vli_nativeToBytes(sig_bytes + NUM_ECC_BYTES, NUM_ECC_BYTES,
					       sig + NUM_ECC_WORDS);

			rc = uECC_verify(pub_bytes, digest_bytes, sizeof(digest_bytes), sig_bytes,
									 uECC_secp256r1());
			/* CAVP expects 0 for success, others for fail */
			rc = !rc; 
			if (exp_rc != 0 && rc != 0) {
				/* mimic CAVP code on errors. */
				rc = exp_rc; 
			}
		}

		result = check_code(i, res_vec[i], exp_rc, rc, verbose);
		if(result == TC_FAIL) {
		  goto exitTest1;
		}
	}
 exitTest1:
	TC_END_RESULT(result);
	return result;
}

int cavp_verify(bool verbose)
{

	/*
	* [P-256,SHA-256]
	*/
	char *Msg[] = {
		"e4796db5f785f207aa30d311693b3702821dff1168fd2e04c0836825aefd850d9aa60326d8"
		"8cde1a23c7745351392ca2288d632c264f197d05cd424a30336c19fd09bb229654f0222fcb"
		"881a4b35c290a093ac159ce13409111ff0358411133c24f5b8e2090d6db6558afc36f06ca1"
		"f6ef779785adba68db27a409859fc4c4a0",
		"069a6e6b93dfee6df6ef6997cd80dd2182c36653cef10c655d524585655462d683877f95ec"
		"c6d6c81623d8fac4e900ed0019964094e7de91f1481989ae1873004565789cbf5dc56c62ae"
		"dc63f62f3b894c9c6f7788c8ecaadc9bd0e81ad91b2b3569ea12260e93924fdddd3972af52"
		"73198f5efda0746219475017557616170e",
		"df04a346cf4d0e331a6db78cca2d456d31b0a000aa51441defdb97bbeb20b94d8d746429a3"
		"93ba88840d661615e07def615a342abedfa4ce912e562af714959896858af817317a840dcf"
		"f85a057bb91a3c2bf90105500362754a6dd321cdd86128cfc5f04667b57aa78c112411e42d"
		"a304f1012d48cd6a7052d7de44ebcc01de",
		"e1130af6a38ccb412a9c8d13e15dbfc9e69a16385af3c3f1e5da954fd5e7c45fd75e2b8c36"
		"699228e92840c0562fbf3772f07e17f1add56588dd45f7450e1217ad239922dd9c32695dc7"
		"1ff2424ca0dec1321aa47064a044b7fe3c2b97d03ce470a592304c5ef21eed9f93da56bb23"
		"2d1eeb0035f9bf0dfafdcc4606272b20a3",
		"73c5f6a67456ae48209b5f85d1e7de7758bf235300c6ae2bdceb1dcb27a7730fb68c950b7f"
		"cada0ecc4661d3578230f225a875e69aaa17f1e71c6be5c831f22663bac63d0c7a9635edb0"
		"043ff8c6f26470f02a7bc56556f1437f06dfa27b487a6c4290d8bad38d4879b334e341ba09"
		"2dde4e4ae694a9c09302e2dbf443581c08",
		"666036d9b4a2426ed6585a4e0fd931a8761451d29ab04bd7dc6d0c5b9e38e6c2b263ff6cb8"
		"37bd04399de3d757c6c7005f6d7a987063cf6d7e8cb38a4bf0d74a282572bd01d0f41e3fd0"
		"66e3021575f0fa04f27b700d5b7ddddf50965993c3f9c7118ed78888da7cb221849b326059"
		"2b8e632d7c51e935a0ceae15207bedd548",
		"7e80436bce57339ce8da1b5660149a20240b146d108deef3ec5da4ae256f8f894edcbbc57b"
		"34ce37089c0daa17f0c46cd82b5a1599314fd79d2fd2f446bd5a25b8e32fcf05b76d644573"
		"a6df4ad1dfea707b479d97237a346f1ec632ea5660efb57e8717a8628d7f82af50a4e84b11"
		"f21bdff6839196a880ae20b2a0918d58cd",
		"1669bfb657fdc62c3ddd63269787fc1c969f1850fb04c933dda063ef74a56ce13e3a649700"
		"820f0061efabf849a85d474326c8a541d99830eea8131eaea584f22d88c353965dabcdc4bf"
		"6b55949fd529507dfb803ab6b480cd73ca0ba00ca19c438849e2cea262a1c57d8f81cd257f"
		"b58e19dec7904da97d8386e87b84948169",
		"3fe60dd9ad6caccf5a6f583b3ae65953563446c4510b70da115ffaa0ba04c076115c7043ab"
		"8733403cd69c7d14c212c655c07b43a7c71b9a4cffe22c2684788ec6870dc2013f269172c8"
		"22256f9e7cc674791bf2d8486c0f5684283e1649576efc982ede17c7b74b214754d70402fb"
		"4bb45ad086cf2cf76b3d63f7fce39ac970",
		"983a71b9994d95e876d84d28946a041f8f0a3f544cfcc055496580f1dfd4e312a2ad418fe6"
		"9dbc61db230cc0c0ed97e360abab7d6ff4b81ee970a7e97466acfd9644f828ffec538abc38"
		"3d0e92326d1c88c55e1f46a668a039beaa1be631a89129938c00a81a3ae46d4aecbf9707f7"
		"64dbaccea3ef7665e4c4307fa0b0a3075c",
		"4a8c071ac4fd0d52faa407b0fe5dab759f7394a5832127f2a3498f34aac287339e043b4ffa"
		"79528faf199dc917f7b066ad65505dab0e11e6948515052ce20cfdb892ffb8aa9bf3f1aa5b"
		"e30a5bbe85823bddf70b39fd7ebd4a93a2f75472c1d4f606247a9821f1a8c45a6cb80545de"
		"2e0c6c0174e2392088c754e9c8443eb5af",
		"0a3a12c3084c865daf1d302c78215d39bfe0b8bf28272b3c0b74beb4b7409db0718239de70"
		"0785581514321c6440a4bbaea4c76fa47401e151e68cb6c29017f0bce4631290af5ea5e2bf"
		"3ed742ae110b04ade83a5dbd7358f29a85938e23d87ac8233072b79c94670ff0959f9c7f45"
		"17862ff829452096c78f5f2e9a7e4e9216",
		"785d07a3c54f63dca11f5d1a5f496ee2c2f9288e55007e666c78b007d95cc28581dce51f49"
		"0b30fa73dc9e2d45d075d7e3a95fb8a9e1465ad191904124160b7c60fa720ef4ef1c5d2998"
		"f40570ae2a870ef3e894c2bc617d8a1dc85c3c55774928c38789b4e661349d3f84d2441a3b"
		"856a76949b9f1f80bc161648a1cad5588e",
		"76f987ec5448dd72219bd30bf6b66b0775c80b394851a43ff1f537f140a6e7229ef8cd72ad"
		"58b1d2d20298539d6347dd5598812bc65323aceaf05228f738b5ad3e8d9fe4100fd767c2f0"
		"98c77cb99c2992843ba3eed91d32444f3b6db6cd212dd4e5609548f4bb62812a920f6e2bf1"
		"581be1ebeebdd06ec4e971862cc42055ca",
		"60cd64b2cd2be6c33859b94875120361a24085f3765cb8b2bf11e026fa9d8855dbe435acf7"
		"882e84f3c7857f96e2baab4d9afe4588e4a82e17a78827bfdb5ddbd1c211fbc2e6d884cddd"
		"7cb9d90d5bf4a7311b83f352508033812c776a0e00c003c7e0d628e50736c7512df0acfa9f"
		"2320bd102229f46495ae6d0857cc452a84",
	};

	char *Qx[] = {
		"87f8f2b218f49845f6f10eec3877136269f5c1a54736dbdf69f89940cad41555",
		"5cf02a00d205bdfee2016f7421807fc38ae69e6b7ccd064ee689fc1a94a9f7d2",
		"2ddfd145767883ffbb0ac003ab4a44346d08fa2570b3120dcce94562422244cb",
		"e424dc61d4bb3cb7ef4344a7f8957a0c5134e16f7a67c074f82e6e12f49abf3c",
		"e0fc6a6f50e1c57475673ee54e3a57f9a49f3328e743bf52f335e3eeaa3d2864",
		"a849bef575cac3c6920fbce675c3b787136209f855de19ffe2e8d29b31a5ad86",
		"3dfb6f40f2471b29b77fdccba72d37c21bba019efa40c1c8f91ec405d7dcc5df",
		"69b7667056e1e11d6caf6e45643f8b21e7a4bebda463c7fdbc13bc98efbd0214",
		"bf02cbcf6d8cc26e91766d8af0b164fc5968535e84c158eb3bc4e2d79c3cc682",
		"224a4d65b958f6d6afb2904863efd2a734b31798884801fcab5a590f4d6da9de",
		"43691c7795a57ead8c5c68536fe934538d46f12889680a9cb6d055a066228369",
		"9157dbfcf8cf385f5bb1568ad5c6e2a8652ba6dfc63bc1753edf5268cb7eb596",
		"072b10c081a4c1713a294f248aef850e297991aca47fa96a7470abe3b8acfdda",
		"09308ea5bfad6e5adf408634b3d5ce9240d35442f7fe116452aaec0d25be8c24",
		"2d98ea01f754d34bbc3003df5050200abf445ec728556d7ed7d5c54c55552b6d",
	};

	char *Qy[] = {
		"e15f369036f49842fac7a86c8a2b0557609776814448b8f5e84aa9f4395205e9",
		"ec530ce3cc5c9d1af463f264d685afe2b4db4b5828d7e61b748930f3ce622a85",
		"5f70c7d11ac2b7a435ccfbbae02c3df1ea6b532cc0e9db74f93fffca7c6f9a64",
		"970eed7aa2bc48651545949de1dddaf0127e5965ac85d1243d6f60e7dfaee927",
		"7f59d689c91e463607d9194d99faf316e25432870816dde63f5d4b373f12f22a",
		"bf5fe4f7858f9b805bd8dcc05ad5e7fb889de2f822f3d8b41694e6c55c16b471",
		"f22f953f1e395a52ead7f3ae3fc47451b438117b1e04d613bc8555b7d6e6d1bb",
		"d3f9b12eb46c7c6fda0da3fc85bc1fd831557f9abc902a3be3cb3e8be7d1aa2f",
		"069ba6cb06b49d60812066afa16ecf7b51352f2c03bd93ec220822b1f3dfba03",
		"178d51fddada62806f097aa615d33b8f2404e6b1479f5fd4859d595734d6d2b9",
		"f8790110b3c3b281aa1eae037d4f1234aff587d903d93ba3af225c27ddc9ccac",
		"972570f4313d47fc96f7c02d5594d77d46f91e949808825b3d31f029e8296405",
		"9581145cca04a0fb94cedce752c8f0370861916d2a94e7c647c5373ce6a4c8f5",
		"f40c93e023ef494b1c3079b2d10ef67f3170740495ce2cc57f8ee4b0618b8ee5",
		"9b52672742d637a32add056dfd6d8792f2a33c2e69dafabea09b960bc61e230a",
	};

	char *R[] = {
		"d19ff48b324915576416097d2544f7cbdf8768b1454ad20e0baac50e211f23b0",
		"dc23d130c6117fb5751201455e99f36f59aba1a6a21cf2d0e7481a97451d6693",
		"9913111cff6f20c5bf453a99cd2c2019a4e749a49724a08774d14e4c113edda8",
		"bf96b99aa49c705c910be33142017c642ff540c76349b9dab72f981fd9347f4f",
		"1d75830cd36f4c9aa181b2c4221e87f176b7f05b7c87824e82e396c88315c407",
		"25acc3aa9d9e84c7abf08f73fa4195acc506491d6fc37cb9074528a7db87b9d6",
		"548886278e5ec26bed811dbb72db1e154b6f17be70deb1b210107decb1ec2a5a",
		"288f7a1cd391842cce21f00e6f15471c04dc182fe4b14d92dc18910879799790",
		"f5acb06c59c2b4927fb852faa07faf4b1852bbb5d06840935e849c4d293d1bad",
		"87b93ee2fecfda54deb8dff8e426f3c72c8864991f8ec2b3205bb3b416de93d2",
		"8acd62e8c262fa50dd9840480969f4ef70f218ebf8ef9584f199031132c6b1ce",
		"dfaea6f297fa320b707866125c2a7d5d515b51a503bee817de9faa343cc48eeb",
		"09f5483eccec80f9d104815a1be9cc1a8e5b12b6eb482a65c6907b7480cf4f19",
		"5cc8aa7c35743ec0c23dde88dabd5e4fcd0192d2116f6926fef788cddb754e73",
		"06108e525f845d0155bf60193222b3219c98e3d49424c2fb2a0987f825c17959",
	};

	char *Result[] = {
		"F (3 - S changed)", "F (2 - R changed)",       "F (4 - Q changed)",
		"P (0 )",            "P (0 )",                  "F (2 - R changed)",
		"F (4 - Q changed)", "F (1 - Message changed)", "F (3 - S changed)",
		"F (2 - R changed)", "F (3 - S changed)",       "F (1 - Message changed)",
		"F (4 - Q changed)", "F (1 - Message changed)", "P (0 )",
	};

	char *S[] = {
		"a3e81e59311cdfff2d4784949f7a2cb50ba6c3a91fa54710568e61aca3e847c6",
		"d6ce7708c18dbf35d4f8aa7240922dc6823f2e7058cbc1484fcad1599db5018c",
		"9467cd4cd21ecb56b0cab0a9a453b43386845459127a952421f5c6382866c5cc",
		"17c55095819089c2e03b9cd415abdf12444e323075d98f31920b9e0f57ec871c",
		"cb2acb01dac96efc53a32d4a0d85d0c2e48955214783ecf50a4f0414a319c05a",
		"9b21d5b5259ed3f2ef07dfec6cc90d3a37855d1ce122a85ba6a333f307d31537",
		"e93bfebd2f14f3d827ca32b464be6e69187f5edbd52def4f96599c37d58eee75",
		"247b3c4e89a3bcadfea73c7bfd361def43715fa382b8c3edf4ae15d6e55e9979",
		"049dab79c89cc02f1484c437f523e080a75f134917fda752f2d5ca397addfe5d",
		"4044a24df85be0cc76f21a4430b75b8e77b932a87f51e4eccbc45c263ebf8f66",
		"cfca7ed3d4347fb2a29e526b43c348ae1ce6c60d44f3191b6d8ea3a2d9c92154",
		"8f780ad713f9c3e5a4f7fa4c519833dfefc6a7432389b1e4af463961f09764f2",
		"a4f90e560c5e4eb8696cb276e5165b6a9d486345dedfb094a76e8442d026378d",
		"9c9c045ebaa1b828c32f82ace0d18daebf5e156eb7cbfdc1eff4399a8a900ae7",
		"62b5cdd591e5b507e560167ba8f6f7cda74673eb315680cb89ccbc4eec477dce",
	};

	struct tc_sha256_state_struct sha256_ctx;

	printf("Test #2: ECDSAvrfy ");
	printf("NIST-p256, SHA2-256\n");

	return vrfy_vectors(&sha256_ctx, Msg, Qx, Qy, R, S, Result, 15, verbose);
}

int montecarlo_signverify(int num_tests, bool verbose)
{
	printf("Test #3: Monte Carlo (%d Randomized EC-DSA signatures) ", num_tests);
	printf("NIST-p256, SHA2-256\n  ");
	int i;
	uint8_t private[NUM_ECC_BYTES];
	uint8_t public[2*NUM_ECC_BYTES];
	uint8_t hash[NUM_ECC_BYTES];
	unsigned int hash_words[NUM_ECC_WORDS];
	uint8_t sig[2*NUM_ECC_BYTES];

	const struct uECC_Curve_t * curve = uECC_secp256r1();

	for (i = 0; i < num_tests; ++i) {
		if (verbose) {
			TC_PRINT(".");
			fflush(stdout);
		}

		uECC_generate_random_int(hash_words, curve->n, BITS_TO_WORDS(curve->num_n_bits));
		uECC_vli_nativeToBytes(hash, NUM_ECC_BYTES, hash_words);

		if (!uECC_make_key(public, private, curve)) {
			TC_ERROR("uECC_make_key() failed\n");
			return TC_FAIL;
		}

		if (!uECC_sign(private, hash, sizeof(hash), sig, curve)) {
			TC_ERROR("uECC_sign() failed\n");
			return TC_FAIL;
		}

		if (!uECC_verify(public, hash, sizeof(hash), sig, curve)) {
			TC_ERROR("uECC_verify() failed\n");
			return TC_FAIL;
		}
		if (verbose) {
			fflush(stdout);
			printf(".");
		}
	}
	TC_PRINT("\n");
	return TC_PASS;
}

int main()
{
	unsigned int result = TC_PASS;

	TC_START("Performing ECC-DSA tests:");
	/* Setup of the Cryptographically Secure PRNG. */
	uECC_set_rng(&default_CSPRNG);

	bool verbose = true;

	TC_PRINT("Performing cavp_sign test:\n");
	result = cavp_sign(verbose);
	if (result == TC_FAIL) { /* terminate test */
		TC_ERROR("cavp_sign test failed.\n");
		goto exitTest;
	}
	TC_PRINT("Performing cavp_verify test:\n");
	result = cavp_verify(verbose);
	if (result == TC_FAIL) {
		TC_ERROR("cavp_verify test failed.\n");
		goto exitTest;
	}
	TC_PRINT("Performing montecarlo_signverify test:\n");
	result = montecarlo_signverify(10, verbose);
	if (result == TC_FAIL) {
		TC_ERROR("montecarlo_signverify test failed.\n");
	goto exitTest;
	}

	TC_PRINT("\nAll ECC-DSA tests succeeded.\n");

 exitTest:
        TC_END_RESULT(result);
        TC_END_REPORT(result);
}
