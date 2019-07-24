/* test_ecc_dh.c - TinyCrypt implementation of some EC-DH tests */

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
 *  test_ecc_ecdh.c -- Implementation of some EC-DH tests
 *
 */
#include <tinycrypt/ecc.h>
#include <tinycrypt/ecc_dh.h>
#include <tinycrypt/ecc_platform_specific.h>
#include <test_ecc_utils.h>
#include <test_utils.h>
#include <tinycrypt/constants.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int ecdh_vectors(char **qx_vec, char **qy_vec, char **d_vec, char **z_vec,
		  int tests, int verbose)
{

	unsigned int pub[2*NUM_ECC_WORDS];
	unsigned int prv[NUM_ECC_WORDS];
	unsigned int z[NUM_ECC_WORDS];
	unsigned int result = TC_PASS;

	int rc;
	unsigned int exp_z[NUM_ECC_WORDS];

	const struct uECC_Curve_t * curve = uECC_secp256r1();

	for (int i = 0;  i < tests; i++) {

		string2scalar(pub + NUM_ECC_WORDS, NUM_ECC_WORDS, qx_vec[i]);
		string2scalar(pub, NUM_ECC_WORDS, qy_vec[i]);
		string2scalar(exp_z, NUM_ECC_WORDS, z_vec[i]);
	    	string2scalar(prv, NUM_ECC_WORDS, d_vec[i]);

		uint8_t pub_bytes[2*NUM_ECC_BYTES];
		uECC_vli_nativeToBytes(pub_bytes, 2*NUM_ECC_BYTES, pub);
		uint8_t private_bytes[NUM_ECC_BYTES];
		uECC_vli_nativeToBytes(private_bytes, NUM_ECC_BYTES, prv);
		uint8_t z_bytes[NUM_ECC_BYTES];
		uECC_vli_nativeToBytes(z_bytes, NUM_ECC_BYTES, exp_z);

		rc = uECC_shared_secret(pub_bytes, private_bytes, z_bytes, curve);

		if (rc == TC_CRYPTO_FAIL) {
			TC_ERROR("ECDH failure, exit.\n");
			result = TC_FAIL;
			return result;;
		}

		uECC_vli_bytesToNative(z, z_bytes, NUM_ECC_BYTES);

		result = check_ecc_result(i, "Z", exp_z, z, NUM_ECC_WORDS, verbose);
		if (result == TC_FAIL) {
		  return result;
		}
  	}
	return result;
}

