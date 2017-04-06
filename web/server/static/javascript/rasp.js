addAjaxHandler("/rasp/"+RASPID, function(rasp) {

	$('.stackLink').attr('href', '/view/stack/'+rasp.stack)

	$('.raspStack').text(rasp.stack);
	$('.raspName').text(rasp.name);
	$('#raspId').text('0x'+RASPID.toString(16));
	$('#raspSSH').val('ssh '+rasp.ip);
	$('#raspStack').text('#'+rasp.stack);
	$('#raspOS').text(rasp.os);
	$('#raspIP').text(rasp.ip);
	$('#raspCPU')
		.text(rasp.cpu)
		.btnColor(rasp.cpu<CONSTANTS.raspCPULimit)
		.text(Math.round(rasp.cpu)+'%');
	$('#raspRAM')
		.text(rasp.ram)
		.btnColor(rasp.ram<CONSTANTS.raspRAMLimit)
		.text(Math.round(rasp.ram)+'%');
}, true);

$(document).ready(function() {
	$("#raspSSH").on("click", function () {
	   $(this).select();
	});

	$('#raspStatus').btnColor(false);
});