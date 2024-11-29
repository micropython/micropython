#### COPYRIGHT AMAS TECHNOLOGIES ####
## LICENSED UNDER AGPL-3.0 ##
#### COPYRIGHT AMAS TECHNOLOGIES ####

C7="Couldn't connect to the WiFi!"
C6='/shadow/name/'
C5='$aws/things/'
C4='Registered under: '
C3='old_ip'
C2='dev_id'
C1='OPTIONS, GET'
C0='/update'
B_='generate'
Bz='DEVICE REBOOTING IN 10 SECONDS!'
By='def_key.der'
Bx='def_cert.der'
Bw='Creating new salt'
Bv='sensors'
Bu='run_finder'
Bt='alerts.json'
Bs='dhcp_hostname'
Br=RuntimeError
Ba='text/plain'
BZ='Access-Control-Allow-Origin'
BY='base64_DER_key'
BX='base64_DER_cert'
BW='ambient_limit'
BV='cust_id_token'
BU='del_cert'
BT='wifi'
BS='name'
BR='anonymous_data_collection'
BQ='\x00'
B5='Starting services'
B4=None
B3='wb'
B2='firmware_version'
B1='https://devicecert.amastech.cloud/'
B0='no_telemetry'
A_='amas'
Az='username'
Ay='device_id'
Ax='api_key'
Aw=dict
Av=bytearray
Ag='key.der'
Af='cert.der'
Ae='POST'
Ad='OPTIONS, POST'
Ac='desired'
Ab='null_desire'
Aa='serial'
AZ='friendly_name'
AY='drain_check'
AX='powered'
AW='light_override'
AV='off'
AU='on'
AO='close cmd'
AN='note'
AM='stale_cert'
AL='cert_id'
AK='no_auto_update'
AJ='auto_update'
AC='application/json'
AB='customer.json'
AA='override'
A9=bool
A8=len
A2='Unauthorized'
A1='Allow'
A0=type
x='admin'
w='cust_id'
v='runtime'
t=Exception
p='Content-Type'
o='interval'
n='salt'
m='debug'
i='OPTIONS'
c='x-api-key'
U='\n'
T=range
Q='reported'
P='r'
O='w'
N='light'
M=False
K='state'
J='pump'
I=str
H=True
F=print
D=int
B=open
import network as R,gc as L,os as W
from httpclient import HttpClient as B6
from machine import WDT
from ubinascii import hexlify
from time import sleep,localtime as B7,time as q,gmtime
from sht3x import SHT3x_Sensor as C8
from ota_updater import OTAUpdater as C9
from ujson import loads as d,dumps as e
from ds3231_port import DS3231
import machine as B8,sys,esp32 as B9,uasyncio as A
from ntptime import settime as BA
from microdot import Microdot as CA,Response as r
from microdot.websocket import with_websocket as Bb
from microdot.cors import CORS
from mqtt_as import MQTTClient as Bc,config as s
import uhashlib as CB,random,string as Bd
from httpclient import HttpClient as B6
import ssl
from binascii import a2b_base64 as AD
Y='v0.22.3'
if Y.split('.')[0]=='v0':E=H
else:
	try:
		with B(m):E=H
	except:E=M
Ah=B9.NVS('nvs')
C=M
Ai=M
try:
	with B(Ax,P)as CC:G=CC.readline().strip()
except:
	try:G=Av(32);Ah.get_blob(b'api_key',G);G=G.decode();L.collect()
	except:sys.exit()
try:
	with B('platform')as CD:AE=CD.read().strip()
	C=H
except:
	try:AE=Av(25);Ah.get_blob(b'platform',AE);AE=AE.decode().split(BQ)[0]
	except:AE='ESP32_GENERIC'
try:
	with B(Ay)as CE:g=CE.read()
	C=H
except:
	try:g=Av(64);Ah.get_blob(b'device_id',g);g=g.decode().split(BQ)[0]
	except:g='nodeviceidset_'+I(q());Ai=H
try:
	with B(Az)as AP:y=A8(AP.readlines())
	with B(Az)as AP:
		if y==1:a=u=AP.read().strip()
		elif y==2:a=AP.readline().strip();u=AP.readline().strip()
		else:raise t
	C=H
except:
	try:a=Av(25);Ah.get_blob(b'serial',a);a=a.decode().split(BQ)[0];u=M
	except:a='noserialset_'+I(q());Ai=H
if E or C:F('Loaded device identifiers!')
h=WDT(timeout=60000)
def Be(ssid,password):
	global a,C
	if C:
		try:
			global u
			if u:R.hostname(A_+u.lower())
			else:raise t
		except:R.hostname(A_+a.lower())
	else:R.hostname(A_+a.lower())
	A=R.WLAN(R.STA_IF)
	if R.WLAN(R.AP_IF).active():R.WLAN(R.AP_IF).active(M)
	A.active(H)
	if E or C:F('Connecting to: '+ssid)
	A.connect(ssid,password)
	for B in T(15):
		sleep(1)
		if A.isconnected():break
		elif B==14:A.active(M);raise OSError
