$(() => {
	var urlTimeout = null;
	function setURL(query, value) {
		clearTimeout(urlTimeout);

		urlTimeout = setTimeout(function() {
			var url = new URL(window.location.href);
			console.log(query,value,value.length,!value.length);
			if (!value.length) {
				console.log
				url.searchParams.delete(query);
			} else if (Array.isArray(value)) {
				url.searchParams.delete(query);
				value.forEach(function(v) {
					url.searchParams.append(query, v);
				})
			} else {
				url.searchParams.set(query, value);
			}

			window.history.pushState(null, document.title, url.href);
		}, 1000);
	}

	function handlePageLoad() {
		var url = new URL(window.location.href);
		//get values from URL
		var filters = url.searchParams.getAll('filter');
		search_terms = filters.join(" ");
		$("#support-matrix-filter").val(search_terms);
		run_filter();
	}

	function filter_boards(search_string) {
		$(".board_hidden").removeClass("board_hidden");
		$(".this_module").removeClass("this_module");
		var nboards = $(".support-matrix-table tbody tr").length;
		if(search_string.trim() == "") {
			$("#support-matrix-filter-num").html("(all)");
			setURL("filter",[]);
			return;
		}
		var list_search = search_string.split(" ").filter(i => i);
		var nvisible = 0;
		$(".support-matrix-table tbody tr").each( (index,item) => {
			var name = $(item).find("td:first-child p").html();
			var modules = $(item).find("a.reference.internal");
			var matching_all = true;
			//
			list_search.forEach((sstring) => {
				var matching = (sstring[0] == "-");
				for(var modi = 0; modi < modules.length; ++modi) {
					module = modules[modi];
					var mod_name = module.firstChild.firstChild.textContent;
					if(sstring[0] == "-") {
						if(mod_name.match(sstring.substr(1))) {
							matching = false;
							break;
						}
					} else {
						if(mod_name.match(sstring)) {
							$(module).addClass("this_module");
							matching = true;
						}
					}
				}
				matching_all = matching_all && matching;
			});
			if(!matching_all) {
				$(item).addClass("board_hidden");
			} else {
				nvisible += 1;
			}
		});
		$("#support-matrix-filter-num").html(`(${nvisible}/${nboards})`);
		setURL("filter",list_search);
	}

	function run_filter() {
		var search_string = $("#support-matrix-filter").val();
		filter_boards(search_string);
	}
	$("#support-matrix-filter").on("keyup", run_filter);
	// $(document).on("keyup", "#support-matrix-filter", run_filter);
	handlePageLoad();
});
