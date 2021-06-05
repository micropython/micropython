# Configure sources for libm
if(${MICROPY_FLOAT_IMPL} EQUAL double)
    set(MICROPY_SOURCE_LIB_M
        __cos.c
        __expo2.c
        __fpclassify.c
        __rem_pio2.c
        __rem_pio2_large.c
        __signbit.c
        __sin.c
        __tan.c
        acos.c
        acosh.c
        asin.c
        asinh.c
        atan.c
        atan2.c
        atanh.c
        ceil.c
        cos.c
        cosh.c
        copysign.c
        erf.c
        exp.c
        expm1.c
        floor.c
        fmod.c
        frexp.c
        ldexp.c
        lgamma.c
        log.c
        log10.c
        log1p.c
        modf.c
        nearbyint.c
        pow.c
        rint.c
        round.c
        scalbn.c
        sin.c
        sinh.c
        tan.c
        tanh.c
        tgamma.c
        trunc.c
    )

    list(TRANSFORM MICROPY_SOURCE_LIB_M PREPEND ${MICROPY_DIR}/lib/libm_dbl/)

    if(${SUPPORTS_HARDWARE_FP_DOUBLE} EQUAL 1)
        list(APPEND MICROPY_SOURCE_LIB_M ${MICROPY_DIR}/lib/libm_dbl/thumb_vfp_sqrt.c)
    else()
        list(APPEND MICROPY_SOURCE_LIB_M ${MICROPY_DIR}/lib/libm_dbl/sqrt.c)
    endif()
else()
    set(MICROPY_SOURCE_LIB_M
        math.c
        acoshf.c
        asinfacosf.c
        asinhf.c
        atan2f.c
        atanf.c
        atanhf.c
        ef_rem_pio2.c
        erf_lgamma.c
        fmodf.c
        kf_cos.c
        kf_rem_pio2.c
        kf_sin.c
        kf_tan.c
        log1pf.c
        nearbyintf.c
        roundf.c
        sf_cos.c
        sf_erf.c
        sf_frexp.c
        sf_ldexp.c
        sf_modf.c
        sf_sin.c
        sf_tan.c
        wf_lgamma.c
        wf_tgamma.c
    )

    list(TRANSFORM MICROPY_SOURCE_LIB_M PREPEND ${MICROPY_DIR}/lib/libm/)    

    if(${SUPPORTS_HARDWARE_FP_SINGLE} EQUAL 1)
        list(APPEND MICROPY_SOURCE_LIB_M ${MICROPY_DIR}/lib/libm/thumb_vfp_sqrtf.c)
    else()
        list(APPEND MICROPY_SOURCE_LIB_M ${MICROPY_DIR}/lib/libm/ef_sqrt.c)
    endif()
endif()

add_library(libm OBJECT ${MICROPY_SOURCE_LIB_M})