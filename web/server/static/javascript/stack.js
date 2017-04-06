addAjaxHandler("/stack/"+STACKID, function(stack) {

	$('#stackTemp')
		.btnColor(stack.heat<CONSTANTS.stackHeatLimit)
		.text(stack.heat+'°C');

	var raspAction = function(raspSlot, raspData) {
		if(raspData !== undefined) {
			$('#raspName'+raspSlot).html('<a href="/view/rasp/'+raspData.address+'" class="card-link">'+raspData.name+')</a>');
			$('#raspI2C'+raspSlot).text(' (0x'+raspData.address.toString(16));
			$('#raspOS'+raspSlot).text(raspData.os);
			$('#raspIP'+raspSlot).text(raspData.ip);
			$('#raspCPU'+raspSlot)
				.btnColor(raspData.cpu<CONSTANTS.raspCPULimit)
				.text(Math.round(raspData.cpu)+'%')
				.show();
			$('#raspRAM'+raspSlot)
				.btnColor(raspData.ram<CONSTANTS.raspRAMLimit)
				.text(Math.round(raspData.ram)+'%')
				.show();
			$('#raspStatus'+raspSlot)
				.btnColor(false)
				.attr('raspId', raspData.address)
				.show();
			$('#raspRestart'+raspSlot)
				.attr('raspId', raspData.address)
				.show();
		}
		else {
			$('#raspName'+raspSlot).html('---');
			$('#raspI2C'+raspSlot).text('');
			$('#raspOS'+raspSlot).text('');
			$('#raspIP'+raspSlot).text('');
			$('#raspCPU'+raspSlot).hide();
			$('#raspRAM'+raspSlot).text('');
			$('#raspStatus'+raspSlot).hide();
		}
	}

	for(var i=0; i < CONSTANTS.nSlavesByStack+1; ++i)
		raspAction(i, stack.rasps[i]);

}, true);

$(document).ready(function() {
	$('#stackStart')
		.btnColor(true)
		.attr('stackId', STACKID);
	$('#stackShutdown')
		.btnColor(false)
		.attr('stackId', STACKID);
});