def BB():
	global G,a;A=R.WLAN(R.AP_IF)
	if R.WLAN(R.STA_IF).active():R.WLAN(R.STA_IF).active(M)
	A.active(H);A.config(essid=A_+a.lower(),authmode=R.AUTH_WPA_WPA2_PSK,password=G)
def CZ(file_object,chunk_size=1024):
	while H:
		A=file_object.read(chunk_size)
		if not A:break
		yield A
def BC(n=16):
	A=''
	for B in T(0,n):A=A+random.choice(Bd.ascii_lowercase+Bd.digits)
	return A
def CF(old,new):
	F=old;C=new;D={}
	for A in C:
		if A0(C[A])is Aw:
			for B in C[A]:
				if A0(C[A][B])is Aw:
					for E in C[A][B]:
						if C[A][B][E]!=F[A][B][E]:
							try:D[A][B][E]=C[A][B][E]
							except:
								try:D[A][B]={};D[A][B][E]=C[A][B][E]
								except:D[A]={};D[A][B]={};D[A][B][E]=C[A][B][E]
				elif C[A][B]!=F[A][B]:
					try:D[A][B]=C[A][B]
					except:D[A]={};D[A][B]=C[A][B]
		elif C[A]!=F[A]:D[A]=C[A]
	return D
def Bf():
	if gmtime(0)[0]==2000:A=q()+946684800
	else:A=q()
	return A
async def Bg():
	global Bh,Bi
	try:C=C8(freq=100000,sclpin=Bh,sdapin=Bi)
	except:return-9999,-9999
	try:B=C.read_temp_humd()
	except:
		A.sleep(1)
		try:B=C.read_temp_humd()
		except:
			A.sleep(1)
			try:B=C.read_temp_humd()
			except:return-9999,-9999
	if D(B[0])==-45 and D(B[1])==0:return-9999,-9999
	return B[0],B[1]
async def Bj():
	global BD,Bk;BD.value(1);A.sleep(.1);B=Bk.read();B=B/4500*100;BD.value(0)
	if B<15:B=-9999
	return B
async def S():global BE;BE.value(1);A.sleep(.05);BE.value(0)
async def Aj():global V;V.value(1);A.sleep(.1);V.value(0)
async def AF(unit_state={},query_state={}):
	a=query_state;G=unit_state;global k,b;c=M
	with B(N,P)as f:Q=f.readline().strip();R=f.readline().strip()
	L.collect()
	with B(J,P)as K:
		try:V=A9(D(K.readline().strip()));X=D(K.readline().strip());Y=D(K.readline().strip())
		except:V=A9(K.readline());X=D(K.readline().strip());Y=D(K.readline().strip())
	L.collect()
	if G:
		if E or C:F(G)
		if N in G:
			try:
				if D(G[N][AU])in T(0,2400):
					if D(G[N][AU][:2])in T(0,24)and D(G[N][AU][2:])in T(0,60):Q=G[N][AU]
			except:pass
			try:
				if D(G[N][AV])in T(0,2400):
					if D(G[N][AV][:2])in T(0,24)and D(G[N][AV][2:])in T(0,60):R=G[N][AV]
			except:pass
			with B(N,O)as f:f.write(Q+U+R+U)
			try:
				if G[N][AA]=='1'or G[N][AA]=='0'or G[N][AA]==M:
					if G[N][AA]!=M:
						b.value(D(G[N][AA]))
						with B(AW,O)as n:n.write(G[N][AA])
					else:
						try:W.remove(AW)
						except:pass
			except:
				try:Z=z.get_time(set_rtc=M)
				except:Z=B7()
				g=I(Z[3])+(I(Z[4])if A8(I(Z[4]))==2 else'0'+I(Z[4]))
				if R==Q:b.value(0)
				elif D(Q)<D(R):
					if D(g)in T(D(Q),D(R)):b.value(1)
					else:b.value(0)
				elif D(Q)>D(R):
					if D(g)in T(D(Q),2400)or D(g)in T(0,D(R)):b.value(1)
					else:b.value(0)
				del g,Z;L.collect()
		if J in G:
			try:
				if G[J][AX]==M or G[J][AX]==H:
					with B(J,O)as K:K.write(I(D(G[J][AX]))+U+I(X)+U+I(Y)+U)
					V=G[J][AX]
				if not V:k.value(0)
				L.collect()
			except:pass
			try:
				if A0(G[J][o])is D and G[J][o]in T(600,86401):
					with B(J,O)as K:K.write(I(D(V))+U+I(G[J][o])+U+I(Y)+U)
					X=G[J][o]
				if A0(G[J][o])is not D and D(G[J][o])in T(600,86401):K.write(I(D(V))+U+I(D(G[J][o]))+U+I(Y)+U);X=D(G[J][o])
				L.collect()
			except:pass
			try:
				if A0(G[J][v])is D and G[J][v]in T(60,86401):
					with B(J,O)as K:K.write(I(D(V))+U+I(X)+U+I(G[J][v])+U)
					Y=G[J][v]
				if A0(G[J][v])is not D and D(G[J][v])in T(60,86401):K.write(I(D(V))+U+I(X)+U+I(D(G[J][v])+U));Y=D(G[J][v])
				L.collect()
			except:pass
			try:
				if G[J]['drain']:
					with B(AY,O)as p:p.write('1')
					k.value(1)
				else:
					try:W.remove(AY)
					except:pass
					k.value(0)
			except:pass
		if AJ in G:
			q=G[AJ]
			if q:
				try:W.remove(AK)
				except:pass
			else:
				with B(AK)as r:r.write('1')
		if BR in G:
			s=G[BR]
			if s:
				try:W.remove(B0)
				except:pass
			else:
				with B(B0)as t:t.write('1')
		if AZ in G:
			with B(BS,O)as u:u.write(I(G[AZ]))
	if a:
		if E or C:F(a)
		try:
			if a['restart']:
				for h in T(3):await S()
				A.create_task(A5());c=H
		except:pass
		try:
			if a['network-reset']:
				for h in T(3):await S()
				try:W.remove(BT)
				except:pass
				A.create_task(A5());c=H
		except:pass
		try:
			if a['factory-reset']:
				try:
					with B(AB,P)as x:l=d(x.read())
					i=l[AL];j=l[w]
				except:pass
				for h in T(3):await S()
				for h in W.listdir('/'):
					try:W.remove(h)
					except:pass
				m=B6()
				try:
					try:y=m.post(B1,json={BU:{w:j,AL:i}})
					except:await A.sleep(1);L.collect();y=m.post(B1,json={BU:{w:j,AL:i}})
				except:
					with B(AM,O)as A1:A1.write(e({w:j,AL:i}))
				A.create_task(A5());c=H
		except:pass
	if c:return H
	return M