int cavp_ecdh(bool verbose)
{
  unsigned int result = TC_PASS;
	/*
	 * P-256
	 */
	char *d[] = {
		"7d7dc5f71eb29ddaf80d6214632eeae03d9058af1fb6d22ed80badb62bc1a534",
		"38f65d6dce47676044d58ce5139582d568f64bb16098d179dbab07741dd5caf5",
		"1accfaf1b97712b85a6f54b148985a1bdc4c9bec0bd258cad4b3d603f49f32c8",
		"207c43a79bfee03db6f4b944f53d2fb76cc49ef1c9c4d34d51b6c65c4db6932d",
		"59137e38152350b195c9718d39673d519838055ad908dd4757152fd8255c09bf",
		"f5f8e0174610a661277979b58ce5c90fee6c9b3bb346a90a7196255e40b132ef",
		"3b589af7db03459c23068b64f63f28d3c3c6bc25b5bf76ac05f35482888b5190",
		"d8bf929a20ea7436b2461b541a11c80e61d826c0a4c9d322b31dd54e7f58b9c8",
		"0f9883ba0ef32ee75ded0d8bda39a5146a29f1f2507b3bd458dbea0b2bb05b4d",
		"2beedb04b05c6988f6a67500bb813faf2cae0d580c9253b6339e4a3337bb6c08",
		"77c15dcf44610e41696bab758943eff1409333e4d5a11bbe72c8f6c395e9f848",
		"42a83b985011d12303db1a800f2610f74aa71cdf19c67d54ce6c9ed951e9093e",
		"ceed35507b5c93ead5989119b9ba342cfe38e6e638ba6eea343a55475de2800b",
		"43e0e9d95af4dc36483cdd1968d2b7eeb8611fcce77f3a4e7d059ae43e509604",
		"b2f3600df3368ef8a0bb85ab22f41fc0e5f4fdd54be8167a5c3cd4b08db04903",
		"4002534307f8b62a9bf67ff641ddc60fef593b17c3341239e95bdb3e579bfdc8",
		"4dfa12defc60319021b681b3ff84a10a511958c850939ed45635934ba4979147",
		"1331f6d874a4ed3bc4a2c6e9c74331d3039796314beee3b7152fcdba5556304e",
		"dd5e9f70ae740073ca0204df60763fb6036c45709bf4a7bb4e671412fad65da3",
		"5ae026cfc060d55600717e55b8a12e116d1d0df34af831979057607c2d9c2f76",
		"b601ac425d5dbf9e1735c5e2d5bdb79ca98b3d5be4a2cfd6f2273f150e064d9d",
		"fefb1dda1845312b5fce6b81b2be205af2f3a274f5a212f66c0d9fc33d7ae535",
		"334ae0c4693d23935a7e8e043ebbde21e168a7cba3fa507c9be41d7681e049ce",
		"2c4bde40214fcc3bfc47d4cf434b629acbe9157f8fd0282540331de7942cf09d",
		"85a268f9d7772f990c36b42b0a331adc92b5941de0b862d5d89a347cbf8faab0",
  	};

	char *x[] = {
		"700c48f77f56584c5cc632ca65640db91b6bacce3a4df6b42ce7cc838833d287",
		"809f04289c64348c01515eb03d5ce7ac1a8cb9498f5caa50197e58d43a86a7ae",
		"a2339c12d4a03c33546de533268b4ad667debf458b464d77443636440ee7fec3",
		"df3989b9fa55495719b3cf46dccd28b5153f7808191dd518eff0c3cff2b705ed",
		"41192d2813e79561e6a1d6f53c8bc1a433a199c835e141b05a74a97b0faeb922",
		"33e82092a0f1fb38f5649d5867fba28b503172b7035574bf8e5b7100a3052792",
		"6a9e0c3f916e4e315c91147be571686d90464e8bf981d34a90b6353bca6eeba7",
		"a9c0acade55c2a73ead1a86fb0a9713223c82475791cd0e210b046412ce224bb",
		"94e94f16a98255fff2b9ac0c9598aac35487b3232d3231bd93b7db7df36f9eb9",
		"e099bf2a4d557460b5544430bbf6da11004d127cb5d67f64ab07c94fcdf5274f",
		"f75a5fe56bda34f3c1396296626ef012dc07e4825838778a645c8248cff01658",
		"2db4540d50230756158abf61d9835712b6486c74312183ccefcaef2797b7674d",
		"cd94fc9497e8990750309e9a8534fd114b0a6e54da89c4796101897041d14ecb",
		"15b9e467af4d290c417402e040426fe4cf236bae72baa392ed89780dfccdb471",
		"49c503ba6c4fa605182e186b5e81113f075bc11dcfd51c932fb21e951eee2fa1",
		"19b38de39fdd2f70f7091631a4f75d1993740ba9429162c2a45312401636b29c",
		"2c91c61f33adfe9311c942fdbff6ba47020feff416b7bb63cec13faf9b099954",
		"a28a2edf58025668f724aaf83a50956b7ac1cfbbff79b08c3bf87dfd2828d767",
		"a2ef857a081f9d6eb206a81c4cf78a802bdf598ae380c8886ecd85fdc1ed7644",
		"ccd8a2d86bc92f2e01bce4d6922cf7fe1626aed044685e95e2eebd464505f01f",
		"c188ffc8947f7301fb7b53e36746097c2134bf9cc981ba74b4e9c4361f595e4e",
		"317e1020ff53fccef18bf47bb7f2dd7707fb7b7a7578e04f35b3beed222a0eb6",
		"45fb02b2ceb9d7c79d9c2fa93e9c7967c2fa4df5789f9640b24264b1e524fcb1",
		"a19ef7bff98ada781842fbfc51a47aff39b5935a1c7d9625c8d323d511c92de6",
		"356c5a444c049a52fee0adeb7e5d82ae5aa83030bfff31bbf8ce2096cf161c4b",
	};

	char *y[] = {
		"db71e509e3fd9b060ddb20ba5c51dcc5948d46fbf640dfe0441782cab85fa4ac",
		"b29d84e811197f25eba8f5194092cb6ff440e26d4421011372461f579271cda3",
		"ef48a3ab26e20220bcda2c1851076839dae88eae962869a497bf73cb66faf536",
		"422294ff46003429d739a33206c8752552c8ba54a270defc06e221e0feaf6ac4",
		"1af98cc45e98a7e041b01cf35f462b7562281351c8ebf3ffa02e33a0722a1328",
		"f2cf6b601e0a05945e335550bf648d782f46186c772c0f20d3cd0d6b8ca14b2f",
		"40f9bead39c2f2bcc2602f75b8a73ec7bdffcbcead159d0174c6c4d3c5357f05",
		"f6de0afa20e93e078467c053d241903edad734c6b403ba758c2b5ff04c9d4229",
		"d8049a43579cfa90b8093a94416cbefbf93386f15b3f6e190b6e3455fedfe69a",
		"d9c50dbe70d714edb5e221f4e020610eeb6270517e688ca64fb0e98c7ef8c1c5",
		"33bbdf1b1772d8059df568b061f3f1122f28a8d819167c97be448e3dc3fb0c3c",
		"62f57f314e3f3495dc4e099012f5e0ba71770f9660a1eada54104cdfde77243e",
		"c3def4b5fe04faee0a11932229fff563637bfdee0e79c6deeaf449f85401c5c4",
		"cdf4e9170fb904302b8fd93a820ba8cc7ed4efd3a6f2d6b05b80b2ff2aee4e77",
		"8af706ff0922d87b3f0c5e4e31d8b259aeb260a9269643ed520a13bb25da5924",
		"09aed7232b28e060941741b6828bcdfa2bc49cc844f3773611504f82a390a5ae",
		"6cab31b06419e5221fca014fb84ec870622a1b12bab5ae43682aa7ea73ea08d0",
		"dfa7bfffd4c766b86abeaf5c99b6e50cb9ccc9d9d00b7ffc7804b0491b67bc03",
		"563c4c20419f07bc17d0539fade1855e34839515b892c0f5d26561f97fa04d1a",
		"e9ddd583a9635a667777d5b8a8f31b0f79eba12c75023410b54b8567dddc0f38",
		"bf7d2f2056e72421ef393f0c0f2b0e00130e3cac4abbcc00286168e85ec55051",
		"09420ce5a19d77c6fe1ee587e6a49fbaf8f280e8df033d75403302e5a27db2ae",
		"5c6e8ecf1f7d3023893b7b1ca1e4d178972ee2a230757ddc564ffe37f5c5a321",
		"e9c184df75c955e02e02e400ffe45f78f339e1afe6d056fb3245f4700ce606ef",
		"57d128de8b2a57a094d1a001e572173f96e8866ae352bf29cddaf92fc85b2f92",
	};

	char *Z[] = {
		"46fc62106420ff012e54a434fbdd2d25ccc5852060561e68040dd7778997bd7b",
		"057d636096cb80b67a8c038c890e887d1adfa4195e9b3ce241c8a778c59cda67",
		"2d457b78b4614132477618a5b077965ec90730a8c81a1c75d6d4ec68005d67ec",
		"96441259534b80f6aee3d287a6bb17b5094dd4277d9e294f8fe73e48bf2a0024",
		"19d44c8d63e8e8dd12c22a87b8cd4ece27acdde04dbf47f7f27537a6999a8e62",
		"664e45d5bba4ac931cd65d52017e4be9b19a515f669bea4703542a2c525cd3d3",
		"ca342daa50dc09d61be7c196c85e60a80c5cb04931746820be548cdde055679d",
		"35aa9b52536a461bfde4e85fc756be928c7de97923f0416c7a3ac8f88b3d4489",
		"605c16178a9bc875dcbff54d63fe00df699c03e8a888e9e94dfbab90b25f39b4",
		"f96e40a1b72840854bb62bc13c40cc2795e373d4e715980b261476835a092e0b",
		"8388fa79c4babdca02a8e8a34f9e43554976e420a4ad273c81b26e4228e9d3a3",
		"72877cea33ccc4715038d4bcbdfe0e43f42a9e2c0c3b017fc2370f4b9acbda4a",
		"e4e7408d85ff0e0e9c838003f28cdbd5247cdce31f32f62494b70e5f1bc36307",
		"ed56bcf695b734142c24ecb1fc1bb64d08f175eb243a31f37b3d9bb4407f3b96",
		"bc5c7055089fc9d6c89f83c1ea1ada879d9934b2ea28fcf4e4a7e984b28ad2cf",
		"9a4e8e657f6b0e097f47954a63c75d74fcba71a30d83651e3e5a91aa7ccd8343",
		"3ca1fc7ad858fb1a6aba232542f3e2a749ffc7203a2374a3f3d3267f1fc97b78",
		"1aaabe7ee6e4a6fa732291202433a237df1b49bc53866bfbe00db96a0f58224f",
		"430e6a4fba4449d700d2733e557f66a3bf3d50517c1271b1ddae1161b7ac798c",
		"1ce9e6740529499f98d1f1d71329147a33df1d05e4765b539b11cf615d6974d3",
		"4690e3743c07d643f1bc183636ab2a9cb936a60a802113c49bb1b3f2d0661660",
		"30c2261bd0004e61feda2c16aa5e21ffa8d7e7f7dbf6ec379a43b48e4b36aeb0",
		"2adae4a138a239dcd93c243a3803c3e4cf96e37fe14e6a9b717be9599959b11c",
		"2e277ec30f5ea07d6ce513149b9479b96e07f4b6913b1b5c11305c1444a1bc0b",
		"1e51373bd2c6044c129c436e742a55be2a668a85ae08441b6756445df5493857",
	};

	TC_PRINT("Test #1: ECDH");
	TC_PRINT("NIST-p256\n");

	result = ecdh_vectors(x, y, d, Z, 25, verbose);
	if(result == TC_FAIL) {
	     goto exitTest1;
	}

 exitTest1:
        TC_END_RESULT(result);
        return result;
}

