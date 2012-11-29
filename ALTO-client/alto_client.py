import httplib
import sys

hostname = sys.argv[1]

print "---------------------------"
# Directory
headers = {"Accept": "application/alto-networkmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("GET", "/directory", None, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data

headers = {"Accept": "application/alto-directory+json, application/alto-error+json"}
#conn = httplib.HTTPConnection("p4p-1.cs.yale.edu:6671")
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("GET", "/networkmap", None, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data
conn.close()

print "----------------------------"
params = "{ \r\n \t \"pids\": \r\n \t\t [ \"mypid2\", \"asldkj\", \"mypid1\" ] \t\t\t \r\n }"
headers = {"Content-type": "application/alto-networkmapfilter+json", "Accept": "application/alto-networkmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/networkmap/filtered", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
conn.close()
print data

print "----------------------------"
params = "{ \r\n \t \"pids\": \r\n \t\t [ \"0.i.interop2012\", \"200.e.interop2012\" ] \t\t\t \r\n }"
headers = {"Content-type": "application/alto-networkmapfilter+json", "Accept": "application/alto-networkmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/networkmap/filtered", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
conn.close()
print data

print "/costmap/numerical/routingcost"
# Cost Map
headers = {"Accept": "application/alto-costmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("GET", "/costmap/numerical/routingcost", None, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data

print "/costmap/ordinal/routingcost"
# Cost Map
headers = {"Accept": "application/alto-costmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("GET", "/costmap/ordinal/routingcost", None, headers)
response = conn.getresponse()
print(response.status, response.reason)
data = response.read()
print response.getheaders();
print data

print "----------------------------"
params = "{\"cost-mode\" : \"numerical\",\"cost-type\" : \"routingcost\",\"pids\" : {\"srcs\" : [ \"mypid1\", \"mypid3\" ],\"dsts\" : [ \"mypid2\", \"peeringpid1\", \"transitpid2\" ]}}"
headers = {"Content-type": "application/alto-costmapfilter+json", "Accept": "application/alto-costmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/costmap/filtered", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
conn.close()
print data

print "----------------------------"
params = "{\"properties\" : [ \"pid\" ],\"endpoints\" : [ \"ipv6:1234::192.168.1.23\",\"ipv6:2001:DB8::2345:5678\", \"ipv4:201.1.13.12\"]}"
headers = {"Content-type": "application/alto-endpointpropparams+json", "Accept": "application/alto-endpointprop+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/property", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
conn.close()
print data

print "----------------------------"
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
headers = {"Content-type": "application/alto-endpointcostparams+json", "Accept": "application/alto-endpointcost+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/endpoints/cost", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
conn.close()
print data

print "##################################"
print "----------------------------"
params = "{\"cost-mode\" : \"numerical\",\"cost-type\" : \"routingcost\",\"pids\" : {\"srcs\" : [ \"mypid1\", \"mypid3\" ],\"dsts\" : [ \"mypid2\", \"peeringpid1\", \"transitpid2\" ]}}"
headers = {"Content-type": "application/alto-costmapfilter+json", "Accept": "application/alto-costmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/foo/foo", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
conn.close()
print data

print "----------------------------"
params = "{\"cost-mode\" : \"numerical\",\"cost-type\" : \"routingcost\",\"pids\" : {\"srcs\" : [ \"mypid1\", \"mypid3\" ],\"dsts\" : [ \"mypid2\", \"peeringpid1\", \"transitpid2\" ]}"
headers = {"Content-type": "application/alto-costmapfilter+json", "Accept": "application/alto-costmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/costmap/filtered", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
conn.close()
print data

print "----------------------------"
params = "{\"cost-mode\" : \"numerical\",\"cost-type\" : \"foo\",\"pids\" : {\"srcs\" : [ \"mypid1\", \"mypid3\" ],\"dsts\" : [ \"mypid2\", \"peeringpid1\", \"transitpid2\" ]}}"
headers = {"Content-type": "application/alto-costmapfilter+json", "Accept": "application/alto-costmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/costmap/filtered", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
conn.close()
print data

print "----------------------------"
params = "{\"cost-mode\" : \"numerical\",\"cost-type\" : \"routingcost\",\"pids\" : {\"srcs\" : \"mypid1\",\"dsts\" : [ \"mypid2\", \"peeringpid1\", \"transitpid2\" ]}}"
headers = {"Content-type": "application/alto-costmapfilter+json", "Accept": "application/alto-costmap+json, application/alto-error+json"}
conn = httplib.HTTPConnection(hostname + ":6671")
conn.request("POST", "/costmap/filtered", params, headers)
response = conn.getresponse()
print response.status, response.reason
data = response.read()
conn.close()
print data


