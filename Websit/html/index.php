<?php 
	if(isset($_GET['notice1']) && isset($_GET['notice2']) && isset($_GET['notice3'])){
		$cont1 = $_GET['notice1'];
		$cont2 = $_GET['notice2'];
		$cont3 = $_GET['notice3'];
		
		//文件名 "text1.txt", FILE_APPEND, 加
		if($cont1){
                        file_put_contents('/etc/OrangePi2Azure.conf','HostName=');
			file_put_contents('/etc/OrangePi2Azure.conf', $cont1.';DeviceId=', FILE_APPEND);
		}
		if($cont2){
			file_put_contents('/etc/OrangePi2Azure.conf', $cont2.';SharedAccessKey=', FILE_APPEND);
		}
		if($cont3){
			file_put_contents('/etc/OrangePi2Azure.conf', $cont3, FILE_APPEND);
		}
	}
	if(isset($_GET['notice4']) && isset($_GET['notice5']) && isset($_GET['notice6'])){
		$cont4 = $_GET['notice4'];
		$cont5 = $_GET['notice5'];
		$cont6 = $_GET['notice6'];
		
		//文件名 "text4.txt", "text5.txt", "text6.txt",默认覆盖
		if($cont1){
			file_put_contents('/etc/OrangePi/OrangePi_2_CognitiveService_Emotion.conf', $cont4);
		}
		if($cont2){
			file_put_contents('/etc/OrangePi/OrangePi_2_CognitiveService_Face.conf', $cont5);
		}
		if($cont3){
			file_put_contents('text6.txt', $cont6);
		}
	}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Microsoft Cognitive Service</title>
<script type="text/javascript" src="jquery1.42.min.js"></script>
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
}
.left_div_img{float:left; width:49.9%; height:600px;}
.box{width:100%; clear:both;height:500px}
.btn{width:200px; height:100px; margin:0 auto; display:block; font-size:25px;}
.botm_label {
	color:#fff; 
	font-size:25px; 
	text-align:center;
	margin:0 auto; 
	display:block;
}
</style>
<script type="text/javascript" >

function jumpToSetting(){
	window.location.href='setting.php';
}

function jump(){
	window.location.href='capture_home.php';
}

</script>
</head>
<body style="min-width: 1024px; min-height:1265px; background:url(BGP.jpg) repeat">
	<div class="top_div">
		<input style="float:right; width:120px; height:60px;" type="button" value="Setting" onclick="jumpToSetting();" />
	</div>
	<div class="box">
		<div style="height:50px;">
			<label style="width:100%;color:#fff; font-size:50px;text-align:center;margin:0 auto;display:block;">Microsoft Cognitive Service</label>
		</div>
		<div style="clear:both; margin-top:200px;width:100%; height:150px;">
			<input class="btn" type="button" value="Quick Start" onclick="jump();" />
		</div>
	</div>
	<div class="middle_div" ></div>
	<div class="bottom_div">
		<label class="botm_label">Powered by OrangePi</label>
	</div>
</body>
</html>
