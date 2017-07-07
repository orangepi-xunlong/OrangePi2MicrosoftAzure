<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Page2</title>
<script type="text/javascript" src="jquery1.42.min.js"></script>
<script language="javascript" type="text/javascript">
    function home(){
		window.location.href='index.html';
    }
	function recapture(){
		window.location.href="mjpg-stream.php";
	}
</script>
<style>
.top_div{
	width:100%;
	height:180px;
}
.middle_div{
	width:100%; 
	height:540px; 
	clear:both
}
.bottom_div{
	width:100%;
	height:45px;
	clear:both;
	vertical-align: bottom ; 
}
.left_div_img{float:left; width:49.9%; height:500px;}
.right_div_img{float:right; width:49.9%; height:500px;}
.img{float:left; width:100%; height:90%;}
.text_label{width:500px; margin:0 auto;display:block; text-align:center}
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
<body style="min-width: 1024px;min-height:1265px; background:url(BGP.jpg) repeat">
	<div class="top_div">
	</div>
	<div style="clear:both">
		<div>
			<div class="left_div_img">
				<img class="img" src="/?action=snapshot"  title="image title 1"/>
				<br />
				<label class="text_label">Capture Raw Picture</label>
				<br />
				<input class="btn" type="button" value="Recapture" onclick="recapture();" />
			</div>
			<div class="right_div_img">
				<img src="After.jpeg" class="img" title="image title 2" alt='picture is handled by cognitive service,pls wait .....(press  "F5")' />
				<br />
				<label class="text_label">Cognitive Service Picture</label>
				<br />
				<input class="btn" type="button" value="Home" onclick="home();" />
			</div>
		</div>
	</div>
	<div class="middle_div" ></div>
	<div class="bottom_div">
		<label class="botm_label">Powered by OrangePi</label>
	</div>
</body>
</html>
