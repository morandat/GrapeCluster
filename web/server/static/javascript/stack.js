addAjaxHandler("/stack/"+STACKID, function(stack) {

	$('#stackTemp')
		.btnColor(stack.heat<CONSTANTS.stackHeatLimit)
		.text(stack.heat+'°C');
	$('#stackShutdown')
		.btnColor(stack.status)
		.text(CONSTANTS.status[stack.status])
		.attr('stackId', STACKID)
		.css('cursor', stack.status?'pointer':'default');

	var raspAction = function(raspSlot, raspData) {
		if(raspData !== undefined) {
			$('#raspName'+raspSlot).html('<a href="/view/rasp/'+raspData.address+'" class="card-link">'+raspData.name+' ('+raspData.address+')</a>');
			$('#raspI2C'+raspSlot).text(raspData.address);
			$('#raspOS'+raspSlot).text(raspData.os);
			$('#raspIP'+raspSlot).text(raspData.ip);
			$('#raspCPU'+raspSlot)
				.btnColor(raspData.cpu<CONSTANTS.raspCPULimit)
				.text(Math.round(raspData.cpu)+'%')
				.show();
			$('#raspRAM'+raspSlot).text(raspData.ram);
			$('#raspStatus'+raspSlot)
				.btnColor(raspData.status)
				.text(CONSTANTS.status[raspData.status])
				.attr('raspId', raspData.address)
				.show();
			$('#raspRestart'+raspSlot).attr('raspId', raspData.address);
			if(raspData.status)
				$('#raspRestart'+raspSlot).show();
			else
				$('#raspRestart'+raspSlot).hide();

			//console.log(raspSlot+' : '+JSON.stringify(raspData))
		}
		else {
			$('#raspName'+raspSlot).html('---');
			$('#raspI2C'+raspSlot).text('');
			$('#raspOS'+raspSlot).text('');
			$('#raspIP'+raspSlot).text('');
			$('#raspCPU'+raspSlot).hide();
			$('#raspRAM'+raspSlot).text('');
			$('#raspStatus'+raspSlot).hide();

			//console.log(raspSlot+' : Empty')
		}
	}

	for(var i=0; i < CONSTANTS.nSlavesByStack+1; ++i)
		raspAction(i, stack.rasps[i]);

}, true);
