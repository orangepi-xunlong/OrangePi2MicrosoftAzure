<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Page1</title>
<script type="text/javascript" src="jquery1.42.min.js" ></script>
<script language="javascript" type="text/javascript">

	function jump(){
            $.ajax({
                 url:"../cgi-bin/do_one_capture.cgi",
                 type:"post",
                 success:function(data){
                 }
	   });
	   window.location.href='speach_result.php';
	   //setTimeout("window.location.href='capture.php'", "2000");
	}
</script>
<style>
.top_div{
	width:100%;
	height:180px;
}

.middle_div{
	width:100%; 
	height:705px; 
	clear:both
}

.bottom_div{
	width:100%;
	height:45px;
	clear:both;
}
.left_div_img{width:100%; height:300px;}
.right_div_img{float:right; width:49.9%; height:400px;}
.label{width:500px; height:100%;margin:0 auto;display:block; font-size:20px; font-weight:bold; text-align:center}
.box{width:100%; padding-top:10px; clear:both;}
.btn{width:100px; margin:0 auto; display:block;}
.botm_label {
	color:#fff; 
	font-size:25px; 
	text-align:center;
	margin:0 auto; 
	display:block;
}
</style>
</head>
<body style="min-width: 1024px;min-height:1265px; background:url(BGP.jpg) repeat" ">
	<div class="top_div">
	</div>
	<div style="clear:both">
		<div>
			<div class="left_div_img">
				<label class="label" >Please speach</label>
			</div>
		</div>
		<div class="box">
			<div style="width:100%; float:left; display:block;"> 
				<input class="btn" type="button" value="Start Speach" onclick="jump();" />
			</div>
		</div>
	</div>
	<div class="middle_div" ></div>
	<div class="bottom_div">
		<label class="botm_label">Powered by OrangePi</label>
	</div>
</body>
</html>
