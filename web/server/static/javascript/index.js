//Before manipulating document, it's better to be sure that it is fully charged
$(document).ready(function(){

    console.log("Doing the request ...");
    $.ajax({url:"http://127.0.0.1:5000/test/welcome", success : function(html, status){
            $("#welcome").html(html);
    }});

});