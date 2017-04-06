$.fn.btnColor = function(condition){
    this.each(function(){
    	$(this)
    		.removeClass('btn-success btn-danger')
    		.addClass(condition ? 'btn-success' : 'btn-danger');
    });

    return this;
};

addAjaxHandler = function(url, handler, interval, method) {
	if(interval === true)
		interval = 5000;
	if(method === undefined)
		method = 'GET';

	var upd = function() {
		$.ajax({
			url : url,
			dataType : 'json',
			type : method,
			success: handler,
			success : function(data) {
				handler(JSON.parse(data))
			},
			error: function() {
				console.log("AJAX Error.");
			}
		});

		if(interval)
			setTimeout(upd, interval);
	}

	$(document).ready(upd);
}

$(document).ready(function() {
	$('.raspStatusBtn').click(function() {
		var $this = $(this);

		if(confirm("Eteindre le Rasp ?")) {
			addAjaxHandler('/rasp/'+$this.attr('raspId')+'/shutdown', function() {
				console.log("Command Sent : Shutdown Rasp "+$this.attr('raspId'));
			}, false, 'POST');
		}
	});

	$('.raspRestartBtn').click(function() {
		var $this = $(this);

		if(confirm("Redémarrer le Rasp ?")) {
			addAjaxHandler('/rasp/'+$this.attr('raspId')+'/restart', function() {
				console.log("Command Sent : Restart Rasp "+$this.attr('raspId'));
			}, false, 'POST');
		}
	});

	$('.stackStatusBtn').click(function() {
		var $this = $(this);

		if($this.text() == "On") {
			if(confirm("Allumer tous les rasps de la stack ?")) {
				addAjaxHandler('/stack/'+$this.attr('stackId')+'/start', function() {
					console.log("Command Sent : Start every Rasp from Stack "+$this.attr('stackId'));
				}, false, 'POST');
			}
		}
		else if($this.text() == "Off") {
			if(confirm("Eteindre tous les rasps de la stack ?")) {
				addAjaxHandler('/stack/'+$this.attr('stackId')+'/shutdown', function() {
					console.log("Command Sent : Shutdown every Rasp from Stack "+$this.attr('stackId'));
				}, false, 'POST');
			}
		}
	});
});
