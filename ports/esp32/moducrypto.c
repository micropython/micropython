/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Paul Sokolovsky
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/runtime.h"
#include "mbedtls/platform.h"
#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"  // 20180505 MHE add rsa_sign

//20180505 MHE------------------------------------------------------------------------------------------
STATIC mp_obj_t mod_crypto_rsa_sign(mp_obj_t key, mp_obj_t message) {

   
    int ret;
    mbedtls_pk_context pk;
    mbedtls_rsa_context *rsa;
    unsigned char hash[32];
    mbedtls_pk_init( &pk );

    // get the buffer to send from
    mp_buffer_info_t bufkey;
    mp_get_buffer_raise(key, &bufkey, MP_BUFFER_READ);
    
    // get the buffer to send from
    mp_buffer_info_t bufmessage;
    mp_get_buffer_raise(message, &bufmessage, MP_BUFFER_READ);
   
    ret = mbedtls_pk_parse_key(&pk, bufkey.buf, bufkey.len + 1, NULL, 0);
    if ( ret != 0 ) {
        mp_raise_ValueError("Unable to parse key");
        goto exit;
    }

    if( !mbedtls_pk_can_do( &pk, MBEDTLS_PK_RSA ) )
    {
        mp_raise_ValueError("Not an RSA key");
        goto exit;
    }
    
    if( ( ret = mbedtls_md(
                    mbedtls_md_info_from_type( MBEDTLS_MD_SHA256 ),
                    bufmessage.buf, bufmessage.len, hash ) ) != 0 )
    {
        mp_raise_ValueError("Unable to hash message");
        goto exit;
    }

    rsa = mbedtls_pk_rsa( pk );

    vstr_t signature;
    vstr_init_len(&signature, rsa->len);

    if( ( ret = mbedtls_rsa_pkcs1_sign(rsa , NULL, NULL, MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA256,
                                0, hash, (byte*)signature.buf ) ) != 0 )
    {
        mp_raise_ValueError("Unable to sign message");
        goto exit;
    }

    exit:
    // free all variables
    mbedtls_pk_free( &pk );
    mbedtls_rsa_free( rsa );

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &signature);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_crypto_rsa_sign_obj, mod_crypto_rsa_sign);
//20180505 MHE-end--------------------------------------------------------------------------------------

STATIC const mp_rom_map_elem_t mp_module_crypto_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ucrypto) },
    { MP_ROM_QSTR(MP_QSTR_rsa_sign), MP_ROM_PTR(&mod_crypto_rsa_sign_obj) }, //20180505 MHE
};

STATIC MP_DEFINE_CONST_DICT(mp_module_crypto_globals,
    mp_module_crypto_globals_table);

const mp_obj_module_t mp_module_ucrypto = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_crypto_globals,
};