async def CG(setup_dic):
	C=setup_dic;global A3,A4,Ai;D=M
	if AZ in C:
		with B(BS,O)as F:F.write(I(C[AZ]))
	if A3:
		try:
			with B(BT,O)as G:G.write(I(C['ssid'])+U+I(C['password']))
			D=H;await S()
		except:pass
	if A4:
		try:
			with B(BV,O)as J:J.write(C['id_token'])
			D=H;await S()
		except:pass
	if Ai:
		try:
			with B(Az,O)as K:E=C[Az];L=E+I(q());K.write(E+U+L)
			D=H;await S()
		except:pass
		try:
			with B(Ay,P)as N:N.write(C[Ay])
			D=H;await S()
		except:pass
	if D:A.create_task(A5());return H
	return M
async def AQ():
	f='status';global k,b,g,a,C,G,A4;h={Ax:G};O=R.WLAN(R.STA_IF)
	if O.active():E=O.ifconfig()[0]
	else:E='192.168.4.1'
	with B(J,P)as F:i=A9(D(F.readline().strip()));j=D(F.readline().strip());l=D(F.readline().strip())
	m=A9(k.value())
	with B(N,P)as Q:S=Q.readline().strip();T=Q.readline().strip()
	n=A9(b.value())
	try:p=B(AY,P);U=H;p.close()
	except:U=M
	try:
		with B(AW,P)as q:V=q.read()
	except:V=M
	try:
		with B(AK)as A0:X=M
	except:X=H
	try:
		with B(B0)as A1:Z=M
	except:Z=H
	L.collect();A={Ay:g,Aa:a,N:{f:n,AU:S,AV:T,AA:V},J:{AX:i,o:j,v:l,f:m,'drain':U},'internal_ip':E,AJ:X,BR:Z};del E,S,T;L.collect();A.update(h)
	try:
		with B(AB)as r:I=d(r.read())
		s=I[w];K=I['cert_exp'];c=Bf()
		if c>float(K):W.remove(AB);B8.reset()
		elif c>=float(K)-2592000:A4=1;e=H
		else:e=M
		del I;L.collect();A.update({'customer_id':s,'device_certificate':{'expiration:':K,'renewal_period_active':e}})
	except:pass
	try:
		with B(BS)as t:x=t.read()
		A.update({AZ:x})
	except:pass
	if C:
		try:global u;A.update({Bs:u})
		except:pass
	A.update({B2:Y})
	with B(Bt,P)as y:z=d(y.read())
	A.update(z);return A
async def Z(unit_state=Bu):
	A=unit_state;C,D=await Bg();E=await Bj();B={Bv:{'environment':{'temperature':C,'humidity':D},'water_level':E}}
	if A:
		if A==Bu:B.update(await AQ())
		else:B.update(A)
	del C,D,E;L.collect();return B
