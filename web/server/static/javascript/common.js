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
		interval = 20000;
  if(method === undefined)
    method = 'GET'

	var upd = function() {
		$.ajax({
			url : url,
			dataType : 'json',
			type : method,
			success : handler,
      error: function() {
        alert("AJAX Error.");
      }
		});

		if(interval)
			setTimeout(upd, interval);
	}

	$(document).ready(upd);
}

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