int cavp_keygen(bool verbose)
{
  unsigned int result = TC_PASS; 
	/*
	 * [P-256, B.4.2 Key Pair Generation by Testing Candidates]
	 */
	char *d[] = {
		"c9806898a0334916c860748880a541f093b579a9b1f32934d86c363c39800357",
		"710735c8388f48c684a97bd66751cc5f5a122d6b9a96a2dbe73662f78217446d",
		"78d5d8b7b3e2c16b3e37e7e63becd8ceff61e2ce618757f514620ada8a11f6e4",
		"2a61a0703860585fe17420c244e1de5a6ac8c25146b208ef88ad51ae34c8cb8c",
		"01b965b45ff386f28c121c077f1d7b2710acc6b0cb58d8662d549391dcf5a883",
		"fac92c13d374c53a085376fe4101618e1e181b5a63816a84a0648f3bdc24e519",
		"f257a192dde44227b3568008ff73bcf599a5c45b32ab523b5b21ca582fef5a0a",
		"add67e57c42a3d28708f0235eb86885a4ea68e0d8cfd76eb46134c596522abfd",
		"4494860fd2c805c5c0d277e58f802cff6d731f76314eb1554142a637a9bc5538",
		"d40b07b1ea7b86d4709ef9dc634c61229feb71abd63dc7fc85ef46711a87b210",
	};

	char *x[] = {
		"d0720dc691aa80096ba32fed1cb97c2b620690d06de0317b8618d5ce65eb728f",
		"f6836a8add91cb182d8d258dda6680690eb724a66dc3bb60d2322565c39e4ab9",
		"76711126cbb2af4f6a5fe5665dad4c88d27b6cb018879e03e54f779f203a854e",
		"e1aa7196ceeac088aaddeeba037abb18f67e1b55c0a5c4e71ec70ad666fcddc8",
		"1f038c5422e88eec9e88b815e8f6b3e50852333fc423134348fc7d79ef8e8a10",
		"7258f2ab96fc84ef6ccb33e308cd392d8b568ea635730ceb4ebd72fa870583b9",
		"d2e01411817b5512b79bbbe14d606040a4c90deb09e827d25b9f2fc068997872",
		"55bed2d9c029b7f230bde934c7124ed52b1330856f13cbac65a746f9175f85d7",
		"5190277a0c14d8a3d289292f8a544ce6ea9183200e51aec08440e0c1a463a4e4",
		"fbcea7c2827e0e8085d7707b23a3728823ea6f4878b24747fb4fd2842d406c73",
	};

	char *y[] = {
		"9681b517b1cda17d0d83d335d9c4a8a9a9b0b1b3c7106d8f3c72bc5093dc275f",
		"1f837aa32864870cb8e8d0ac2ff31f824e7beddc4bb7ad72c173ad974b289dc2",
		"a26df39960ab5248fd3620fd018398e788bd89a3cea509b352452b69811e6856",
		"d7d35bdce6dedc5de98a7ecb27a9cd066a08f586a733b59f5a2cdb54f971d5c8",
		"43a047cb20e94b4ffb361ef68952b004c0700b2962e0c0635a70269bc789b849",
		"489807ca55bdc29ca5c8fe69b94f227b0345cccdbe89975e75d385cc2f6bb1e2",
		"503f138f8bab1df2c4507ff663a1fdf7f710e7adb8e7841eaa902703e314e793",
		"32805e311d583b4e007c40668185e85323948e21912b6b0d2cda8557389ae7b0",
		"ecd98514821bd5aaf3419ab79b71780569470e4fed3da3c1353b28fe137f36eb",
		"2393c85f1f710c5afc115a39ba7e18abe03f19c9d4bb3d47d19468b818efa535",
	};

	TC_PRINT("Test #2: ECC KeyGen ");
	TC_PRINT("NIST-p256\n");

 	result = keygen_vectors(d, x, y, 10, verbose);
	if(result == TC_FAIL) {
	     goto exitTest1;
	}

 exitTest1:
        TC_END_RESULT(result);
        return result;
}

