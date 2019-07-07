/**
 * Add or update a query string parameter. If no URI is given, we use the current
 * window.location.href value for the URI.
 * 
 * Based on the DOM URL parser described here:
 * http://james.padolsey.com/javascript/parsing-urls-with-the-dom/
 *
 * @param   (string)    uri     Optional: The URI to add or update a parameter in
 * @param   (string)    key     The key to add or update
 * @param   (string)    value   The new value to set for key
 *
 * Tested on Chrome 34, Firefox 29, IE 7 and 11
 */
function update_query_string( uri, key, value ) {

	// Use window URL if no query string is provided
	if ( ! uri ) { uri = window.location.href; }

	// Create a dummy element to parse the URI with
	var a = document.createElement( 'a' ), 

		// match the key, optional square brackets, an equals sign or end of string, the optional value
		reg_ex = new RegExp( key + '((?:\\[[^\\]]*\\])?)(=|$)(.*)' ),

		// Setup some additional variables
		qs,
		qs_len,
		key_found = false;

	// Use the JS API to parse the URI 
	a.href = uri;

	// If the URI doesn't have a query string, add it and return
	if ( ! a.search ) {

		a.search = '?' + key + '=' + value;

		return a.href;
	}

	// Split the query string by ampersands
	qs = a.search.replace( /^\?/, '' ).split( /&(?:amp;)?/ );
	qs_len = qs.length; 

	// Loop through each query string part
	while ( qs_len > 0 ) {

		qs_len--;

		// Remove empty elements to prevent double ampersands
		if ( ! qs[qs_len] ) { qs.splice(qs_len, 1); continue; }

		// Check if the current part matches our key
		if ( reg_ex.test( qs[qs_len] ) ) {

			// Replace the current value
			qs[qs_len] = qs[qs_len].replace( reg_ex, key + '$1' ) + '=' + value;

			key_found = true;
		}
	}   

	// If we haven't replaced any occurrences above, add the new parameter and value
	if ( ! key_found ) { qs.push( key + '=' + value ); }

	// Set the new query string
	a.search = '?' + qs.join( '&' );

	return a.href;
}