async def BF(payload):
	A=payload;D={};F=Bf()
	if A:
		A=A[K][Q]
		for B in A:
			if B in[N,J,B2]:
				if A0(A[B])is Aw:
					for C in A[B]:D[B+'_'+C]=A[B][C]
				else:D[B]=A[B]
			if B==Bv:
				for C in A[B]:
					if A0(A[B][C])is Aw:
						for E in A[B][C]:
							if not A[B][C][E]==-9999:D[C+'_'+E]=A[B][C][E]
					elif not A[B][C]==-9999:D[C]=A[B][C]
	D.update({'iso':F});return D
async def A5():
	if E or C:F('Restarting device in 10 seconds!')
	await A.sleep(10);B8.reset()
async def Ak():pass
async def Al():pass
async def Am():
	global k;await A.sleep(600)
	while H:
		try:
			with B(AY,P)as I:k.value(1)
		except:
			with B(J,P)as C:
				try:E=A9(D(C.readline().strip()));F=D(C.readline().strip());G=D(C.readline().strip())
				except:E=A9(C.readline().strip());F=D(C.readline().strip());G=D(C.readline().strip())
			L.collect()
			if E:k.value(1);await A.sleep(G);k.value(0)
			else:k.value(0)
		await A.sleep(F)
async def An():
	global b
	try:global z;C=z.get_time(set_rtc=M)
	except:C=B7()
	while H:
		try:
			with B(AW,P)as K:
				O=D(K.readline().strip())
				if O:b.value(1)
				else:b.value(0)
		except:
			with B(N,P)as J:E=J.readline().strip();F=J.readline().strip()
			L.collect();G=I(C[3])+(I(C[4])if A8(I(C[4]))==2 else'0'+I(C[4]))
			if F==E:b.value(0)
			elif D(E)<D(F):
				if D(G)in T(D(E),D(F)):b.value(1)
				else:b.value(0)
			elif D(E)>D(F):
				if D(G)in T(D(E),2400)or D(G)in T(0,D(F)):b.value(1)
				else:b.value(0)
		await A.sleep(30)
async def Ao():
	S='High';N='Low';K='Sensor Error'
	while H:
		if not k.value():
			P=await Bj()
			if P==-9999:G=K
			elif P<25:G=N
			else:G=M
			with B(BW)as C:T=D(C.readline().strip());U=D(C.readline().strip());V=D(C.readline().strip());W=D(C.readline().strip())
			I,J=await Bg()
			if I>T:E=S
			elif I==-9999:E=K
			elif I<U:E=N
			else:E=M
			if J>V:F=S
			elif J==-9999:F=K
			elif J<W:F=N
			else:F=M
			Q={'alerts':{'water_level_alert':G,'temperature_alert':E,'humidity_alert':F}}
			with B(Bt,O)as R:R.write(e(Q))
			del Q,R;L.collect()
		h.feed();await A.sleep(10)
async def CH():
	global V
	while H:V.value(1);await A.sleep(.5);V.value(0);await A.sleep(1)
async def CI():
	global V
	while H:V.value(1);await A.sleep(.1);V.value(0);await A.sleep(.05);V.value(1);await A.sleep(.1);V.value(0);await A.sleep(.05);V.value(1);await A.sleep(.1);V.value(0);await A.sleep(1)
async def CJ():
	global V
	while H:V.value(1);await A.sleep(.1);V.value(0);await A.sleep(.05);V.value(1);await A.sleep(.25);V.value(0);await A.sleep(.05);V.value(1);await A.sleep(.1);V.value(0);await A.sleep(1)
async def Bl():
	while H:
		try:BA()
		except:
			await A.sleep(120)
			try:BA()
			except:
				await A.sleep(120)
				try:BA()
				except:pass
		try:global z;z.save_time()
		except:pass
		await A.sleep(86400)
async def Ap():
	global A3,BG
	while H:
		B=R.WLAN(R.STA_IF)
		if not B.active():
			try:
				Be(BK,BL)
				if BG:B8.reset()
			except:A3=1;BB()
		del B;L.collect();await A.sleep(600)
