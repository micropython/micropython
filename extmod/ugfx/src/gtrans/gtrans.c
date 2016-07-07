/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include <string.h>
#include "../../gfx.h"

#if GFX_USE_GTRANS

static const transTable* _languageBase;
static const transTable* _languageCurrent;

void _gtransInit(void)
{
	_languageBase = 0;
	_languageCurrent = 0;
}

void _gtransDeinit(void)
{
}

const char* gtransString(const char* string)
{
	// Find the index of the specified string in the base language table
	size_t i = 0;
	while (1) {
		// Prevent overflow
		if (i >= _languageBase->numEntries) {
			return string;
		}

		// Check if we found the string
		if (strcmp(string, _languageBase->strings[i]) == 0) {
			break;
		}

		// Otherwise keep going
		i++;
	}

	// Make sure that the index exists in the current language table
	if (i >= _languageCurrent->numEntries) {
		return string;
	}

	// Return the translated string
	return _languageCurrent->strings[i];
}

const char* gtransIndex(unsigned index)
{
	if (!_languageCurrent) {
		return 0;
	}

	if (index >= _languageCurrent->numEntries) {
		return 0;
	}

	return _languageCurrent->strings[index];
}

void gtransSetBaseLanguage(const transTable* const translation)
{
	_languageBase = translation;
}

void gtransSetLanguage(const transTable* const translation)
{
	_languageCurrent = translation;
}

#endif /* GFX_USE_GTRANS */
