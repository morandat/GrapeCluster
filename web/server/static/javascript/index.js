$(document).ready(function(){

	var updatePage = function() {
		$.ajax({
		 	url : "/stack",
		 	dataType : 'json',
		 	type : 'GET',
		 	success : function(data) {
		 		for(var i=0; i<CONSTANTS.maxStacks; ++i)
		 			$('#stack'+i).hide();

		 		var stackCount = 0;

		 		for(var stackId in data) {
		 			var stack = data[stackId];

		 			$('#stackId'+stackCount)
		 				.text(stackId);
		 			$('#stackTemp'+stackCount)
		 				.btnColor(stack.heat<CONSTANTS.stackHeatLimit)
		 				.text(stack.heat+'°C');

		 			var raspAction = function(raspSlot, raspData) {
		 				var position = stackCount+'_'+raspSlot;

		 				if(raspData !== undefined) {
		 					$('#raspName'+position).html('<a href="view/rasp/'+raspData.address+'" class="card-link">'+raspData.name+' ('+raspData.address+')</a>');
								$('#raspStatus'+position)
									.btnColor(raspData.status)
									.text(CONSTANTS.raspStatus[raspData.status])
									.show();

		 					//console.log(position+' : '+JSON.stringify(raspData))
		 				}
		 				else {
		 					$('#raspName'+position).html('---');
		 					$('#raspStatus'+position).hide();

		 					//console.log(position+' : Empty')
		 				}
		 			}

		 			raspAction('master', stack.rasps.master);

		 			for(var i=0; i<CONSTANTS.nSlavesByStack; ++i)
		 				raspAction(i, stack.rasps[i]);

		 			$('#stack'+stackCount).show();

		 			++stackCount;
		 		}
		 	}
		});

		setTimeout(updatePage, 20000);
	}

	updatePage();
});