async def Bm():
	try:global z
	except:pass
	G=0
	while H:
		try:
			with B(AK)as V:pass
			Q=900
		except:
			try:L=z.get_time(set_rtc=M)
			except:L=B7()
			R=I(L[3])+(I(L[4])if A8(I(L[4]))==2 else'0'+I(L[4]))
			with B(N,P)as S:J=S.readline().strip();K=S.readline().strip()
			Q=900
			if K==J:
				for U in T(0,48):
					await A.sleep(900)
					if not K==J:break
					if U==47:
						G=G+1
						if G==6:await A5()
						F=await f.update(Y,debug=E or C)
						if not F:
							await A.sleep(15);F=await f.update(Y,debug=E or C)
							if not F:
								await A.sleep(15);F=await f.update(Y,debug=E or C)
								if not F:await A.sleep(15);F=await f.update(Y,debug=E or C)
			elif D(K)<D(J):
				O=I(D((D(K)+D(J))/2))[:-2]
				if D(O+'00')<D(R)<D(O+'20'):
					G=G+1
					if G==6:await A5()
					F=await f.update(Y,debug=E or C)
					if not F:
						await A.sleep(15);F=await f.update(Y,debug=E or C)
						if not F:
							await A.sleep(15);F=await f.update(Y,debug=E or C)
							if not F:await A.sleep(15);F=await f.update(Y,debug=E or C)
			elif D(K)>D(J):
				O=I(D((D(K)-D(J))/2))[:-2]
				if D(O+'00')<D(R)<D(O+'20'):
					G=G+1
					if G==6:await A5()
					F=await f.update(Y,debug=E or C)
					if not F:
						await A.sleep(15);F=await f.update(Y,debug=E or C)
						if not F:
							await A.sleep(15);F=await f.update(Y,debug=E or C)
							if not F:await A.sleep(15);F=await f.update(Y,debug=E or C)
		await A.sleep(Q)
async def CK():
	await A.sleep(65)
	if E or C:F('All services started successfully!')
	CM.mark_app_valid_cancel_rollback()
	if E or C:F('Since no errors were found, marked boot partition as valid!')
try:
	with B(J,P)as AR:y=A8(AR.readlines())
	if not y==3:raise t
except:
	with B(J,O)as CL:CL.write('0\n600\n60\n')
try:
	with B(N,P)as AR:y=A8(AR.readlines())
	if not y==2:raise t
except:
	with B(N,O)as BH:BH.write('0000\n0000\n')
try:
	with B(BW,P)as AR:y=A8(AR.readlines())
	if not y==4:raise t
except:
	with B(BW,O)as BH:BH.write('40\n10\n90\n10\n')
try:
	with B(n)as AS:X=AS.read()
	if E or C:F('Found existing salt')
except:
	if E or C:F(Bw)
	X=BC()
	with B(n,O)as AS:AS.write(X)
from vars_ESP32_GENERIC_S3 import amb_scl as Bh,amb_sda as Bi,analog_water_sensor as Bk,water_power as BD,hard_reset_button,network_reset_button,pump as k,light as b,err_light as V,dat_light as BE,i2c_r
try:z=DS3231(i2c_r);z.get_time(set_rtc=H)
except:pass
CM=B9.Partition(B9.Partition.RUNNING)
f=C9(url='https://ota.amastechnologies.com/',watchdog=h,light_activities={'good_light':S,'error_light':Aj},platform=AE)
if E or C:F('Loaded global variables!')
h.feed()
try:W.remove(AY)
except:pass
try:W.remove(AW)
except:pass
try:W.remove(Ab)
except:pass
from default_api_cert import base64_cert_and_key as BI
with B(Bx,B3)as CN:CN.write(AD(BI[BX]))
with B(By,B3)as CO:CO.write(AD(BI[BY]))
del BI
L.collect()
j=CA()
CORS(j,allowed_origins='*',allow_credentials=H,max_age=20,expose_headers=[A1,p,c,BZ,'Access-Control-Allow-Methods','Access-Control-Allow-Headers','Access-Control-Request-Method','Access-Control-Expose-Headers','Access-Control-Allow-Credentials'])
@j.route('/control',methods=[Ae,i])
async def Ca(request):
	B=request;global X;global G;D=B.headers
	if not D or c not in D or D[c]!=G:return A2,401
	A=B.json
	if A:
		try:F=A[K][x]
		except:F={}
		try:A=A[K][Ac]
		except:A={}
	else:F={}
	J=await AF(unit_state=A,query_state=F)
	if J:H={K:{AN:Bz,x:{n:X,m:E or C},Q:await Z()}}
	else:H={K:{x:{n:X,m:E or C},Q:await Z()}}
	I=r(body=H,status_code=200,headers={p:AC})
	if B.method==i:I.headers[A1]=Ad
	await S();return I
@j.route('/setup',methods=[Ae,i])
async def Cb(request):
	A=request;global X;global G;B=A.headers
	if not B or c not in B or B[c]!=G:return A2,401
	D=M;F=A.json
	if F:D=await CG(F)
	if D:H={K:{AN:Bz,x:{n:X,m:E or C},Q:await Z()}}
	else:H={K:{x:{n:X,m:E or C},Q:await Z()}}
	I=r(body=H,status_code=200,headers={p:AC})
	if A.method==i:I.headers[A1]=Ad
	return I
@j.route('/salt',methods=[Ae,i])
async def Cc(request):
	A=request;global X;global G;D=A.headers
	if not D or c not in D or D[c]!=G:return A2,401
	I=A.json
	if I[B_]:
		if E or C:F(Bw)
		X=BC()
		with B(n,O)as J:J.write(X)
	else:raise Br
	L={K:{AN:'New salt generated!'}};H=r(body=L,status_code=200,headers={p:AC})
	if A.method==i:H.headers[A1]=Ad
	return H