/* Test ecc_make_keys, and also as keygen part of other tests */
int pkv_vectors(char **qx_vec, char **qy_vec, char **res_vec, int tests, 
		 bool verbose)
{

	unsigned int pub[2 * NUM_ECC_WORDS];
	uint8_t _public[2 * NUM_ECC_BYTES];
	int rc;
	int exp_rc;
	char tmp;
	unsigned int result = TC_PASS;
	const struct uECC_Curve_t * curve = uECC_secp256r1();

	for (int i = 0; i < tests; i++)  {

		if (2 != sscanf(res_vec[i], "%c (%d ", &tmp, &exp_rc)) {
			TC_ERROR("Error: failed to parse CAVP response.\n");
			result = TC_FAIL;
			goto exitTest1;
		}

	if (strlen(qx_vec[i]) > 2 * NUM_ECC_BYTES ||
		strlen(qy_vec[i]) > 2 * NUM_ECC_BYTES) {
		/* invalid input to ECC digit conversion (string2native()) */
		rc = -2;
	} else {
		string2scalar(pub, NUM_ECC_WORDS, qx_vec[i]);
		string2scalar(pub + NUM_ECC_WORDS, NUM_ECC_WORDS, qy_vec[i]);

		uECC_vli_nativeToBytes(_public, NUM_ECC_BYTES, pub);
		uECC_vli_nativeToBytes(_public + NUM_ECC_BYTES, NUM_ECC_BYTES, pub+NUM_ECC_WORDS);

		rc = uECC_valid_public_key(_public, curve);
	}

	/*
	 * map to CAVP error codes
 	 *  0 => 0 - success
	 * -1 => ? - (x,y) = (0,0) (not covered)
	 * -2 => 1 - out of bounds (pubverify or ecc import)
	 * -3 => 2 - not on curve
	 * -4 => ? - public key is the group generator
	 */

	if (rc == -3) rc = 2;
	if (rc == -2) rc = 1;

	result = check_code(i, res_vec[i], exp_rc, rc, verbose);
	if(result == TC_FAIL) {
	     goto exitTest1;
	}
  }

 exitTest1:
        TC_END_RESULT(result);
        return result;
}

