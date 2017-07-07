<?php 

$host = gethostbyname($_SERVER['SERVER_NAME']);

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Page1</title>
<script type="text/javascript" src="jquery1.42.min.js" ></script>
<script language="javascript" type="text/javascript">
	function upload(){
         $.ajax({
                 url:"../cgi-bin/update.cgi",
                 type:"post",
                 success:function(data){//ajax返回的数据
                 }
		});
		//setTimeout("window.location.href='upload.php'", "4000");
		window.location.href='upload.php';
	}
	
	function recapture(){
                $.ajax({
                 url:"../cgi-bin/do_one_capture.cgi",
                 type:"post",
                 success:function(data){//ajax返回的数据
                 }
                });
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
	height:505px; 
	clear:both
}
.bottom_div{
	width:100%;
	height:40px;
	clear:both;
}
.left_div_img{width:100%; height:500px;}
.right_div_img{float:right; width:49.9%; height:500px;}
.img{width:650px; height:100%;margin:0 auto;display:block;}
.box{width:100%; padding-top:10px; clear:both;}
.btnl{width:100px; height:50px; display:block; float:left}
.btnr{width:100px;display:block; float:right}
.botm_label {
	color:#fff; 
	font-size:25px; 
	text-align:center;
	margin:0 auto; 
	display:block;
}
</style>
</head>
<body style="min-width: 1024px;min-height:1265px; background:url(BGP.jpg) repeat" >
	<div class="top_div">
	</div>
	<div style="clear:both">
		<div>
			<div class="left_div_img">
				<img name="imgtag"  src="<?php echo 'http://'.$host.':8080/?action=snapshot'?>" class="img" />
			</div>
		</div>
		<div class="box">
		  <div style="width:250px; margin:0 auto;display:block;">
			<input class="btnl" type="button" value="Upload" onclick="upload();" />
			<input class="btnl" style="margin-left:20px;" type="button" value="Recapture" onclick="recapture();" />
		  </div>
		</div>
	</div>
	<div class="middle_div" ></div>
	<div class="bottom_div">
		<label class="botm_label">Powered by OrangePi</label>
	</div>
</body>
</html>