@j.route('/api_key',methods=[Ae,i])
async def Cd(request):
	C=request;global G;D=C.headers
	if not D or c not in D or D[c]!=G:return A2,401
	H=C.json
	if H[B_]:
		E=BC(32)
		with B(Ax,O)as I:I.write(E)
		A={Ax:E};A={K:{'new':A}}
	else:A={K:{AN:'New API key not generated!'}}
	F=r(body=A,status_code=200,headers={p:AC})
	try:G=E
	except:pass
	if C.method==i:F.headers[A1]=Ad
	return F
@j.route(C0)
@Bb
async def Ce(request,ws):
	Z='Rebooting';X='No update necessary';V='Update end UTC';U='ERROR!!';T='Update has not yet run after boot!';Q='Retrying in 15 seconds!';N='update_log';D=ws;global G;K=d(await D.receive())
	if not K or c not in K or K[c]!=G:await D.send(AO);return A2,401
	try:
		with B(N,P)as I:
			await D.send('Previous run -----------')
			try:
				for F in I:await D.send(F)
			except:pass
			await D.send('End previous run -------')
	except:await D.send(T)
	L=d(await D.receive())
	try:
		if L['auto']:
			try:W.remove(AK);await D.send('Automatic update enabled!')
			except:await D.send('Automatic update was already enabled!')
		else:
			with B(AK,O)as I:I.write('1')
			await D.send('Automatic update disabled!')
	except:pass
	try:
		if L['tail']:
			try:
				with B(N)as I:
					for F in I:await D.send(F)
					if U in F or V in F or X in F or Z in F:await D.send(AO);return M
			except:await D.send(T);await D.send(AO);return M
			R='';S=0
			while H:
				try:
					with B(N)as I:
						for F in I:pass
						if R==F.encode():S+=1
						R=F.encode();await D.send(F)
						if S>7 or U in F or V in F or X in F or Z in F:await D.send(AO);return M
				except:break
				await A.sleep(5)
	except:pass
	try:
		if L['start']:
			J=await f.update(Y,D,debug=E or C)
			if not J:
				await D.send(Q);await A.sleep(15);J=await f.update(Y,D,debug=E or C)
				if not J:
					await D.send(Q);await A.sleep(15);J=await f.update(Y,D,debug=E or C)
					if not J:await D.send(Q);await A.sleep(15);J=await f.update(Y,D,debug=E or C)
	except:await D.send(AO);return M
@j.route('/debug',methods=[Ae,i])
async def Cf(request):
	A=request;global E;global G;C=A.headers
	if not C or c not in C or C[c]!=G:return A2,401
	I=A.json
	if I[m]:
		with B(m,O)as J:J.write('1')
		D={K:{AN:'Debugging enabled, please connect the device to a computer to start!'}};F=r(body=D,status_code=200,headers={p:AC});E=H
	else:
		try:W.remove(m)
		except:pass
		D={K:{AN:'Debugging disabled!'}};F=r(body=D,status_code=200,headers={p:AC});E=M
	if A.method==i:F.headers[A1]=Ad
	return F
@j.route('/poll',methods=['GET',i])
async def Cg(request):
	B=request;global X,E,C;global G;A=B.headers
	if not A or c not in A or A[c]!=G:return A2,401
	F={K:{x:{n:X,m:E or C},Q:await Z()}};D=r(body=F,status_code=200,headers={p:AC});await S()
	if B.method==i:D.headers[A1]=C1
	return D
@j.route('/metrics')
@Bb
async def Ch(request,ws):
	O='Sent data through WebSocket';D=ws;global X,E,C;global G;I=d(await D.receive())
	if not I or c not in I or I[c]!=G:await D.send(AO);return A2,401
	J=0;B=e({K:{x:{n:X,m:E or C},Q:await Z()}});await D.send(B);await S()
	if E or C:F(O)
	del B;L.collect();M=await AQ()
	while H:
		N=await AQ()
		if M!=N:
			B=e({K:{x:{n:X,m:E or C},Q:await Z(N)}});M=N;L.collect();await D.send(B);await S()
			if E or C:F(O)
			L.collect()
		if J<30:J+=1
		else:
			J=0;L.collect();B=e({K:{x:{n:X,m:E or C},Q:await Z(M)}});await D.send(B);await S()
			if E or C:F(O)
		await A.sleep(2)
@j.route('/local',methods=['GET',i])
async def Ci(request):
	A=r(body='OK',status_code=200,headers={p:Ba});await S()
	if request.method==i:A.headers[A1]=C1
	return A