int cavp_pkv(bool verbose)
{
	/*
	* [P-256]
	*/
	char *x[] = {
		"e0f7449c5588f24492c338f2bc8f7865f755b958d48edb0f2d0056e50c3fd5b7",
		"d17c446237d9df87266ba3a91ff27f45abfdcb77bfd83536e92903efb861a9a9",
		"17875397ae87369365656d490e8ce956911bd97607f2aff41b56f6f3a61989826",
		"f2d1c0dc0852c3d8a2a2500a23a44813ccce1ac4e58444175b440469ffc12273",
		"10b0ca230fff7c04768f4b3d5c75fa9f6c539bea644dffbec5dc796a213061b58",
		"2c1052f25360a15062d204a056274e93cbe8fc4c4e9b9561134ad5c15ce525da",
		"a40d077a87dae157d93dcccf3fe3aca9c6479a75aa2669509d2ef05c7de6782f",
		"2633d398a3807b1895548adbb0ea2495ef4b930f91054891030817df87d4ac0a",
		"14bf57f76c260b51ec6bbc72dbd49f02a56eaed070b774dc4bad75a54653c3d56",
		"2fa74931ae816b426f484180e517f5050c92decfc8daf756cd91f54d51b302f1",
		"f8c6dd3181a76aa0e36c2790bba47041acbe7b1e473ff71eee39a824dc595ff0",
		"7a81a7e0b015252928d8b36e4ca37e92fdc328eb25c774b4f872693028c4be38",
	};

	char *y[] = {
		"86d7e9255d0f4b6f44fa2cd6f8ba3c0aa828321d6d8cc430ca6284ce1d5b43a0",
		"1eabb6a349ce2cd447d777b6739c5fc066add2002d2029052c408d0701066231c",
		"980a3c4f61b9692633fbba5ef04c9cb546dd05cdec9fa8428b8849670e2fba92",
		"32bfe992831b305d8c37b9672df5d29fcb5c29b4a40534683e3ace23d24647dd",
		"f5edf37c11052b75f771b7f9fa050e353e464221fec916684ed45b6fead38205",
		"ced9783713a8a2a09eff366987639c625753295d9a85d0f5325e32dedbcada0b",
		"503d86b87d743ba20804fd7e7884aa017414a7b5b5963e0d46e3a9611419ddf3",
		"d6b2f738e3873cc8364a2d364038ce7d0798bb092e3dd77cbdae7c263ba618d2",
		"7a231a23bf8b3aa31d9600d888a0678677a30e573decd3dc56b33f365cc11236",
		"5b994346137988c58c14ae2152ac2f6ad96d97decb33099bd8a0210114cd1141",
		"9c965f227f281b3072b95b8daf29e88b35284f3574462e268e529bbdc50e9e52",
		"08862f7335147261e7b1c3d055f9a316e4cab7daf99cc09d1c647f5dd6e7d5bb",
	};

	char *Result[] = {
		"P (0 )",                          "F (1 - Q_x or Q_y out of range)",
		"F (1 - Q_x or Q_y out of range)", "F (2 - Point not on curve)",
		"F (1 - Q_x or Q_y out of range)", "P (0 )",
		"F (2 - Point not on curve)",      "P (0 )",
		"F (1 - Q_x or Q_y out of range)", "P (0 )",
		"F (2 - Point not on curve)",      "F (2 - Point not on curve)",
	};

	TC_PRINT("Test #3: PubKeyVerify ");
	TC_PRINT("NIST-p256-SHA2-256\n");

	return pkv_vectors(x, y, Result, 12, verbose);
}

