$(function () {
  $("form").submit(function (e) {

    /*$("#haiku").addClass("hidden");
    setTimeout(function () {
      $("#confirmation").addClass("shown");
    }, 800);*/
   e.preventDefault();
//will need to uncomment this section to get it working
   $.get('/text', $(this).serialize(), function(d){
     $("#haiku").removeClass("hidden");
     /*$("#confirmation").addClass("shown");*/
   });
   $(this).find('input').val('');
  });
  
  /******  This is to show how many characters have been typed  ******/
  var keyHandler = function (e) {
    var len = this.value.length;
    var left = 120 - len;
    if (left < 10) left = "&nbsp;&nbsp;"+left;
    // pad to keep the width the same, total hack
    
    $(this).nextAll(".num").html(left);
    
    
    var anyEmpty = $("input").is(function () {
      return this.value.length === 0;
    })
    
    if (!anyEmpty) {
      $("button").addClass("complete");
    } else {
      $("button").removeClass("complete");
    }
  };
  $("input").keydown(keyHandler);
  $("input").keypress(keyHandler);
  $("input").keyup(keyHandler);
  
  
});


