$.fn.btnColor = function(condition){
    this.each(function(){
    	$(this)
    		.removeClass('btn-success btn-danger')
    		.addClass(condition ? 'btn-success' : 'btn-danger');
    });
    
    return this;
};

addAjaxHandler = function(url, handler, interval) {
	if(interval === true)
		interval = 20000;

	var upd = function() {
		$.ajax({
			url : url,
			dataType : 'json',
			type : 'GET',
			success : handler
		});

		if(interval)
			setTimeout(upd, interval);
	}

	$(document).ready(upd);
}