PGM_P(ajaxscript) =
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script>"
  "<script>"
  "function gt(n){$.ajax({url:'tt',success:function(result){{$('#tt').html(result);}}});}"              // reloj
  "function te(n){$.ajax({url:'te'+n,success:function(result){{$('#te'+n).html(result);}}});}"          // temperaturas
  "function gl(n){$.ajax({url:'l'+n,success:function(result){{$('#l'+n).html(result);}}});}"            // entradas y salidas locales
  "function gi(n){$.ajax({url:'gi'+n,success:function(result){{$('#gi'+n).html(result);}}});}"          // entradas gpios variables
  "function go(n){$.ajax({url:'go'+n,success:function(result){{$('#go'+n).html(result);}}});}"          // salidas gpios variables
  "function ge(n){$.ajax({url:'ge'+n,success:function(result){{$('#ge'+n).html(result);}}});}"          // temperaturas gpios variable
  "function gr(n){$.ajax({url:'r'+n,success:function(result){{$('#r'+n).html(result);}}});}"            // entradas y salidas remotas
  "function ch5(){for(i=0;i<1;i++){gt(i);}for(i=0;i<8;i++){te(i);}for(i=0;i<10;i++){ge(i);}setTimeout(function(){ch5();},10000);}"   // reloj, temperaturas cada 5 seg.
  "function ch(){for(i=0;i<12;i++){gl(i);}for(i=0;i<10;i++){gi(i);}for(i=0;i<10;i++){go(i);}for(i=0;i<0;i++){gr(i);}setTimeout(function(){ch();},1000);}"    // ent. y sal. cada 1 seg.
  "ch5();ch();"
  "</script>";

