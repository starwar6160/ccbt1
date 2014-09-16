#'#coding=utf-8'
import HidProtocol as elk
import time
rstr0000="""
<?xml version="1.0" encoding="utf-8"?><root><TransCode>0000</TransCode><TransName>CallForActInfo</TransName><TransDate>20140807</TransDate><TransTime>173828</TransTime><DevCode>12345698Z</DevCode><SpareString1>NULL</SpareString1><SpareString2>NULL</SpareString2></root>
"""
r05="""
<?xml version="1.0" encoding="utf-8"?>
<root><TransCode>0005</TransCode><TransName>ReadLog</TransName><TransDate>20140916</TransDate><TransTime>135824</TransTime><BeginNo>0</BeginNo><EndNo>23</EndNo></root>
"""

count=0
elk.Open(5)
while count<5:
	print ("*******************************************",count)
	elk.Notify(r05)
	time.sleep(2)	
	rexml="aa"
	while rexml!="NODATA916":
		rexml=elk.dbgGetLockReturnXML()
		if rexml!="NODATA916":
			print("###")
			print(rexml)
		time.sleep(1)
	count=count+1
elk.Close()