<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Page1</title>
<script type="text/javascript" src="jquery1.42.min.js" ></script>
<script language="javascript" type="text/javascript">

	function jump2(){
	   document.getElementById('formid').submit();
	}
	function jump1(){
	   window.location.href='index.php';
	}
</script>
<style>
.top_div{
	width:100%;
	height:180px;
}

.middle_div{
	width:100%; 
	height:695px; 
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
.box{width:100%; padding-top:30px; clear:both;}
.btn{width:100px;height:50px; margin:0 auto; display:block;}
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
				<div style="width:300px; height:100%; margin:0 auto; display:block;">
				<form id="formid" action="index.php" method="GET">
					<div style="clear:both; margin-top:10px;">
						<label>HostName:</label><input name="notice1" type="text" placeholder="The host name of AzureIOT" width="80px;" />
					</div>
					<div style="clear:both; margin-top:10px;">
						<label>DeviceID:</label><input name="notice2" type="text" placeholder="The name of AzureIOT device" width="80px;" />
					</div>
					<div style="clear:both; margin-top:10px;">
						<label>SharedAccessKey:</label><input name="notice3" type="text" placeholder="The string of AzureIOT access key" width="80px;" />
					</div>
					<div style="clear:both; margin-top:10px;">
						<label>Emotion Preview:</label><input name="notice4" type="text" placeholder="Recognize Emotion in Image" width="80px;" />
					</div>
					<div style="clear:both; margin-top:10px;">
						<label>Face Preview:</label><input name="notice5" type="text" placeholder="Computer Vision API" width="80px;" />
					</div>
					<div style="clear:both; margin-top:10px;">
						<label>Reserved:</label><input name="notice6" type="text" placeholder="Reserved" width="80px;" />
					</div>
				 </form>
				</div>
			</div>
		</div>
		<div class="box">
			<div style="width:48%; float:left; display:block;"> 
				<input class="btn" style="float:right" type="button" value="OK" onclick="jump2();" />
			</div>
			<div style="width:48%; float:left; display:block;" > 
				<input class="btn" style="float:left; margin-left:10px" type="button" value="Cancel" onclick="jump1();" />
			</div>
		</div>
	</div>
	<div class="middle_div" ></div>
	<div class="bottom_div">
		<label class="botm_label">Powered by OrangePi</label>
	</div>
</body>
</html>
