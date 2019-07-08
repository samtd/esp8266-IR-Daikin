const char PAGE_NetworkConfiguration[] PROGMEM = R"=====(
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n

<!DOCTYPE html>
<html>
<meta http-equiv='Cache-control' content='no-cache'>
<style>

/* For Firefox */
input[type='number'] {-moz-appearance:textfield;}

/* Webkit browsers like Safari and Chrome */
input[type=number]::-webkit-inner-spin-button,
input[type=number]::-webkit-outer-spin-button {-webkit-appearance: none; margin: 0;}
#trzebra tr:nth-child(even) {background-color: #f2f2f2;}

.buttonwarning {background-color: orange; padding: 10px 15px; font-weight: bold;} 
.button1 {padding: 5px 10px; font-weight: bold;} 
.button2 {padding: 3px 5px; font-weight: bold;}


.center { margin: auto; width: auto; padding: 50px; font-size: 20px;}
.current { text-align: right; background-color: powderblue; padding: 5px;}
.red { color: red;}
.green {color: green;}
#myProgress { width: 70%; background-color: #ddd;}
#myBar { width: 1%; height: 20px; background-color: #4CAF50;}
</style>
<body>
<div style='margin-left: 15px'>

<table style='width:80%'>
<td style='font-size: 25px; border-radius: 15px; color:white; background-color:red; text-align: center; font-weight: bold'>ESP8266</td>
<td style='text-align: center;font-size: 20px;'> <b style='padding-bottom: 10px;'> NODEMCU ESP8266 HVAC - Samuel David </b> </td>
<td style='width:20%; font-weight: bold;' id='progrevision'> Revision: 00.0 00\00\2018 </td>
<td style='border: 2px solid grey; width:20%; text-align: center;'>
<button type='button' class='buttonwarning' onclick='sendData("reset")'>REBOOT</button>
<button type='button' class='buttonwarning' onclick='sendData("resetwifi")'>RESET WIFI</button>
</td>
</tr>
</table>
 
<table style='width:80%'>
<tr>
<td style='border: 2px solid grey; padding-right: 20px; padding-left: 5px; width:35%'> 
   <table id='trzebra' style='width: 100%;'>
      <tr>
        <td style='width: 45%;'>SSID</td>
        <td>:</td>
        <td style='font-weight: bold; text-align: center;' id='wifissid'></td>
       </tr>
       <tr>
        <td>Hostname</td>
        <td>:</td>
        <td style='font-weight: bold; text-align: center;' id='wifihostname'></td>
       </tr>
       <tr>
        <td>MQTT Client</td>
        <td>:</td>
        <td style='font-weight: bold; text-align: center;' id='topicclient'></td>
       </tr>
       <tr>
        <td>Mac Adress</td>
        <td>:</td>
        <td style='font-weight: bold; text-align: center;' id='wifimac'></td>
       </tr>
       <tr>  
        <td>IP Address</td>
        <td>:</td>
        <td style='font-weight: bold; text-align: center;' id='stringip'></td>
       </tr>
       <tr>
        <td>signal strength (RSSI)</td>
        <td>:</td>
        <td style='font-weight: bold; text-align: center;' id='wifirssi'> </td>
       </tr>
       <tr>
        <td>Wifi Status</td>
        <td>:</td>
        <td style='font-weight: bold; text-align: center;' id='wifisatus'>--</td>
       </tr>
       <tr>
        <td>MQTT Status</td>
        <td>:</td>
        <td style='font-weight: bold; text-align: center;' id='clientcon'></td>
       </tr>   
       <tr>
       <td>Uptime:</td>
       <td>:</td>
       <td style='font-weight: bold; text-align: center;' id='uptime'>--</td>
       </tr>
        <td>MQTT Online:</td>
       <td>:</td>
       <td style='font-weight: bold; text-align: center;' id='uptimeMQTT'>--</td>
       </tr>         
       <tr>      
        <td>Nivel Iluminacao:</td>
        <td>:</td>
        <td style='font-weight: bold; text-align: center;'>
             <table>
             <tr>
             <td id='myProgress'><div id='myBar'></div></td>
             <td><span style='font-weight: bold;' id='ilumina'>--</span>LUX</td>
             </tr>    
             </table>    
       </td>
       </tr> 
       <tr>
       <td>Sensor Movimento:</td>
       <td>:</td>
       <td style='font-weight: bold; text-align: center;' id='pirstatus'>--</td>
       </tr>
   </table>
