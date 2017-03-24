addAjaxHandler("/rasp/"+RASPID, function(rasp) {

	$('.stackLink').attr('href', '/view/stack/'+rasp.stack)

	$('.raspStack').text(rasp.stack);
	$('.raspName').text(rasp.name);
	$('#raspStack').text('#'+rasp.stack);
	$('#raspStatus')
		.btnColor(rasp.status)
		.text(CONSTANTS.raspStatus[rasp.status]);
	$('#raspOS').text(rasp.os);
	$('#raspIP').text(rasp.ip);
	$('#raspCPU').text(rasp.cpu);
	$('#raspRAM').text(rasp.ram);
}, true);

$('.statusBtn').click(function() {
	if($(this).text() == CONSTANTS.raspStatus[0]) {
		if(confirm("Allumer la Rasp ?")) {
			addAjaxHandler('/rasp/start/'+$(this).attr('raspId'), function() {
				$(this).text(CONSTANTS.raspStatus[1]).btnColor(1);
			}, false, 'POST');
		}
	}
	if($(this).text() == CONSTANTS.raspStatus[1]) {
		if(confirm("Eteindre la Rasp ?")) {
			addAjaxHandler('/rasp/start/'+$(this).attr('raspId'), function() {
				$(this).text(CONSTANTS.raspStatus[0]).btnColor(0);
			}, false, 'POST');
		}
	}
});