@j.errorhandler(404)
async def Cj(request):A=r(body='No such endpoint!',status_code=404,headers={p:Ba,BZ:'*'});await Aj();return A
@j.errorhandler(Br)
async def Ck(request,exception):
	A=exception
	if not(E or C):A='Runtime error! Restarting the device is advisable!'
	B=r(body=A,status_code=500,headers={p:Ba,BZ:'*'});await Aj();return B
async def Aq():
	global C;A.create_task(CK());D=ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
	try:
		with B(Af,'rb'):pass
		with B(Ag,'rb'):pass
		D.load_cert_chain(Af,Ag)
		if E or C:F('Using IP specific API certificate')
	except:
		D.load_cert_chain(Bx,By)
		if E or C:F('Using default API certificate')
	await j.start_server(port=443,debug=E or C,ssl=D)
def CP(topic,msg,retained):
	H=msg;A.create_task(S())
	try:
		H=H.decode()
		if E or C:F('MQTT message received: '+H)
		D=d(H)
		if K in D:
			try:D=D[K][Ac]
			except:
				D=D[K]
				if J not in D and N not in D and AJ not in D:D={}
		elif Ac in D:D=D[Ac]
		if J not in D and N not in D and AJ not in D:D={}
		if D:
			for I in D:
				if I==AJ:
					with B(Ab,'a')as G:G.write(e({I:B4}));G.write(U)
				else:
					for M in D[I]:
						with B(Ab,'a')as G:G.write(e({I:{M:B4}}));G.write(U)
			L.collect()
		A.create_task(AF(unit_state=D))
	except t as O:
		if E or C:F(O)
		pass
async def CQ(client):global Bn;await client.subscribe(Bn,1)
async def CR(client,anon_hash=B4):
	i='Collected anonymized data';c='anon_hash';b='Sent state to MQTT broker';a='last_mqtt_update';Y=anon_hash;U=client;global AG,Ar;f=0;R=0;G={K:{Q:await Z()}};G=e(G);await U.publish(AG,G,qos=1);await S()
	if E or C:F(b)
	with B(a,O)as T:T.write(I(q()))
	G=d(G)
	if N in G[K][Q]or J in G[K][Q]or B2 in G[K][Q]:
		if Y:
			M=await BF(G);M.update({c:Y});M=e(M);await U.publish(Ar,M,qos=1);await S();del M
			if E or C:F(i)
			L.collect()
	del G;L.collect();G={K:{Q:await Z()}};G=e(G);await U.publish(AG,G,qos=1);await S()
	if E or C:F(b)
	with B(a,O)as T:T.write(I(q()))
	del G;V=await AQ()
	while H:
		X=await AQ()
		if V!=X:
			g=CF(V,X);R+=1
			if V[J][o]<=1200:
				if R<=6:G={K:{Q:await Z(g)}}
				else:R=0;G={K:{Q:await Z(X)}}
			elif R<=4:G={K:{Q:await Z(g)}}
			else:R=0;G={K:{Q:await Z(X)}}
			V=X;del X;L.collect();G=e(G);await U.publish(AG,G,qos=1);await S()
			if E or C:F('Sent new state to MQTT broker')
			with B(a,O)as T:T.write(I(q()))
			try:
				with B(Ab,P)as j:
					for h in j:
						k=e({K:{Ac:d(h)}});await U.publish(AG,k,qos=1)
						if E or C:F('Deleted desired values from MQTT broker: '+h)
						await A.sleep(1)
				try:W.remove(Ab)
				except:pass
			except:pass
			G=d(G)
			if N in G[K][Q]or J in G[K][Q]or B2 in G[K][Q]:
				if Y:M=await BF(G);M.update({c:Y});M=e(M);await U.publish(Ar,M,qos=1);await S();del M;L.collect()
			del G;L.collect()
		with B(a)as T:l=D(T.read())
		if q()-l>900:
			del X;L.collect()
			if V[J][o]<=1200:
				if R<=6:G={K:{Q:await Z({})}}
				else:R=0;G={K:{Q:await Z(V)}}
			elif R<=3:G={K:{Q:await Z({})}}
			else:R=0;G={K:{Q:await Z(V)}}
			G=e(G);await U.publish(AG,G,qos=1);await S()
			if E or C:F(b)
			with B(a,O)as T:T.write(I(q()))
			if Y:
				M=await BF(d(G));M.update({c:Y});M=e(M);await U.publish(Ar,M,qos=1);await S();del M;L.collect()
				if E or C:F(i)
			del G;L.collect()
		if f<2160:f+=1
		else:raise t
		await A.sleep(5)
async def Bo(anon_hash=B4):
	global As
	while H:
		try:await A.wait_for(As.connect(),50);await CR(As,anon_hash)
		except t as B:
			await Aj()
			if E or C:F(B)
			try:As.close()
			except:pass
			if E or C:F('MQTT connection closed, reconnecting...')
			await A.sleep(10)
		L.collect()