</td>
<td style='width:1%'></td>

<td style='border: 2px solid red; padding-right: 10px; padding-left: 5px; width:65%'>
    <form method='POST' action='/GET' enctype='multipart/form-data' onsubmit='return SendText()' >
    
    <table>
    <tr>
    <td colspan='4' style='border: 2px solid frey; font-weight: bold;  color: red; padding: 5px; text-align: center;'> CONFIGURACAO</td>
    </tr>
    <tr>
    <td style='width: 10%; font-weight: bold;'> CLIENT </td>
    <td>:</td>
    <td colspan='2' style='text-align: center;'> 
        Hostname: <input type='text'  name='hostname' id='hostname'> 
        MQTT Client: <input type='text' name='MQTTclient' id='MQTTclient'>
    </td>
    </tr>
    <tr>
    <td style='font-weight: bold;'> SERVER IP </td>
    <td>:</td>
    <td colspan='2' style='text-align: center;'> 
        <input style='width: 3em;' type='number' name='Cip_01' id='Cip_01' min='0' max='255' maxlength='3'>.
        <input style='width: 3em;' type='number' name='Cip_02' id='Cip_02' min='0' max='255' maxlength='3'>.
        <input style='width: 3em;' type='number' name='Cip_03' id='Cip_03' min='0' max='255' maxlength='3'>.
        <input style='width: 3em;' type='number' name='Cip_04' id='Cip_04' min='0' max='255' maxlength='3'>
    </td>
    </tr>
    <tr><td colspan='4' style='border: 1px solid grey; background-color: grey;'></td></tr>
    <tr>
    <td colspan='2' style='width: 10%;'> </td>
    <td style='width: 45%; font-weight: bold; text-align: center;'> Temp:Hum Calibration (0.000) </td>
    <td style='width: 45%; font-weight: bold; text-align: center;'> Lux Calibration (0.000) </td>
    </tr>
    <tr>
    <td colspan='2' style='width: 10%;'> </td>
    <td style='width: 50%; text-align: center;'>  
    Temp&nbsp;Cal:&nbsp;<input type='text' name='Temp_factor' id='Temp_factor' maxlength='5' size='5'>&nbsp;&nbsp;&nbsp;
    Humi&nbsp;Cal:&nbsp;<input type='text' name='Humi_factor' id='Humi_factor' maxlength='5' size='5'>   
    </td>
    <td style='width: 40%; text-align: center;'>
    Lux&nbsp;Cal:&nbsp;<input type='text' name='Lux_factor' id='Lux_factor' maxlength='5' size='5'>
    </td>    
    </tr> 
 
    <tr><td colspan='4' style='border: 1px solid grey; background-color: grey;'></td></tr>
    </table>
    
    Password: arduino<br>
    Password:&nbsp;<input type='password' name='Pverifica' id='Pverifica' maxlength='7' size='7'>
      <input type='submit' value='Validar'>&nbsp;Status:&nbsp;
    <b id='Pmessage' ></b>
    <b>&nbsp; - &nbsp; </b>
    <a id='filestatus'><b>&nbsp;&nbsp;&nbsp;
    <b id='Pmessage1' ></b> 
    </form> 
  
</td>
</tr>
</table>