int montecarlo_ecdh(int num_tests, bool verbose) 
{
	int i;
	uint8_t private1[NUM_ECC_BYTES] = {0};
	uint8_t private2[NUM_ECC_BYTES] = {0};
	uint8_t public1[2*NUM_ECC_BYTES] = {0};
	uint8_t public2[2*NUM_ECC_BYTES] = {0};
	uint8_t secret1[NUM_ECC_BYTES] = {0};
	uint8_t secret2[NUM_ECC_BYTES] = {0};
        unsigned int result = TC_PASS;

	const struct uECC_Curve_t * curve = uECC_secp256r1();

	TC_PRINT("Test #4: Monte Carlo (%d Randomized EC-DH key-exchange) ", num_tests);
	TC_PRINT("NIST-p256\n  ");

	for (i = 0; i < num_tests; ++i) {
		if (verbose) {
			TC_PRINT(".");
			fflush(stdout);
		}

		if (!uECC_make_key(public1, private1, curve) ||
		    !uECC_make_key(public2, private2, curve)) {
			TC_ERROR("uECC_make_key() failed\n");
			result = TC_FAIL;
			goto exitTest1;
		}

		if (!uECC_shared_secret(public2, private1, secret1, curve)) {
			TC_ERROR("shared_secret() failed (1)\n");
			result = TC_FAIL;
			goto exitTest1;;
		}

		if (!uECC_shared_secret(public1, private2, secret2, curve)) {
			TC_ERROR("shared_secret() failed (2)\n");
			result = TC_FAIL;
			goto exitTest1;
		}

		if (memcmp(secret1, secret2, sizeof(secret1)) != 0) {
			TC_PRINT("Shared secrets are not identical!\n");
			TC_PRINT("Private key 1 = ");
			vli_print_bytes(private1, 32);
			TC_PRINT("\nPrivate key 2 = ");
			vli_print_bytes(private2, 32);
			TC_PRINT("\nPublic key 1 = ");
			vli_print_bytes(public1, 64);
			TC_PRINT("\nPublic key 2 = ");
			vli_print_bytes(public2, 64);
			TC_PRINT("\nShared secret 1 = ");
			vli_print_bytes(secret1, 32);
			TC_PRINT("\nShared secret 2 = ");
			vli_print_bytes(secret2, 32);
			TC_PRINT("\n");
		}
	}

	TC_PRINT("\n");

 exitTest1:
        TC_END_RESULT(result);
        return result;
}

