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

$('.statusBtn').click(function() {
	var $this = $(this);

	if($this.text() == CONSTANTS.raspStatus[0]) {
		if(confirm("Allumer le Rasp ?")) {
			addAjaxHandler('/rasp/start/'+$(this).attr('raspId'), function() {
				$this.text(CONSTANTS.raspStatus[1]).btnColor(true);
			}, false, 'POST');
		}
	}
	if($this.text() == CONSTANTS.raspStatus[1]) {
		if(confirm("Eteindre le Rasp ?")) {
			addAjaxHandler('/rasp/start/'+$(this).attr('raspId'), function() {
				$this.text(CONSTANTS.raspStatus[0]).btnColor(false);
			}, false, 'POST');
		}
	}
});