<table style='width:80%'>
<tr><td colspan='3' style='width: 100%; border: 2px solid grey;'>

        <table style='width: 100%;'>
                <tr>
                <td>
                
                    <table class='table table-condensed' style='width: 100%;'>
                        <tr>
                            <th style='text-align: center'>Descricao</th>
                            <th style='text-align: center'>Estado</th>
                            <th style='text-align: center'>Alteracao</th>
                            <th style='text-align: center'>Descricao</th>
                            <th style='text-align: center'>Estado</th>
                            <th style='text-align: center'>Alteracao</th>
                        </tr> 
                        <tr>
                            <td class='text-left'>Temperatura:</td>
                            <td class='current' name='temperatura' id='temperatura' >--&nbsp;C</td>
                            <td class='text-left'></td>
                            <td class='text-left'>Humidade:</td>
                            <td class='current' name='humidade' id='humidade'>--&nbsp;%</td>
                            <td class='text-left'></td>
                        </tr>
                        <tr>
                            <td class='text-left'>AC Setpoint:</td>
                            <td class='current' id='temp'>--</td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bsetempm")'>-</button>
                                <button type='button' class='btn btn-default' onclick='sendData("bsetempp")'>+</button></td>
                            <td class='text-left'>AC Estado:</td>
                            <td class='current' id='power' ></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bpoweron")'>ON</button>
                                <button type='button' class='btn btn-default' onclick='sendData("bpoweroff")'>OFF</button></td>
                        </tr>
                        <tr>
                            <td class='text-left'>AC Mode:</td>
                            <td class='current' id='mode' ></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bmode")'>Change</button></td>
                            <td class='text-left'>AC Fan:</td>
                            <td class='current' id='fan'></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bfan")'>Change</button></td> 
                        </tr>
                        <tr>
                            <td class='text-left'>AC Powerful:</td>
                            <td class='current' id='powerful'></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bpowerfulon")'>ON</button>
                                <button type='button' class='btn btn-default' onclick='sendData("bpowerfuloff")'>OFF</button></td>
                            <td class='text-left'>AC Quiet:</td>
                            <td class='current' id='quiet'></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bquieton")'>ON</button>
                                <button type='button' class='btn btn-default' onclick='sendData("bquietoff")'>OFF</button></td>
                        </tr>
                        <tr>
                            <td class='text-left'>AC Sensor:</td>
                            <td class='current' id='sensor'></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bsensoron")'>ON</button>
                                <button type='button' class='btn btn-default' onclick='sendData("bsensoroff")'>OFF</button></td>
                            <td class='text-left'>AC Eye:</td>
                            <td class='current' id='eye'></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("beyeon")'>ON</button>
                                <button type='button' class='btn btn-default' onclick='sendData("beyeoff")'>OFF</button></td>
                        </tr>
                        <tr>
                            <td class='text-left'>AC Mold:</td>
                            <td class='current' id='mold'></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bmoldon")'>ON</button>
                                <button type='button' class='btn btn-default' onclick='sendData("bmoldoff")'>OFF</button></td>
                            <td class='text-left'>AC Econo:</td>
                            <td class='current' id='econo'></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("beconoon")'>ON</button>
                                <button type='button' class='btn btn-default' onclick='sendData("beconooff")'>OFF</button></td>
                        </tr>
                        <tr>
                            <td class='text-left'>SwingVertical:</td>
                            <td class='current' id='swingvertical'></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bswingverticalon")'>ON</button>
                                <button type='button' class='btn btn-default' onclick='sendData("bswingverticaloff")'>OFF</button></td>
                            <td class='text-left'>SwingHorizontal:</td>
                            <td class='current' id='swinghorizontal'></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bswinghorizontalon")'>ON</button>
                                <button type='button' class='btn btn-default' onclick='sendData("bswinghorizontaloff")'>OFF</button></td>
                        </tr>
                        <tr>
                            <td class='text-left'>OnTimer Estado:</td>
                            <td class='current' id='ontimers'></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bontimerson")'>ON</button>
                                <button type='button' class='btn btn-default' onclick='sendData("bontimersoff")'>OFF</button></td>
                            <td class='text-left'>OnTimer Time:</td>
                            <td class='current' id='ontimert'></td>
                            <td></td>
                        </tr>
                        <tr>
                            <td class='text-left'>OffTimer Estado:</td>
                            <td class='current' id='offtimers'></td>
                            <td><button type='button' class='btn btn-default' onclick='sendData("bofftimerson")'>ON</button>
                                <button type='button' class='btn btn-default' onclick='sendData("bofftimersoff")'>OFF</button></td>
                            <td class='text-left'>OffTimer Time:</td>
                            <td class='current' id='offtimert'></td>
                            <td></td>
                        </tr>
                        <tr>
                            <td class='text-left'>CurrentTime:</td>
                            <td colspan='2' class='text-left' id='currentime'></td>
                            <td colspan='2'></td>
                        </tr>
                 
                    </table>
                
                </td>
                
                </tr>
                </table>


