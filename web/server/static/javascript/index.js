addAjaxHandler("/stack", function(stacks) {

	for(var i=0; i<CONSTANTS.maxStacks; ++i)
		$('#stack'+i).hide();

	var stackCount = 0;

	for(var stackId in stacks) {

		var stack = stacks[stackId];

		$('#stackId'+stackCount)
			.attr('href', '/view/stack/'+stackId)
			.text('Stack #'+stackId);
		$('#stackStart'+stackCount)
			.btnColor(true)
			.attr('stackId', stackId);
		$('#stackShutdown'+stackCount)
			.btnColor(false)
			.attr('stackId', stackId);
		$('#stackTemp'+stackCount)
			.btnColor(stack.heat<CONSTANTS.stackHeatLimit)
			.text(stack.heat+'°C');

		var raspAction = function(raspSlot, raspData) {
			var position = stackCount+'_'+raspSlot;

			if(raspData !== undefined) {
				$('#raspName'+position).html('<a href="/view/rasp/'+raspData.address+'" class="card-link">'+raspData.name+' ('+raspData.address+')</a>');
				$('#raspStatus'+position)
					.btnColor(false)
					.attr('raspId', raspData.address)
					.show();
				$('#raspRestart'+position)
					.attr('raspId', raspData.address)
					.show();
			}
			else {
				$('#raspName'+position).html('---');
				$('#raspStatus'+position).hide();
				$('#raspRestart'+position).hide();
			}
		}

		for(var i=0; i<CONSTANTS.nSlavesByStack+1; ++i)
			raspAction(i, stack.rasps[i]);

		$('#stack'+stackCount).show();

		++stackCount;
	}

}, true);
