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
    method = 'GET'

	var upd = function() {
		$.ajax({
			url : url,
			dataType : 'json',
			type : method,
			success : handler,
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

		if($this.text() == CONSTANTS.status[0]) {
			if(confirm("Allumer le Rasp ?")) {
				addAjaxHandler('/rasp/start/'+$this.attr('raspId'), function() {
					$this.text(CONSTANTS.status[1]).btnColor(true);
					$('.raspRestartBtn[raspId="'+$this.attr('raspId')+'"]').show();
				}, false, 'POST');
			}
		}
		if($this.text() == CONSTANTS.status[1]) {
			if(confirm("Eteindre le Rasp ?")) {
				addAjaxHandler('/rasp/stop/'+$this.attr('raspId'), function() {
					$this.text(CONSTANTS.status[0]).btnColor(false);
					$('.raspRestartBtn[raspId="'+$this.attr('raspId')+'"]').hide();
				}, false, 'POST');
			}
		}
	});

	$('.raspRestartBtn').click(function() {
		var $this = $(this);

		if(confirm("Redémarrer le Rasp ?")) {
			addAjaxHandler('/rasp/restart/'+$this.attr('raspId'), function() {
				$('.raspStatusBtn[raspId='+$this.attr('raspId')+']').text(CONSTANTS.status[0]).btnColor(false);
				$this.hide();
			}, false, 'POST');
		}
	});

	$('.stackStatusBtn').click(function() {
		var $this = $(this);

		if($this.text() == CONSTANTS.status[1]) {
			if(confirm("Eteindre la stack ?")) {
				addAjaxHandler('/stack/shutdown/'+$this.attr('stackId'), function() {
					$this.text(CONSTANTS.status[0]).btnColor(false);
				}, false, 'POST');
			}
		}
	});
});
