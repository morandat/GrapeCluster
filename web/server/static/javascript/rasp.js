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
	$('#raspCPU').text(rasp.cpu)
		.btnColor(rasp.cpu<CONSTANTS.raspCPULimit)
		.text(Math.round(rasp.cpu*100));
	$('#raspRAM').text(rasp.ram);
}, true);
