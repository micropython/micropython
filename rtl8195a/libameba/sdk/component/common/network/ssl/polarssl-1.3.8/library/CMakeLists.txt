option(USE_STATIC_POLARSSL_LIBRARY "Build PolarSSL static library." ON)
option(USE_SHARED_POLARSSL_LIBRARY "Build PolarSSL shared library." OFF)
option(LINK_WITH_PTHREAD "Explicitly link PolarSSL library to pthread." OFF)

set(src
     aes.c
     aesni.c
     arc4.c
     asn1parse.c
     asn1write.c
     base64.c
     bignum.c
     blowfish.c
     camellia.c
     ccm.c
     certs.c
     cipher.c
     cipher_wrap.c
     ctr_drbg.c
     debug.c
     des.c
     dhm.c
     ecp.c
     ecp_curves.c
     ecdh.c
     ecdsa.c
     entropy.c
     entropy_poll.c
     error.c
     gcm.c
     havege.c
     hmac_drbg.c
     md.c
     md_wrap.c
     md2.c
     md4.c
     md5.c
     memory_buffer_alloc.c
     net.c
     oid.c
     padlock.c
     pbkdf2.c
     pem.c
     pkcs5.c
     pkcs11.c
     pkcs12.c
     pk.c
     pk_wrap.c
     pkparse.c
     pkwrite.c
     platform.c
     ripemd160.c
     rsa.c
     sha1.c
     sha256.c
     sha512.c
     ssl_cache.c
     ssl_ciphersuites.c
     ssl_cli.c
     ssl_srv.c
     ssl_tls.c
     threading.c
     timing.c
     version.c
     version_features.c
     x509.c
     x509_crt.c
     x509_crl.c
     x509_csr.c
     x509_create.c
     x509write_crt.c
     x509write_csr.c
     xtea.c
)

if(WIN32)
set(libs ws2_32)
endif(WIN32)

if(CMAKE_COMPILER_IS_GNUCC)
  set(CMAKE_C_FLAGS_CHECK "${CMAKE_C_FLAGS_CHECK} -Wmissing-declarations -Wmissing-prototypes")
  set(CMAKE_C_FLAGS_CHECKFULL "${CMAKE_C_FLAGS_CHECK} -Wcast-qual")
endif(CMAKE_COMPILER_IS_GNUCC)

if(CMAKE_COMPILER_IS_CLANG)
  set(CMAKE_C_FLAGS_CHECK "${CMAKE_C_FLAGS_CHECK} -Wmissing-declarations -Wmissing-prototypes")
endif(CMAKE_COMPILER_IS_CLANG)

if (NOT USE_STATIC_POLARSSL_LIBRARY AND NOT USE_SHARED_POLARSSL_LIBRARY)
	message(FATAL_ERROR "Need to choose static or shared polarssl build!")
endif(NOT USE_STATIC_POLARSSL_LIBRARY AND NOT USE_SHARED_POLARSSL_LIBRARY)

if(USE_STATIC_POLARSSL_LIBRARY AND USE_SHARED_POLARSSL_LIBRARY)
	# if we build both static an shared, then let
	# tests and programs link to the shared lib target
	set(polarssl_static_target "polarssl_static")
elseif(USE_STATIC_POLARSSL_LIBRARY)
	set(polarssl_static_target "polarssl")
endif()

if(USE_STATIC_POLARSSL_LIBRARY)
	add_library(${polarssl_static_target} STATIC ${src})
	set_target_properties(${polarssl_static_target} PROPERTIES OUTPUT_NAME polarssl)
	target_link_libraries(${polarssl_static_target} ${libs})

	if(ZLIB_FOUND)
		target_link_libraries(${polarssl_static_target} ${ZLIB_LIBRARIES})
	endif(ZLIB_FOUND)

	if(LINK_WITH_PTHREAD)
        target_link_libraries(${polarssl_static_target} pthread)
	endif()

	install(TARGETS ${polarssl_static_target}
			DESTINATION ${LIB_INSTALL_DIR}
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif()

if(USE_SHARED_POLARSSL_LIBRARY)
	add_library(polarssl SHARED ${src})
	set_target_properties(polarssl PROPERTIES VERSION 1.3.8 SOVERSION 7)

	target_link_libraries(polarssl ${libs})

	if(ZLIB_FOUND)
		target_link_libraries(polarssl ${ZLIB_LIBRARIES})
	endif(ZLIB_FOUND)

	if(LINK_WITH_PTHREAD)
        target_link_libraries(polarssl pthread)
	endif()

	install(TARGETS polarssl
			DESTINATION ${LIB_INSTALL_DIR}
			PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
endif(USE_SHARED_POLARSSL_LIBRARY)
