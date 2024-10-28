import configparser
from flask import Flask, jsonify, request
import influxdb_client

from influxdb_client.client.write_api import SYNCHRONOUS


config = configparser.ConfigParser()
config.read('../configuration/config.ini')


# creating a Flask app 
app = Flask(__name__) 

# on the terminal type: curl http://127.0.0.1:5000/ 
# returns hello world when we use GET. 
# returns the data that we send when we use POST. 
@app.route('/add', methods = ['POST']) 
def home():
	data = request.get_json()
	print("Received JSON data:", data.keys())
	
	client = influxdb_client.InfluxDBClient(url = config.get('APP', 'INFLUX_URL'), token=config.get('APP', 'INFLUX_TOKEN'), org=config.get('APP', 'INFLUX_ORG'))
	write_api = client.write_api(write_options=SYNCHRONOUS)
	point = influxdb_client.Point("g-nodeb").tag("Short name", data.get('Short name', "Cell-Name-01")) \
	      .field("DL Effective Throughput [Mbps]", data.get('DL Effective Throughput [Mbps]', 1024.0))\
            .field("UL Effective Throughput [Mbps]", data.get('UL Effective Throughput [Mbps]', 2048))\
            .field("DL Volume (GB)", data.get('DL Volume (GB)', 100000))\
            .field("UL Volume (GB)", data.get('UL Volume (GB)', 40000))\
            .field("RRC.ConnMean", data.get('RRC.ConnMean', 0.000001))\
            .field("Avg. CQI", data.get('Avg. CQI', 10.0))\
            .field("Avg. DL PRB Utilization", data.get('Avg. DL PRB Utilization', 1.0))\
            .field("Avg. UL PRB Utilization", data.get('Avg. UL PRB Utilization', 1.0))\
            .field("DRB.PacketLossRateUl", data.get('DRB.PacketLossRateUl', 0.0))\
            .field("RRC Connection Success Rate (%)", data.get('RRC Connection Success Rate (%)', 100.0))\
            .field("RRC Drop Rate (Session Drop Rate, %)", data.get('RRC Drop Rate (Session Drop Rate, %)', 0.0))\
            .field("HO Success Rate (%)", data.get('HO Success Rate (%)', 100.0))\
            .field("RRE Success Rate (%)", data.get('RRE Success Rate (%)', 100.0))
	write_api.write(bucket=config.get('APP', 'INFLUX_BUCKET'), org=config.get('APP', 'INFLUX_ORG'), record=point)
	return jsonify({'data': data}) 
 


# driver function 
if __name__ == '__main__': 
	app.run(debug = True) 