</td></tr>

</table>


<table  style='width: 100%;'>
    <tr>
      <th colspan='3' style='text-align: center; border: 2px solid grey;'>Last IR Receiver&nbsp;( <span id='irrecvcounter'> </span>)</th>
    </tr>
    <tr>
      <td colspan='3' style='text-align: left; border: 2px solid grey;' id='htmlirreceive'></td>
    </tr>
</table>

<table  style='width: 100%;'>
    <tr>
    <th colspan='3' style='text-align: center; border: 2px solid grey;'>Last IR Sender&nbsp;(<span id='irsendcounter'> </span>)</th>
    </tr>
    <tr>
    <td colspan='3' style='text-align: left; border: 2px solid grey;' id='htmlirsend'></td>
    </tr>
</table>


</div>
</body>

<script>

var iluminax;
var start; 

function SendText() {   
    var Confpassinput = document.getElementById('Pverifica').value;
    var Confpass = 'arduino';
    var n = Confpassinput.localeCompare(Confpass);
    var cal01 = Number(document.getElementById('Temp_factor').value);
    var cal02 = Number(document.getElementById('Humi_factor').value);
    var cal03 = Number(document.getElementById('Lux_factor').value);
    
    if(isNaN(cal01)) {n = 1}
    if(isNaN(cal02)) {n = 1}
    if(isNaN(cal03)) {n = 1}

    if ( n == 0) {    
                    document.getElementById('Pmessage').innerHTML='OK';
                     // Countdown timer
                     var counter = 10;
                     var interval = setInterval(function()
                     {
                        document.getElementById('Pmessage1').innerHTML = -- counter;
                        if (counter == 0)
                        {
                            clearInterval(interval);
                            location.reload(true);  
                        }
                      }, 1000);
                  }
    else {
            document.getElementById('Pmessage').innerHTML='ERRO';
            return false;
           }
  }


setInterval(function() { 
  getData();
  iluminafuc();
  }, 1000); 


  