int main()
{
        unsigned int result = TC_PASS;

	TC_START("Performing ECC-DH tests:");

	/* Setup of the Cryptographically Secure PRNG. */
	uECC_set_rng(&default_CSPRNG);

	bool verbose = true;

	TC_PRINT("Performing cavp_ecdh test:\n");
	result = cavp_ecdh(verbose);
        if (result == TC_FAIL) { /* terminate test */
                TC_ERROR("cavp_ecdh test failed.\n");
                goto exitTest;
        }
	TC_PRINT("Performing cavp_keygen test:\n");
	result = cavp_keygen(verbose);
        if (result == TC_FAIL) { /* terminate test */
                TC_ERROR("cavp_keygen test failed.\n");
                goto exitTest;
        }
	TC_PRINT("Performing cavp_pkv test:\n");
	result = cavp_pkv(verbose);
        if (result == TC_FAIL) { /* terminate test */
                TC_ERROR("cavp_pkv failed.\n");
                goto exitTest;
        }
	TC_PRINT("Performing montecarlo_ecdh test:\n");
	result = montecarlo_ecdh(10, verbose);
        if (result == TC_FAIL) { /* terminate test */
                TC_ERROR("montecarlo_ecdh test failed.\n");
                goto exitTest;
        }

        TC_PRINT("All EC-DH tests succeeded!\n");

 exitTest:
        TC_END_RESULT(result);
        TC_END_REPORT(result);
}
