import httplib
import sys

hostname = sys.argv[1]

print "------------ Test cases for Interop 2012 ------------"
print "--- http://tools.ietf.org/pdf/draft-gurbani-alto-interop-cases-02.pdf ---\n\n"


print "------------ Test-IRD-1 ------------\n"
# Pass
headers = {"Accept": "application/alto-directory+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("GET", "/directory", None, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-EPS-1 ------------\n"
# Pass
params = "{\
            \"properties\": [ \"pid\" ],\
            \"endpoints\": [ \"ipv4:192.168.1.23\"]\
        }"
headers = {"Accept": "application/alto-endpointprop+json", "Content-Type": "application/alto-endpointpropparams+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/property", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-EPS-2 ------------\n"
# Pass
params = "{\
            \"properties\": [ \"pid\" ],\
            \"endpoints\": [ \"ipv4:192.168.10.23\"]\
        }"
headers = {"Accept": "application/alto-endpointprop+json", "Content-Type": "application/alto-endpointpropparams+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/property", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-EPS-3 ------------\n"
# Pass
params = "{\
            \"properties\": [ \"pid\" ],\
            \"endpoints\": [ \"ipv4:201.1.13.12\"]\
        }"
headers = {"Accept": "application/alto-endpointprop+json", "Content-Type": "application/alto-endpointpropparams+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/property", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-EPS-4 ------------\n"
# Pass
params = "{\
            \"properties\": [ \"pid\" ],\
            \"endpoints\": [ \"ipv6:1234::192.168.1.23\",\
				\"ipv4:132.0.10.12\" ]\
        }"
headers = {"Accept": "application/alto-endpointprop+json", "Content-Type": "application/alto-endpointpropparams+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/property", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-ECS-1 ------------\n"
# PASS
params = "{\
        \"cost-mode\" : \"numerical\",\
          \"cost-type\" : \"routingcost\",\
          \"endpoints\" : {\
            \"srcs\": [ \"ipv4:10.0.0.0\", \"ipv4:192.168.11.0\",\
                      \"ipv4:192.168.10.0\"],\
            \"dsts\": [\
              \"ipv4:10.0.0.0\",\
              \"ipv4:15.0.0.0\",\
              \"ipv4:192.168.11.0\",\
              \"ipv4:192.168.10.0\",\
              \"ipv4:128.0.0.0\",\
              \"ipv4:130.0.0.0\",\
              \"ipv4:0.0.0.0\",\
              \"ipv4:132.0.0.0\",\
              \"ipv4:135.0.0.0\"\
            ]\
          }\
        }"
headers = {"Content-type": "application/alto-endpointcostparams+json", "Accept": "application/alto-endpointcost+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/cost", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-ECS-2 ------------\n"
# PASS
params = "{\
        \"cost-mode\" : \"ordinal\",\
          \"cost-type\" : \"routingcost\",\
          \"endpoints\" : {\
            \"srcs\": [ \"ipv6:2001:DB8::ABCD:6789\", \"ipv4:192.168.10.1\" ],\
            \"dsts\": [\
		\"ipv6:2001:DB8::2345:5678\",\
		\"ipv4:135.0.29.1\",\
		\"ipv4:192.168.10.23\"\
		]\
          }\
        }"
headers = {"Content-type": "application/alto-endpointcostparams+json", "Accept": "application/alto-endpointcost+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/cost", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-ECS-3 ------------\n"
# PASS 
params = "{\
	\"constraints\": [\"le 5\", \"ge 4\"],\
        \"cost-mode\" : \"numerical\",\
          \"cost-type\" : \"routingcost\",\
          \"endpoints\" : {\
            \"srcs\": [ \"ipv4:10.0.0.0\", \"ipv4:192.168.11.0\", \"ipv4:192.168.10.0\" ],\
            \"dsts\": [\
		\"ipv4:10.0.0.0\",\
		\"ipv4:15.0.0.0\",\
		\"ipv4:192.168.11.0\",\
		\"ipv4:192.168.10.0\",\
		\"ipv4:128.0.0.0\",\
		\"ipv4:130.0.0.0\",\
		\"ipv4:0.0.0.0\",\
		\"ipv4:132.0.0.0\",\
		\"ipv4:135.0.0.0\"\
		]\
          }\
        }"
headers = {"Content-type": "application/alto-endpointcostparams+json", "Accept": "application/alto-endpointcost+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/cost", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-MAPS-1 ------------\n"
# PASS
headers = {"Accept": "application/alto-networkmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("GET", "/networkmap", None, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data
conn.close()


print "\n\n\n------------ Test-MAPS-2 ------------\n"
# PASS
headers = {"Accept": "application/alto-costmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("GET", "/costmap/numerical/routingcost", None, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data

print "\n\n\n------------ Test-MAPS-3 ------------\n"
# PASS
headers = {"Accept": "application/alto-costmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("GET", "/costmap/ordinal/routingcost", None, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-FILTER-1 ------------\n"
# PASS
params = "{\
	\"pids\": [ \"mypid2\" ]\
	}"
headers = {"Content-type": "application/alto-networkmapfilter+json", "Accept": "application/alto-networkmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/networkmap/filtered", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
conn.close()
print response.getheaders();
print data


print "\n\n\n------------ Test-FILTER-2 ------------\n"
# PASS
params = "{\"cost-mode\" : \"numerical\",\"cost-type\" : \"routingcost\",\"pids\" : {\"srcs\" : [ \"mypid1\", \"mypid3\" ],\"dsts\" : [ \"mypid2\", \"peeringpid1\", \"transitpid2\" ]}}"
headers = {"Content-type": "application/alto-costmapfilter+json", "Accept": "application/alto-costmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/costmap/filtered", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
conn.close()
print response.getheaders();
print data


print "\n\n\n------------ Test-JSON-ERROR-1 ------------\n"
# PASS
params = "{\
        \"cost-mode\" : \"numberical\",\
          \"cost-type\" : \"routingcost\",\
          \"endpoints\" : {\
            \"srcs\": [ \"ipv4:10.0.0.0\" ],\
            \"dsts\": [ \"ipv4:10.0.0.0\" ]\
        }"
headers = {"Content-type": "application/alto-endpointcostparams+json", "Accept": "application/alto-endpointcost+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/cost", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-JSON-ERROR-2 ------------\n"
# PASS
params = "{\
        \"cost-mode\" : \"numerical\",\
          \"cost-type\" : \"routingcost\",\
          \"endpoints\" : {\
            \"srcs\": [ \"ipv4:192.168.10.1\" ]\
		}\
        }"
headers = {"Content-type": "application/alto-endpointcostparams+json", "Accept": "application/alto-endpointcost+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/cost", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-JSON-ERROR-3 ------------\n"
# PASS
params = "{\
        \"cost-mode\" : \"numerical\",\
          \"cost-type\" : \"routingcost\",\
          \"endpoints\" : {\
            	\"srcs\": \"ipv4:10.0.0.0\",\
		\"dsts\": [ \"ipv4:10.0.0.0\" ]\
		}\
        }"
headers = {"Content-type": "application/alto-endpointcostparams+json", "Accept": "application/alto-endpointcost+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/cost", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-JSON-ERROR-4 ------------\n"
# PASS
params = "{\
        \"cost-mode\" : \"foo\",\
          \"cost-type\" : \"routingcost\",\
          \"pids\" : {\
            	\"srcs\": [],\
		\"dsts\": []\
		}\
        }"
headers = {"Content-type": "application/alto-costmapfilter+json", "Accept": "application/alto-costmap+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/costmap/filtered", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-JSON-ERROR-5 ------------\n"
# PASS
params = "{\
        \"cost-mode\" : \"numerical\",\
          \"cost-type\" : \"foo\",\
          \"pids\" : {\
            	\"srcs\": [],\
		\"dsts\": []\
		}\
        }"
headers = {"Content-type": "application/alto-costmapfilter+json", "Accept": "application/alto-costmap+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/costmap/filtered", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-JSON-ERROR-6 ------------\n"
# PASS
params = "{\
            	\"endpoints\": [\"ipv4:10.0.0.1\"],\
		\"properties\": [\"foo\"]\
        }"
headers = {"Content-type": "application/alto-endpointpropparams+json", "Accept": "application/alto-endpointprop+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/property", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data


print "\n\n\n------------ Test-JSON-ERROR-7 ------------\n"
# PASS
params = "{\
        \"cost-mode\" : \"bar\",\
          \"cost-type\" : \"foo\",\
          \"pids\" : {\
            	\"srcs\": [],\
		\"dsts\": []\
		}\
        }"
headers = {"Content-type": "application/alto-costmapfilter+json", "Accept": "application/alto-costmap+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/costmap/filtered", params, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data

