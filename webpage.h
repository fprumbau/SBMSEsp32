

const char part1[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<title>ElectroDacus</title>
<meta charset="utf-8"/>
<meta http-equiv='cache-control' content='no-cache'>
<meta http-equiv='expires' content='0'>
<meta http-equiv='pragma' content='no-cache'>
<!--meta http-equiv='refresh' content='3'-->
<style media='screen' type='text/css'>
meter {-moz-appearance:none;appearance:none;width:180px;height:12px;position:absolute;left:10px;box-shadow:1px 2px 3px #ec6;}
.bar{position:absolute;bottom:0px;display:inline-block;}
mx1{border:1px solid #fff;color:#fff;padding:0px 3px}
mn1{border:1px solid #fe9;color:#fe9;padding:0px 3px}
Val{color:#fed;}
txt{color:#a99;}
V5{color:#000;white-space:pre;}
v0{background:#fc4;}
v1{background:#e6a;}
v3{background:#0f0;}
v4{background:#f00;}
v6{background:#01f;}
v7{background:#e9f;}
lt{background: #fa6;}
.ch0{background:#f00;}
.date{font-weight:bold;color:#fff;}
.dbg{font-size:0.7em;margin-top:2px;}
#console{box-shadow: inset 0px 1px 4px #666;overflow-y:scroll;font-size:0.7em}
div{position: relative;float:left;}
div2{position: absolute;color:#fed;line-height: 21px;}
div3{position: relative;height:180px; width:720px;float:left;font-family:Arial,serif;font-weight: bold;font-size:18px;}
div1h{position:absolute;width: 179px;height: 160px;left:360px;color:#300;background: rgba(120,90,0,0.6);}
div1m{position:absolute;width: 180px;height: 160px;left:540px;color:#300;background: rgba(30,70,0,0.5);}
div4{position:absolute;width: 236px;height: 22px;bottom:9px;color:#211;background: #fa5;}
div5{position:absolute;background: rgba(120,90,0,0.4);}
button{color:#b50;background:#D8BFD8;border:2px solid white;width:120px;height:25px;}
</style>
</head>
<body style='background: #000;'>
<div3 style='height: 135px;'>
<canvas id='Lg' width='70' height='120' style='position:relative; top:11px; left:12px; z-index:2; float: left;'></canvas>
<div2 style='top:12px; left:360px; color:#d92;text-shadow:-1px -2px 1px #fd4,1px 2px 2px #fea;font-size:40px;' id='id'></div2>
<div2 style='width:350px; top:82px; left:90px; color:#be5;float:none;'><div >www.ElectroDacus.com</div>
<div style='color:transparent; -webkit-transform: rotateX(180deg);transform: rotateX(180deg);-ms-transform:rotateX(180deg); text-shadow: 0px 0px 1px #371;' onClick="document.location.href='/update'">www.ElectroDacus.com</div></div2>
<div2 id="demo"></div2>
<div2 style='width:350px; top:82px; left:520px;'>
<button id="bb" onclick="toggleBattery(this.innerHTML);">Netzvorrang</button>
<br><input type='checkbox' id='dbg1' onclick='toggleDebug(1);'>&nbsp;Dbg1</input>
<input type='checkbox' id='dbg2' onclick='toggleDebug(2);'>&nbsp;Dbg2</input>
</div2>
</div3>
<div3>
<meter id='bat' style='height: 70px; width: 320px; top: 9px;' min='0' low='20' max='100'></meter>
<meter style='height: 50px; left: 332px; top:18px; width: 15px;' min='2' max='8' value='0'></meter>
<div2 style='color:#030;font-size:48px;top:35px;left:120px;text-shadow: -2px -2px 2px #efc;' id='SOC' ></div2>
<div2 style='left:360px;color:#ea8;' id='d2'></div2>
<div2 style='left:430px;' id='d3'></div2>
<div2 style='left:490px;color:#888;' id='d4'></div2>
<div2 style='left:5px;top:92px;color:#a99;line-height: 25px;' id='d12'></div2>
<div2 style='left:510px;' id='mt1'></div2> 
</div3>
<div3>
<div2 style='left:5px;color:#ea8;top:22px;'id='d6'></div2>
<div2 style='right:530px;text-align: right;'id='d7'></div2>
<div2 style='right:430px;text-align: right;'id='d8'></div2>
<div2 style='right:220px;text-align: right;'id='d9'></div2>
<div2 style='right:10px;text-align: right;'id='d10'></div2>
<div5 style='width: 720px;top:0px;height:22px;text-align: right;'id='d11'></div5>

</div3>

<br>
<div id="console" style="width:707px;height:150px;background-color:#505050;border:1px solid #606060;color:beige;padding:5px;">
</div>
<script id='smain2'>

var debug = false;

function log(msg) {
  cs = document.getElementById('console');
  cs.innerHTML = cs.innerHTML + '<br><b class=date>' + new Date().toLocaleString() + ':</b>&nbsp;&nbsp;' + msg;
  cs.scrollTop = cs.scrollHeight;
}

var xsbms="01d&u%u$#'G";
var eA="##lh###v1---$v2---empty-%v1&2-#+#y#$1u#y##";
var eW="##T_###$1---#%2---empty-#&1&2-#$#x#$#&#z##";
var s1=['110Ah','nnnW','SBMS120 '];
var s2=[0,0,0,0,0,0,0,0,3,7,1,1];

localStorage['cap']=s1[0];
localStorage['WA']=s1[1];
localStorage['model']=s1[2];

)=====";

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const char part2[] PROGMEM = R"=====(

//Reconnecting-websocket
!function(a,b){"function"==typeof define&&define.amd?define([],b):"undefined"!=typeof module&&module.exports?module.exports=b():a.ReconnectingWebSocket=b()}(this,function(){function a(b,c,d){function l(a,b){var c=document.createEvent("CustomEvent");
return c.initCustomEvent(a,!1,!1,b),c}var e={debug:!1,automaticOpen:!0,reconnectInterval:1e3,maxReconnectInterval:3e4,reconnectDecay:1.5,timeoutInterval:2e3};d||(d={});for(var f in e)this[f]="undefined"!=typeof d[f]?d[f]:e[f];
this.url=b,this.reconnectAttempts=0,this.readyState=WebSocket.CONNECTING,this.protocol=null;var h,g=this,i=!1,j=!1,k=document.createElement("div");k.addEventListener("open",function(a){g.onopen(a)}),k.addEventListener("close",function(a){g.onclose(a)}),
k.addEventListener("connecting",function(a){g.onconnecting(a)}),k.addEventListener("message",function(a){g.onmessage(a)}),k.addEventListener("error",function(a){g.onerror(a)}),this.addEventListener=k.addEventListener.bind(k),this.removeEventListener=k.removeEventListener.bind(k),
this.dispatchEvent=k.dispatchEvent.bind(k),this.open=function(b){h=new WebSocket(g.url,c||[]),b||k.dispatchEvent(l("connecting")),(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","attempt-connect",g.url);var d=h,
e=setTimeout(function(){(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","connection-timeout",g.url),j=!0,d.close(),j=!1},g.timeoutInterval);h.onopen=function(){clearTimeout(e),(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","onopen",g.url),
g.protocol=h.protocol,g.readyState=WebSocket.OPEN,g.reconnectAttempts=0;var d=l("open");d.isReconnect=b,b=!1,k.dispatchEvent(d)},h.onclose=function(c){if(clearTimeout(e),h=null,i)g.readyState=WebSocket.CLOSED,k.dispatchEvent(l("close"));
else{g.readyState=WebSocket.CONNECTING;var d=l("connecting");d.code=c.code,d.reason=c.reason,d.wasClean=c.wasClean,k.dispatchEvent(d),b||j||((g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","onclose",g.url),k.dispatchEvent(l("close")));
var e=g.reconnectInterval*Math.pow(g.reconnectDecay,g.reconnectAttempts);setTimeout(function(){g.reconnectAttempts++,g.open(!0)},e>g.maxReconnectInterval?g.maxReconnectInterval:e)}},h.onmessage=function(b){(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","onmessage",g.url,b.data);
var c=l("message");c.data=b.data,k.dispatchEvent(c)},h.onerror=function(b){(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","onerror",g.url,b),k.dispatchEvent(l("error"))}},1==this.automaticOpen&&this.open(!1),this.send=function(b){if(h)
return(g.debug||a.debugAll)&&console.debug("ReconnectingWebSocket","send",g.url,b),h.send(b);throw"INVALID_STATE_ERR : Pausing to reconnect websocket"},this.close=function(a,b){"undefined"==typeof a&&(a=1e3),i=!0,h&&h.close(a,b)},this.refresh=function(){h&&h.close()}}
return a.prototype.onopen=function(){},a.prototype.onclose=function(){},a.prototype.onconnecting=function(){},a.prototype.onmessage=function(){},
a.prototype.onerror=function(){},a.debugAll=!1,a.CONNECTING=WebSocket.CONNECTING,a.OPEN=WebSocket.OPEN,a.CLOSING=WebSocket.CLOSING,a.CLOSED=WebSocket.CLOSED,a});

console.log('Trying to open Webclient socket');
log('Trying to open Webclient socket hihi');
)=====";

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const char part3[] PROGMEM = R"=====(

//var timerID=0;
connection.onopen = function () { 
  //if(window.timerID){
  //  window.clearInterval(window.timerID);
  //  window.timerID=0;
  //}  
  connection.send('Connect ' + new Date()); 
};
connection.onerror = function (error) { 
  console.log('wsServer Error ', error);
  log('wsServer Error ' + error);
};

connection.onclose = function(){
   //if(!window.timerID){
   //   window.timerID = setInterval(function(){connect()}, 30000);
   //}
};

//vom Server empfangen
var server = '';
var data = '';
connection.onmessage = function (e) { 
  if(server == '') {
    server = e.data;
    console.log('Server: ', server);
    log('Server: ' + server);
  } else {
    data = e.data;
    if(data[0] != '@') {
      if(debug) {
        console.log('Daten: ', data);
        log('Daten: ' + data);
      }
      sbms=data
      localStorage['sbmsb']=sbms;
      all();
    } else {
      console.log('Daten: ', data);
      log(data);      
      bb = document.getElementById('bb')
      if(data.indexOf('LOW')!==-1) {
        bb.style.background='#D8BFD8';
        bb.style.color='#b50';
        bb.innerHTML='Netzvorrang';
      } else if(data.indexOf('HIGH')!==-1) {
        bb.style.background='#f00';
        bb.style.color='#ff0';
        bb.innerHTML='Batterie&nbsp;&nbsp;&nbsp;&nbsp;';
      } else if(data.indexOf('debug to 1')!==-1) {
        document.getElementById("dbg1").checked=true
      } else if(data.indexOf('debug to 0')!==-1) {
        document.getElementById("dbg1").checked=false
      } else if(data.indexOf('debug2 to 1')!==-1) {
        document.getElementById("dbg2").checked=true
      } else if(data.indexOf('debug2 to 0')!==-1) {
        document.getElementById("dbg2").checked=false
      }
    }
  }
};
console.log('End trying to open Webclient socket');
log('End trying to open webclient socket');

//Keine State-Information hier, die Bestätigung kommt mit Websocket-Datagramm
function toggleBattery(txt) {
    if(txt == 'Netzvorrang') {
      connection.send("@+"); //auf Batterie aendern
    } else {
      connection.send("@-"); //auf Netzvorrang schalten
    }
}
function toggleDebug(nr) {
    debug=document.getElementById("dbg"+nr).checked;
    connection.send("@d"+nr+"-"+debug);
}

function all(){
  
  var sbms=localStorage['sbmsb'];
  var sbms2=JSON.parse(JSON.stringify(s2));
  
  var sbms1=['','Batt','PV1','PV2','ExtLd','PV1+PV2','Load','ExtLd',s1[0],s1[1],s1[2]];
  var lg1="#B33A33B##333333B##B33A33B##B33333B##B''''''##A44A544##B44444B##;75444A##144B444##B33333B##444444B##2331$$A##B8:B:8B#";
  
  var c = document.getElementById('Lg');
  var ctx = c.getContext('2d');
  var r ='</br>'
  
  function htm(id,s){document.getElementById(id).innerHTML =s;};
  function pad(n, width, z) {z=z || '0';n=n+'';return n.length >= width ? n : new Array(width - n.length + 1).join(z) + n;}
  function dcmp(p,s,d){xx=0; for (z=0;z<s;z++){xx = xx + ((d.charCodeAt((p+s-1)-z)-35)*Math.pow(91,z));}return xx;}
  function fN(nm){return nm.toString().replace(/(\d)(?=(\d{3})+(?!\d))/g, '$1 , ')}
  htm('id',sbms1[10]);
  var SOC='';
  SOC =dcmp(6,2,sbms);
  htm('SOC','<b>'+SOC+'%</b>');
  document.getElementById('bat').value=SOC;

  if (sbms2[10]!=1){mos(0);};if (sbms2[11]!=1){mos(1);};
  function mos(rr){document.getElementById('mo'+rr).style.background='rgba(120,90,0,0.7)';}

  //Logo animieren
  for (i=0;i<15;i++){setInterval(function(){lg(lg1);},500*i);};
  function lg(d){
    var k = new Date();
    var n = k.getSeconds();
    if (n>=40){n=n-40};
    if (n>=20){n=n-20};
    if (n>12){n=12};
    ctx.clearRect(0, 0, 70, 120);
    for (y=0;y<9;y++){
    for (x=0;x<7;x++){
    var pix2=((pad(((d.charCodeAt(y+(12*9))-35).toString(2)),6)).charCodeAt(x))-48;
    var pix=((pad(((d.charCodeAt(y+(n*9))-35).toString(2)),6)).charCodeAt(x))-48;
    if (pix==1){col1='#be9';col2='rgba(142,204,104,0.'}
    else if (pix2==1){col1='#694';col2='rgba(66,104,44,0.'}
    else {col1='#361';col2='rgba(42,84,36,0.'}
    ctx.fillStyle =col1;
    ctx.fillRect(x*10, y*10, 8, 8);
    if(y>=6){ctx.fillStyle =col2+(((y*2)-2)-8)+')';ctx.fillRect(x*10, (17.3-y)*10, 8, 8);}
  }}}
  
  mt8('#mt1');
  function mt8(m1) {
    var w = new Array();
    for (i=0;i<20;i++){w[i]='';}
    var bv=pv3=sv=max1=min1=0;
    for (x1=0;x1<8;x1++) {
      var n=n1='';   
      var cv=dcmp((x1*2)+8,2,sbms)/1000;
      if (sbms2[9]==x1+1){min1=cv;n='<mn1>';n1='</mn1>';};
      if (sbms2[8]==x1+1){max1=cv;n='<mx1>';n1='</mx1>';};
      w[0] +=n+'Cell. '+ (x1+1)+n1+r ;
      w[1] +=n+cv.toFixed(3)+n1+r;
      if (sbms2[x1]!=1){w[2] +='<txt>V</txt>'+r;}
      else {w[2] +='<lt><</lt>'+r;};
      bv +=cv;
      var mt = document.querySelector(m1);
      var x = document.createElement('meter');
      //Frank: Divisor von 1000 auf 100 geaendert
      x.setAttribute('min',dcmp(5,2,xsbms)/100);
      x.setAttribute('max',dcmp(3,2,xsbms)/100);
      x.setAttribute('value',cv);
      x.style.top=((x1*21)+3)+'px'
      mt.appendChild(x);
    } 
    for (i=2;i<5;i++){htm('d'+i,w[i-2]);}
    for (x1=0;x1<7;x1++) {
      col = sbms1[x1+1];
      if(col == 'Load') continue; //Load nicht mehr benoetigt
      var n2=w[8]=w[9]=w[10]=w[11]='';
      var cv=dcmp((x1*3)+29,3,sbms)/1000;
      var enW=dcmp(x1*6,6,eW);
      var enA=dcmp(x1*6,6,eA);
      if (x1==0){
        n2=sbms.charAt(28);
        w[8]='[A]'+r;
        w[9]='[W]'+r;
        w[10]='[MAh][kAh][Ah][mAh]'+r;
        w[11]='[MWh][kWh][Wh]'+r;
      };
      if (x1==1||x1==2){pv3 +=cv;};
      if (x1==3){sv=cv;}
      if (x1==4){cv=pv3;}
      if (x1==5){cv=dcmp(0,3,xsbms)/1000;}
      if (x1==6){cv=sv;}  
      if(x1!=3){
      w[3] +=col+r ;
      w[4] +=w[8]+n2+cv.toFixed(3)+r;
      w[5] +=w[9]+n2+(cv*bv).toFixed(1)+r;
      w[6] +=w[10]+fN(enA)+r;
      w[7] +=w[11]+fN((enW/10).toFixed(1))+r;
      }
        
    }   
    for (i=6;i<9;i++){ //Spalten MAh,kAh,Ah,mAh | MWh, kWh, Wh weg ( i<11 vorher)
      htm('d'+i,w[i-3]);
    }
    htm('d'+12,'Typ: LiIon Kap: 110Ah Status: '+dcmp(56,3,sbms)+r+'SBMS Temp Int: <val>'+ ((dcmp(24,2,sbms)/10)-45).toFixed(1)+'</val>&#8451 Ext: <val>'+ ((dcmp(26,2,sbms)/10)-45).toFixed(1)+'</val>&#8451'+r+'BattVoltage <Val>'+ bv.toFixed(3)+'</Val> V Cell &#916 <Val>'+((max1-min1)*1000).toFixed(0)+'</Val> mV');
  }

} //end all

//anfangs sollten die Checkboxen nicht selektiert sein 
document.getElementById("dbg1").checked=false;
document.getElementById("dbg2").checked=false;
</script>

</body>
</html>
)=====";
