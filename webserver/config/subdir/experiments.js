$(document).ready(function() {
	$("#act").click(function() {
		$.post(
			"../empty",
			{},
			function(xml) {
				$("#results").html(xml);
			});
	});
});
