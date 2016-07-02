/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/**
 * @file    src/gtrans/gtrans.h
 *
 * @addtogroup GTRANS
 *
 * @brief	Module that allows changing the language of an application dynamically during run-time.
 *
 * @{
 */

#ifndef _TRANS_H
#define _TRANS_H

#include "../../gfx.h"

#if GFX_USE_GTRANS || defined(__DOXYGEN__)

/**
 * @struct transTable
 * @brief A table containing translated strings.
 */
typedef struct transTable {
	unsigned numEntries;    /**< The number of strings that this table contains */
	const char** strings;	/**< The translated strings */
} transTable;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A wrapper macro to make writing and reading translatable applications easier.
 */
#define gt(str) gtransString(str)

/**
 * @brief Get the string of the current language specified by the string of the base language.
 *
 * @details This function will return the string of the current language that corresponds to
 *			the specified string in the base language.
 * @details This function uses strcmp() internally to compare strings.
 *
 * @param[in] string The string to translate.
 *
 * @return The corresponding string of the current language or the string passed as a parameter if it doesn't exist.
 */
const char* gtransString(const char* string);

/**
 * @brief Get the string at the specified index position of the current language.
 *
 * @details Getting translation strings is a lot faster using the index as an accessor rather
 *          than the string in the base language.
 *
 * @param[in] index The index of the string in the current language translation table.
 *
 * @return The string at the given index of the current language or 0 if it doesn't exist.
 */
const char* gtransIndex(unsigned index);

/**
 * @brief Set the base language.
 *
 * @details A translatable application needs to have a base language. All translations will
 *          be relative to this base language.
 *
 * @param[in] translation The translation table
 */
void gtransSetBaseLanguage(const transTable* const translation);

/**
 * @brief Set the current language.
 *
 * @details All translations will refer to the current language set by calling this function.
 *
 * @param[in] translation The translation table
 */
void gtransSetLanguage(const transTable* const translation);

#ifdef __cplusplus
}
#endif

#endif /* GFX_USE_GTRANS */

#endif /* _TRANS_H */
/** @} */