function getData() {
  var xmlHttp=createXmlHttpObject();
  //nocache = "&nocache" + Math.random() * 10000;

 function createXmlHttpObject(){
    if(window.XMLHttpRequest){
      xmlHttp=new XMLHttpRequest();
    }else{
      xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
    }
    return xmlHttp;
  }
  
  xmlHttp.onreadystatechange = function() {
    if (xmlHttp.readyState == 4 && xmlHttp.status == 200) {
      var xmlDoc=xmlHttp.responseXML;

        document.getElementById('progrevision').innerHTML =
           xmlDoc.getElementsByTagName('progrevision')[0].childNodes[0].nodeValue;
        document.getElementById('wifissid').innerHTML =
           xmlDoc.getElementsByTagName('wifissid')[0].childNodes[0].nodeValue;
        document.getElementById('wifihostname').innerHTML =
           xmlDoc.getElementsByTagName('wifihostname')[0].childNodes[0].nodeValue;
        document.getElementById('topicclient').innerHTML =
           xmlDoc.getElementsByTagName('topicclient')[0].childNodes[0].nodeValue;
        document.getElementById('wifimac').innerHTML =
           xmlDoc.getElementsByTagName('wifimac')[0].childNodes[0].nodeValue;
        document.getElementById('stringip').innerHTML =
           xmlDoc.getElementsByTagName('stringip')[0].childNodes[0].nodeValue;
        document.getElementById('clientcon').innerHTML =
           xmlDoc.getElementsByTagName('clientcon')[0].childNodes[0].nodeValue;
        if(xmlDoc.getElementsByTagName('clientcon')[0].childNodes[0].nodeValue == 'ONLINE')
        {document.getElementById('clientcon').style.color = "green";}
        else
        { document.getElementById('clientcon').style.color = "red"; }

        document.getElementById('hostname').defaultValue =
           xmlDoc.getElementsByTagName('hostname')[0].childNodes[0].nodeValue;
        document.getElementById('MQTTclient').defaultValue =
           xmlDoc.getElementsByTagName('MQTTclient')[0].childNodes[0].nodeValue;                 
        document.getElementById('wifirssi').innerHTML =
           xmlDoc.getElementsByTagName('wifirssi')[0].childNodes[0].nodeValue + " dBm";
        //HTML DOM Style Object
        var rssicolor = xmlDoc.getElementsByTagName('wifirssi')[0].childNodes[0].nodeValue
        if(rssicolor <= '-60')
        {document.getElementById('wifirssi').style.color = "green";}
        else if ( rssicolor > '-60' &&  rssicolor <= '-80')
        { document.getElementById('wifirssi').style.color = "orange"; }
        else
        {document.getElementById('wifirssi').style.color = "red";}
        
        document.getElementById('wifisatus').innerHTML =
           xmlDoc.getElementsByTagName('wifisatus')[0].childNodes[0].nodeValue;
        if(xmlDoc.getElementsByTagName('wifisatus')[0].childNodes[0].nodeValue == 'CONNECTED')
        {document.getElementById('wifisatus').style.color = "green";}
        
        document.getElementById('uptime').innerHTML =
           xmlDoc.getElementsByTagName('uptime')[0].childNodes[0].nodeValue;
        document.getElementById('uptimeMQTT').innerHTML =
           xmlDoc.getElementsByTagName('uptimeMQTT')[0].childNodes[0].nodeValue;


       
       
        document.getElementById('Cip_01').defaultValue =
           xmlDoc.getElementsByTagName('Cip_01')[0].childNodes[0].nodeValue;
        document.getElementById('Cip_02').defaultValue =
           xmlDoc.getElementsByTagName('Cip_02')[0].childNodes[0].nodeValue;
        document.getElementById('Cip_03').defaultValue =
           xmlDoc.getElementsByTagName('Cip_03')[0].childNodes[0].nodeValue;
        document.getElementById('Cip_04').defaultValue =
           xmlDoc.getElementsByTagName('Cip_04')[0].childNodes[0].nodeValue;
        document.getElementById('Temp_factor').defaultValue =
           xmlDoc.getElementsByTagName('Temp_factor')[0].childNodes[0].nodeValue;
        document.getElementById('Humi_factor').defaultValue =
           xmlDoc.getElementsByTagName('Humi_factor')[0].childNodes[0].nodeValue;
        document.getElementById('Lux_factor').defaultValue =
           xmlDoc.getElementsByTagName('Lux_factor')[0].childNodes[0].nodeValue;    

          document.getElementById('temperatura').innerHTML =
           xmlDoc.getElementsByTagName('temperatura')[0].childNodes[0].nodeValue;
          document.getElementById('humidade').innerHTML =
           xmlDoc.getElementsByTagName('humidade')[0].childNodes[0].nodeValue;

          document.getElementById('temp').innerHTML =
           xmlDoc.getElementsByTagName('temp')[0].childNodes[0].nodeValue;
          document.getElementById('power').innerHTML =
           xmlDoc.getElementsByTagName('power')[0].childNodes[0].nodeValue;
          document.getElementById('mode').innerHTML =
           xmlDoc.getElementsByTagName('mode')[0].childNodes[0].nodeValue;
          document.getElementById('fan').innerHTML =
           xmlDoc.getElementsByTagName('fan')[0].childNodes[0].nodeValue;
          document.getElementById('powerful').innerHTML =
           xmlDoc.getElementsByTagName('powerful')[0].childNodes[0].nodeValue;
          document.getElementById('quiet').innerHTML =
           xmlDoc.getElementsByTagName('quiet')[0].childNodes[0].nodeValue;
          document.getElementById('sensor').innerHTML =
           xmlDoc.getElementsByTagName('sensor')[0].childNodes[0].nodeValue;
          document.getElementById('eye').innerHTML =
           xmlDoc.getElementsByTagName('eye')[0].childNodes[0].nodeValue;
          document.getElementById('mold').innerHTML =
           xmlDoc.getElementsByTagName('mold')[0].childNodes[0].nodeValue;
          document.getElementById('econo').innerHTML =
           xmlDoc.getElementsByTagName('econo')[0].childNodes[0].nodeValue;
          document.getElementById('swingvertical').innerHTML =
           xmlDoc.getElementsByTagName('swingvertical')[0].childNodes[0].nodeValue;
          document.getElementById('swinghorizontal').innerHTML =
           xmlDoc.getElementsByTagName('swinghorizontal')[0].childNodes[0].nodeValue;
          document.getElementById('ontimers').innerHTML =
           xmlDoc.getElementsByTagName('ontimers')[0].childNodes[0].nodeValue;
          document.getElementById('ontimert').innerHTML =
           xmlDoc.getElementsByTagName('ontimert')[0].childNodes[0].nodeValue;
          document.getElementById('offtimers').innerHTML =
           xmlDoc.getElementsByTagName('offtimers')[0].childNodes[0].nodeValue;
          document.getElementById('offtimert').innerHTML =
           xmlDoc.getElementsByTagName('offtimert')[0].childNodes[0].nodeValue;
          document.getElementById('currentime').innerHTML =
           xmlDoc.getElementsByTagName('currentime')[0].childNodes[0].nodeValue;
          document.getElementById('filestatus').innerHTML =
           xmlDoc.getElementsByTagName('filestatus')[0].childNodes[0].nodeValue;
          document.getElementById('ilumina').innerHTML =
           xmlDoc.getElementsByTagName('ilumina')[0].childNodes[0].nodeValue;
          document.getElementById('pirstatus').innerHTML =
           xmlDoc.getElementsByTagName('pirstatus')[0].childNodes[0].nodeValue;

           var xx = xmlDoc.getElementsByTagName('ilumina1')[0];
           var yy = xx.childNodes[0];
           iluminax = yy.nodeValue;
          //iluminax = xmlDoc.getElementsByTagName('ilumina1')[0].childNodes[0].nodeValue;


          
          document.getElementById('htmlirsend').innerHTML =
           xmlDoc.getElementsByTagName('htmlirsend')[0].childNodes[0].nodeValue;
          document.getElementById('htmlirreceive').innerHTML =
           xmlDoc.getElementsByTagName('htmlirreceive')[0].childNodes[0].nodeValue;

          document.getElementById('irrecvcounter').innerHTML =
           xmlDoc.getElementsByTagName('irrecvcounter')[0].childNodes[0].nodeValue;
          document.getElementById('irsendcounter').innerHTML =
           xmlDoc.getElementsByTagName('irsendcounter')[0].childNodes[0].nodeValue;
           
    }
  };
  //xmlHttp.open("GET", "xml" + nocache, true);
  xmlHttp.open("GET", "xml", true);
  xmlHttp.send();
}

      function sendData(bot) {
        var xhttp = new XMLHttpRequest();
        xhttp.open('GET', 'setBOT?BOTstate='+bot, true);
        xhttp.send();
        
       }


    function iluminafuc() {
  var elem = document.getElementById('myBar');   
  var f = parseInt(iluminax);
  var width = (f*100)/1000;
  if (width > 100)
  width = 100;
   elem.style.width = width + '%';
  // document.getElementById('demo').innerHTML = f
    }

  
  
</script>
</html>








)=====";
