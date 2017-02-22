$.fn.btnColor = function(condition){
    this.each(function(){
    	$(this)
    		.removeClass('btn-success btn-danger')
    		.addClass(condition ? 'btn-success' : 'btn-danger');
    });
    
    return this;
};