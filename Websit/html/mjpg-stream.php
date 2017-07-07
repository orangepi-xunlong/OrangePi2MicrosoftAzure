<?php 
	$host = gethostbyname($_SERVER['SERVER_NAME']);
	$cont1 = $_GET['notice1'];
	$cont2 = $_GET['notice2'];
	$cont3 = $_GET['notice3'];
	
	//文件名 "text1.txt", "text2.txt", "text3.txt",
    file_put_contents('text1.txt', $cont1);
	file_put_contents('text2.txt', $cont2);
	file_put_contents('text3.txt', $cont3);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Page1</title>
<script type="text/javascript" src="jquery1.42.min.js" ></script>
<script language="javascript" type="text/javascript">
	function jump1(){
         $.ajax({
                 url:"../cgi-bin/update.cgi",
                 type:"post",
                 success:function(data){//ajax返回的数据
                 }
		});
		window.location.href='capture.php';
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
.img{width:650px; height:100%;margin:0 auto; display:block;}
.btnl{width:100px;height:50px; display:block; clear:both; margin:0 auto; display:block; }
.btnr{width:100px; height:50px;display:block; float:right}
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
		<div style="float:left;width:100%">
			<div class="left_div_img">
				<img name="imgtag"  src="<?php echo 'http://'.$host.':8080/?action=stream' ?>" class="img" />
			</div>
			<div style="clear:both; padding-top:10px;">
				<input class="btnl" type="button" value="Capture" onclick="jump1();" />
			</div>
		</div>
	</div>
	<div class="middle_div" ></div>
	<div class="bottom_div">
		<label class="botm_label">Powered by OrangePi</label>
	</div>
</body>
</html>
