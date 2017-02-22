$(document).ready(function(){

	var updatePage = function() {
		$.ajax({
		 	url : "/rasp/"+RASPID,
		 	dataType : 'json',
		 	type : 'GET',
		 	success : function(data) {
		 		$('#raspName').text(data.name);
		 		$('#raspStatus')
		 			.btnColor(data.status)
		 			.text(CONSTANTS.raspStatus[data.status]);
		 		$('#raspOS').text(data.os);
		 		$('#raspI2C').text(data.address);
		 		$('#raspIP').text(data.ip);
		 	}
		});

		setTimeout(updatePage, 20000);
	}

	updatePage();
});