h.feed()
BJ=M
BG=M
try:
	with B(BT,P)as Bp:BK=Bp.readline().strip();BL=Bp.readline().strip()
	BJ=H
	if E or C:F('WiFi file available')
	Be(BK,BL)
	if E or C:F('WiFi connected')
	BM=B6()
	try:
		with B(AM)as BN:AH=d(BN.read());W.remove(AM)
		A6=BM.post(B1,json={BU:AH})
	except:pass
	try:
		with B(BV,P)as CS:Bq=CS.read()
		if E or C:F('Customer id_token found, requesting certificates')
		h.feed()
		try:
			with B(AB,P)as A7:l=d(A7.read())
			AH={w:l[w],AL:l[AL]}
		except:
			try:
				with B(AM)as BN:AH=d(BN.read())
				W.remove(AM)
			except:AH=M
		if AH:AT={'token':Bq,C2:g,Aa:a,AM:AH}
		else:AT={'token':Bq,C2:g,Aa:a}
		A6=BM.post(B1,json=AT)
		if A6:
			l=A6.json()
			if E or C:F('Got user certificates')
			with B(AB,O)as A7:A7.write(e(l))
			W.remove(BV)
	except:pass
	CT=R.WLAN(R.STA_IF);At=CT.ifconfig()[0]
	try:
		with B(C3)as BO:CU=BO.read()
		if At!=CU:raise t
		with B(Af):pass
		with B(Ag):pass
	except:
		try:
			try:
				if u:AT={'ip':At,Aa:a,Bs:u}
				else:raise t
			except:AT={'ip':At,Aa:a}
			A6=BM.post('https://httpscert.amastech.cloud/',json=AT)
			if A6:
				if E or C:F('Got API certificates')
				Au=A6.json()
				if E or C:F(Au)
				with B(Af,B3)as CV:CV.write(AD(Au[BX]))
				with B(Ag,B3)as CW:CW.write(AD(Au[BY]))
				with B(C3,O)as BO:BO.write(At)
				del Au,A6;L.collect()
				if E or C:F('Saved API certificates')
		except:
			try:W.remove(Af)
			except:pass
			try:W.remove(Ag)
			except:pass
	try:
		with B(AB,P)as A7:l=d(A7.read())
		if E or C:F('Using saved device certificates')
		AI=l[w]
		if E or C:F(C4+AI)
		CX=AD(l[BX]);CY=AD(l[BY]);del l;A3=0;A4=0;Bn=C5+AI+C6+g+'/update/delta';AG=C5+AI+C6+g+C0;Ar='$aws/rules/anonymized_data_collection/device/'+g+'/data';s['server']='iotcore.amastech.cloud';s['port']=8883;s['client_id']=g.encode();s['ssid']=BK;s['wifi_pw']=BL;s['ssl']=H;s['ssl_params']={'cert':CX,'key':CY};s['subs_cb']=CP;s['connect_coro']=CQ;Bc.DEBUG=C or E;As=Bc(s);A.create_task(AF());L.collect();h.feed()
		if E or C:F(B5)
		A.create_task(Bl());A.create_task(Ap());A.create_task(Bm());A.create_task(Ak());A.create_task(Al());A.create_task(An());A.create_task(Am());A.create_task(Ao());h.feed()
		try:
			with B(B0)as Cl:
				if E or C:F('No telemtry selected')
			A.create_task(Bo())
		except:
			if E or C:F('Anonymous telemetry selected')
			with B(n)as AS:X=AS.read()
			BP=CB.sha256((AI+g+X).encode());BP=hexlify(BP.digest())
			if E or C:F('Got hash, continuing with services')
			A.create_task(Bo(BP))
		h.feed();A.run(Aq())
	except:
		A3=0;A4=1;L.collect();A.create_task(AF())
		if E or C:F(B5)
		h.feed();A.create_task(Bl());A.create_task(Ap());A.create_task(Bm());A.create_task(Ak());A.create_task(Al());A.create_task(CJ());A.create_task(An());A.create_task(Am());A.create_task(Ao());h.feed();A.run(Aq())
except:
	BG=H
	try:
		with B(AB,P)as A7:l=d(A7.read())
		AI=l[w]
		if E or C:F(C4+AI)
		del l;A3=1;A4=0;BB();A.create_task(AF())
		if E or C:F(B5)
		h.feed()
		if BJ:
			if E or C:F(C7)
			A.create_task(Ap())
		A.create_task(Ak());A.create_task(Al());A.create_task(CI());A.create_task(An());A.create_task(Am());A.create_task(Ao());h.feed();A.run(Aq())
	except:
		A3=1;A4=1;BB();A.create_task(AF())
		if E or C:F(B5)
		h.feed()
		if BJ:
			if E or C:F(C7)
			A.create_task(Ap())
		A.create_task(Ak());A.create_task(Al());A.create_task(CH());A.create_task(An());A.create_task(Am());A.create_task(Ao());h.feed();A.run(Aq())